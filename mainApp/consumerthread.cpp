#include "consumerthread.h"
#include <ctime>
#include <locale>
#include <QThread>
#include <QString>

int flip_flag = 1;

ConsumerThread::ConsumerThread(std::vector<OutputStream*>& streams, 
    NvEglRenderer* renderer) :
    m_streams(streams),
    m_compositedFrame(0),
    m_JpegEncoder(NULL),
    m_OutputBuffer(NULL),
    m_renderer(renderer)
{
    threadInitialize();
    sbsBufferInitialize();
}


ConsumerThread::~ConsumerThread()
{
    if (m_compositedFrame)
        NvBufferDestroy(m_compositedFrame);

    for (uint32_t i = 0; i < m_streams.size(); i++)
        if (m_dmabufs[i])
            NvBufferDestroy(m_dmabufs[i]);

    if (m_JpegEncoder)
        delete m_JpegEncoder;

    if (m_OutputBuffer)
        delete[] m_OutputBuffer;
}

bool ConsumerThread::threadInitialize()
{
    NvBufferRect dstCompRect[2];
    int32_t spacing = 10;
    NvBufferCreateParams input_params = { 0 };

    // Initialize destination composite rectangles
    // The window layout is as below
    int32_t cellWidth = CAPTURE_SIZE.width() / 4 ;
    int32_t cellHeight = CAPTURE_SIZE.height() / 4;

    dstCompRect[0].top = 0;
    dstCompRect[0].left = 0;
    dstCompRect[0].width = CAPTURE_SIZE.width();
    dstCompRect[0].height = CAPTURE_SIZE.height();

    dstCompRect[1].top = spacing;
    dstCompRect[1].left = CAPTURE_SIZE.width() - cellWidth - spacing;
    dstCompRect[1].width = cellWidth;
    dstCompRect[1].height = cellHeight;

    /* Allocate composited buffer */
    input_params.payloadType = NvBufferPayload_SurfArray;
    input_params.width = CAPTURE_SIZE.width();
    input_params.height = CAPTURE_SIZE.height();
    input_params.layout = NvBufferLayout_Pitch;
    input_params.colorFormat = NvBufferColorFormat_NV12;
    input_params.nvbuf_tag = NvBufferTag_VIDEO_CONVERT;

    NvBufferCreateEx(&m_compositedFrame, &input_params);
    if (!m_compositedFrame)
        ORIGINATE_ERROR("Failed to allocate composited buffer");

    /* Initialize composite parameters */
    memset(&m_compositeParam, 0, sizeof(m_compositeParam));
    m_compositeParam.composite_flag = NVBUFFER_COMPOSITE;
    m_compositeParam.input_buf_count = m_streams.size();
    memcpy(m_compositeParam.dst_comp_rect, dstCompRect,
        sizeof(NvBufferRect) * m_compositeParam.input_buf_count);
    for (uint32_t i = 0; i < 2; i++)
    {
        m_compositeParam.dst_comp_rect_alpha[i] = 1.0f;
        m_compositeParam.src_comp_rect[i].top = 0;
        m_compositeParam.src_comp_rect[i].left = 0;
        m_compositeParam.src_comp_rect[i].width = CAPTURE_SIZE.width();
        m_compositeParam.src_comp_rect[i].height = CAPTURE_SIZE.height();
    }

    /* Initialize buffer handles. Buffer will be created by FrameConsumer */
    memset(m_dmabufs, 0, sizeof(m_dmabufs));

    /* Create the FrameConsumer */
    for (uint32_t i = 0; i < m_streams.size(); i++)
    {
        m_consumers[i].reset(FrameConsumer::create(m_streams[i]));
    }



    m_OutputBuffer = new unsigned char[JPEG_BUFFER_SIZE];
    if (!m_OutputBuffer)
        return false;

    m_JpegEncoder = NvJPEGEncoder::createJPEGEncoder("jpenenc");
    if (!m_JpegEncoder)
        ORIGINATE_ERROR("Failed to create JPEGEncoder.");

    if (DO_STAT)
        m_JpegEncoder->enableProfiling();

    return true;
}

