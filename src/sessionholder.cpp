#include "sessionholder.h"
#include <string>

SessionHolder::SessionHolder()
{
}

SessionHolder::~SessionHolder()
{
    /* Destroy the output stream */
    // left_outputStream.reset();
    // right_outputStream.reset();
}

bool SessionHolder::initialize(std::vector<CameraDevice*> &cameraDevices, ICameraProvider *iCameraProvider, NvEglRenderer* g_renderer)
{
    PRODUCER_PRINT("initializing session holder..\n");

    ICameraProperties* iCameraProperties = interface_cast<ICameraProperties>(cameraDevices[0]);
    if (!iCameraProperties)
        ORIGINATE_ERROR("Failed to get ICameraProperties interface");

    /* Create the capture session using the first device and get the core interface */
    m_captureSession.reset(iCameraProvider->createCaptureSession(cameraDevices));
    if ( !m_captureSession.get() ){
        std::string s = "\nlrCamera size: " + std::to_string(cameraDevices.size());
        ORIGINATE_ERROR(("Failed to create CaptureSession" + s).c_str());
    }

    iCaptureSession = interface_cast<ICaptureSession>(m_captureSession);
    if ( !iCaptureSession )
        ORIGINATE_ERROR("Failed to create iCaptureSession");


    // create general stream setting, OutputStream and EglStream
    UniqueObj<OutputStreamSettings> streamSettings(
        iCaptureSession->createOutputStreamSettings(STREAM_TYPE_EGL));
    IOutputStreamSettings *iStreamSettings = 
        interface_cast<IOutputStreamSettings>(streamSettings);
    IEGLOutputStreamSettings* iEglStreamSettings =
        interface_cast<IEGLOutputStreamSettings>(streamSettings);
    if (!iStreamSettings || !iEglStreamSettings)
        ORIGINATE_ERROR("Failed to create OutputStreamSettings");

    iEglStreamSettings->setPixelFormat(PIXEL_FMT_YCbCr_420_888);


    /* Config Egl Stream setting, capture, left */
    iEglStreamSettings->setResolution(CAPTURE_SIZE);
    iEglStreamSettings->setMetadataEnable(true);
    leftCaptureStream.reset(iCaptureSession->createOutputStream(streamSettings.get()));

    /* Config Egl Stream setting, capture, right */
    iStreamSettings->setCameraDevice(cameraDevices[1]);
    rightCaptureStream.reset(iCaptureSession->createOutputStream(streamSettings.get()));



    /* Create capture request and enable the output stream */
    m_request.reset(iCaptureSession->createRequest());
    IRequest* iRequest = interface_cast<IRequest>(m_request);
    if (!iRequest)
        ORIGINATE_ERROR("Failed to create Request");


    iRequest->enableOutputStream(leftCaptureStream.get());
    iRequest->enableOutputStream(rightCaptureStream.get());

    PRODUCER_PRINT("create session holder ok\n");


    // ISensorMode* iSensorMode;
    // std::vector<SensorMode*> sensorModes;
    // iCameraProperties->getBasicSensorModes(&sensorModes);
    // if (sensorModes.size() == 0)
    //     ORIGINATE_ERROR("Failed to get sensor modes");
    // PRODUCER_PRINT("Total Sensor Modes: %d \n", sensorModes.size());

    // ISourceSettings* iSourceSettings =
    //     interface_cast<ISourceSettings>(iRequest->getSourceSettings());
    // if (!iSourceSettings)
    //     ORIGINATE_ERROR("Failed to get ISourceSettings interface");

    // /* Check and set sensor mode */
    // if (SENSOR_MODE >= sensorModes.size())
    //     ORIGINATE_ERROR("Sensor mode index is out of range");
    // SensorMode* sensorMode = sensorModes[SENSOR_MODE];
    // iSensorMode = interface_cast<ISensorMode>(sensorMode);
    // iSourceSettings->setSensorMode(sensorMode);

    // /* Check fps */
    // Range<uint64_t> sensorDuration(iSensorMode->getFrameDurationRange());
    // Range<uint64_t> desireDuration(1e9 / DEFAULT_FPS + 0.9);
    // if (desireDuration.min() < sensorDuration.min() ||
    //     desireDuration.max() > sensorDuration.max()) {
    //     PRODUCER_PRINT("Requested FPS out of range. Fall back to 30\n");
    //     DEFAULT_FPS = 30;
    // }
    // iSourceSettings->setFrameDurationRange(Range<uint64_t>(1e9 / DEFAULT_FPS));
    // g_renderer->setFPS((float)DEFAULT_FPS);

    return true;
}