bool ConsumerThread::sbsBufferInitialize()
{
    NvBufferRect dstCompRect[2];
    NvBufferCreateParams input_params = { 0 };

    dstCompRect[0].top = 0;
    dstCompRect[0].left = 0;
    dstCompRect[0].width = CAPTURE_SIZE.width();
    dstCompRect[0].height = CAPTURE_SIZE.height();

    dstCompRect[1].top = 0;
    dstCompRect[1].left = CAPTURE_SIZE.width();
    dstCompRect[1].width = CAPTURE_SIZE.width();
    dstCompRect[1].height = CAPTURE_SIZE.height();

    /* Allocate composited buffer */
    input_params.payloadType = NvBufferPayload_SurfArray;
    input_params.width = CAPTURE_SIZE.width() * 2;
    input_params.height = CAPTURE_SIZE.height();
    input_params.layout = NvBufferLayout_Pitch;
    input_params.colorFormat = NvBufferColorFormat_YUV420;
    input_params.nvbuf_tag = NvBufferTag_VIDEO_CONVERT;

    NvBufferCreateEx(&m_sbsFrame, &input_params);
    if (!m_sbsFrame)
        ORIGINATE_ERROR("Failed to allocate sbs buffer");

    /* Initialize composite parameters */
    memset(&m_sbsParam, 0, sizeof(m_sbsParam));
    m_sbsParam.composite_flag = NVBUFFER_COMPOSITE;
    m_sbsParam.input_buf_count = m_streams.size();
    memcpy(m_sbsParam.dst_comp_rect, dstCompRect,
        sizeof(NvBufferRect) * m_sbsParam.input_buf_count);
    for (uint32_t i = 0; i < 2; i++)
    {
        m_sbsParam.dst_comp_rect_alpha[i] = 1.0f;
        m_sbsParam.src_comp_rect[i].top = 0;
        m_sbsParam.src_comp_rect[i].left = 0;
        m_sbsParam.src_comp_rect[i].width = CAPTURE_SIZE.width();
        m_sbsParam.src_comp_rect[i].height = CAPTURE_SIZE.height();
    }


    return true;
}


bool ConsumerThread::consumerInitialize()
{
    for (uint32_t i = 0; i < m_streams.size(); i++)
    {
        iEglOutputStreams[i] = interface_cast<IEGLOutputStream>(m_streams[i]);
        iFrameConsumers[i] = interface_cast<IFrameConsumer>(m_consumers[i]);
        if (!iFrameConsumers[i])
            CONSUMER_PRINT("Failed to get IFrameConsumer interface\n");

        /* Wait until the producer has connected to the stream */
        CONSUMER_PRINT("Waiting until producer is connected...\n");
        if (iEglOutputStreams[i]->waitUntilConnected() != STATUS_OK)
            CONSUMER_PRINT("Stream failed to connect.\n");
        CONSUMER_PRINT("Producer has connected; continuing.\n");
    }
}


bool ConsumerThread::fetchFrame(int deviceID, bool showInfo, bool modePreview)
{
    // if (showInfo)
    //     CONSUMER_PRINT("fetching frame of device %d\n", deviceID);
    int i = deviceID;
    uint64_t frameNumber;
    uint64_t timeStamp;


    /* Acquire a frame */
    uint64_t timeout = modePreview ? 1e8 : TIMEOUT_INFINITE;
    UniqueObj<Frame> frame(iFrameConsumers[i]->acquireFrame(timeout));
    IFrame* iFrame = interface_cast<IFrame>(frame);
    if (!iFrame){
        CONSUMER_PRINT("Fetching %s capture frame timeout ...\n",  i ? "Right" : "Left");
        if (i == 0) leftFrameEmpty = true;
        else rightFrameEmpty = true;
        return false;          // switch to next itr
    }
    if (i == 0) leftFrameEmpty = false;
    else rightFrameEmpty = false;
    frameNumber = iFrame->getNumber();
    timeStamp   = iFrame->getTime(); 
    if (showInfo){
        IArgusCaptureMetadata *iArgusMetadata = interface_cast<IArgusCaptureMetadata>(frame);
        CaptureMetadata* metadata = iArgusMetadata->getMetadata();
        ICaptureMetadata* iMetadata = interface_cast<ICaptureMetadata>(metadata);
        uint64_t kernalTime = iMetadata->getSensorTimestamp();
        CONSUMER_PRINT("%s timestamp: \t%u \t%u\n", 
                        i ? "Right" : "Left", 
                        (unsigned)timeStamp,
                        kernalTime);
    }
    /* Get the IImageNativeBuffer extension interface */
    NV::IImageNativeBuffer* iNativeBuffer =
        interface_cast<NV::IImageNativeBuffer>(iFrame->getImage());
    if (!iNativeBuffer)
        ORIGINATE_ERROR("IImageNativeBuffer not supported by Image.");

    /* If we don't already have a buffer, create one from this image.
        Otherwise, just blit to our buffer */
    if (!m_dmabufs[i])
    {
        m_dmabufs[i] = iNativeBuffer->createNvBuffer(
            iEglOutputStreams[i]->getResolution(),
            NvBufferColorFormat_YUV420,
            NvBufferLayout_BlockLinear,
            flip_flag ? NV::ROTATION_180 : NV::ROTATION_0   // directly retrive rotated image !!!
        );
        if (!m_dmabufs[i])
            CONSUMER_PRINT("\tFailed to create NvBuffer\n");
    }
    else if (
        iNativeBuffer->copyToNvBuffer(
            m_dmabufs[i], 
            flip_flag ? NV::ROTATION_180 : NV::ROTATION_0
        ) != STATUS_OK)
    {
        ORIGINATE_ERROR("Failed to copy frame to NvBuffer.");
    }

}

bool ConsumerThread::startPreview()
{
    if (!iFrameConsumers[0] || !iFrameConsumers[1])
    {
        consumerInitialize();
    }

    preview_mode = true;
    while (preview_mode)
    {
        bool leftResult = fetchFrame(0, false, true);
        bool rightResult = fetchFrame(1, false, true);

        /* Render frame. */
        /* Composite multiple input to one frame */
        if (!m_dmabufs[0] || !m_dmabufs[1])
            continue;
        NvBufferComposite(m_dmabufs, m_compositedFrame, &m_compositeParam);
        m_renderer->render(m_compositedFrame);
        // m_renderer->render(m_dmabufs[0]);
            
    }
    CONSUMER_PRINT("Preview stopped.\n");

    return true;
}


void ConsumerThread::startCapture()
{
    // fetchFrame(0, true, false);
    // fetchFrame(1, true, false);

    capture();
    
}


void ConsumerThread::capture()
{
    CONSUMER_PRINT("creating sbs img file...\n");
    QThread::msleep(100);
    /* composite sbs frame*/
    if (!m_dmabufs[0] || !m_dmabufs[1]){
        CONSUMER_PRINT("Error when compositing SBS frame.\n");
        return ;
    }
    CONSUMER_PRINT("encoding buffer into sbs...\n");
    NvBufferComposite(m_dmabufs, m_sbsFrame, &m_sbsParam);
    CONSUMER_PRINT("encoding successful...\n");

    /* create timestamp for filename */
    std::time_t t = std::time(nullptr);
    char t_format[100];
    std::strftime(t_format, sizeof(t_format), "%m%d%H%M%s", std::localtime(&t));
    std::string filename(t_format);
    filename = "sbs_" + filename + ".jpg";

    std::ofstream* outputFile = new std::ofstream(filename);
    if (outputFile)
    {
        unsigned long size = JPEG_BUFFER_SIZE;
        unsigned char* buffer = m_OutputBuffer;
        m_JpegEncoder->encodeFromFd(m_sbsFrame, JCS_YCbCr, &buffer, size, 95);
        outputFile->write((char*)buffer, size);
        delete outputFile;
    }
    // emit captureSucceed(filename);
    emit captureSucceed(QString::fromStdString(filename));

    preview_mode = true;
}

void ConsumerThread::swtichCaptureMode()
{
    preview_mode = false;
}

bool ConsumerThread::allFramesEmpty()
{
    if (leftFrameEmpty || rightFrameEmpty)
        return true;
    else
        return false;
}
