#include "threadworker.h"
#include <QDebug>
#include <QThread>


#include "constants.h"
#include "sessionholder.h"


// --- CONSTRUCTOR ---
ThreadWorker::ThreadWorker() {
    // you could copy data from constructor arguments to internal variables here.

}

// --- DECONSTRUCTOR ---
ThreadWorker::~ThreadWorker() {
    // free resources
    /* Wait for idle */
    iCaptureSession->waitForIdle();

    /* Destroy the capture resources */
    sessionHolder.reset();

    /* Wait for the rendering thread to complete */
    delete consumerThread;

    /* Shut down Argus */
    g_cameraProvider.reset();

    /* Cleanup EGL Renderer */
    delete g_renderer;
}

// --- PROCESS ---
// Start processing data.
bool ThreadWorker::process()
{
    qDebug("Starting camera thread...");

    /* Initialize EGL renderer */
    g_renderer = NvEglRenderer::createEglRenderer("renderer0", STREAM_SIZE.width(),
        STREAM_SIZE.height(), 0, 0);
    if (!g_renderer)
        ORIGINATE_ERROR("Failed to create EGLRenderer.");
    emit xWindowReady(g_renderer->getXWindowId());


    /* Initialize the Argus camera provider */
    g_cameraProvider = UniqueObj<CameraProvider>(CameraProvider::create());
    ICameraProvider* iCameraProvider = interface_cast<ICameraProvider>(g_cameraProvider);
    if (!iCameraProvider)
        ORIGINATE_ERROR("Failed to get ICameraProvider interface");
    printf("Argus Version: %s\n", iCameraProvider->getVersion().c_str());

    /* Get the camera devices */
    std::vector<CameraDevice*> cameraDevices;
    iCameraProvider->getCameraDevices(&cameraDevices);
    if (cameraDevices.size() == 0)
        ORIGINATE_ERROR("No cameras available");

    uint32_t streamCount = cameraDevices.size() < MAX_CAMERA_NUM ?
        cameraDevices.size() : MAX_CAMERA_NUM;
    if (streamCount > g_stream_num)
        streamCount = g_stream_num;
    
    
    /* Initialize sessionHolder  */
    sessionHolder.reset(new SessionHolder);
    if (!sessionHolder.get()->initialize(cameraDevices, iCameraProvider, g_renderer))
        ORIGINATE_ERROR("Failed to initialize Camera session ");

    PRODUCER_PRINT("threadworking reporting \n");

    std::vector<OutputStream*> captureStreams;
    sessionHolder.get()->getCaptureStreams(captureStreams);


    /* Start the consumer thread */
    consumerThread = new ConsumerThread(captureStreams, g_renderer);
    QThread* thread = new QThread;
    consumerThread->moveToThread(thread);
    connect(thread, SIGNAL(started()), consumerThread, SLOT(startPreview()));
    connect(this, SIGNAL(captureSignal()), consumerThread, SLOT(startCapture()));
    connect(consumerThread, SIGNAL(captureSucceed()), consumerThread, SLOT(startPreview()));
    thread->start();



    request = sessionHolder.get()->getRequest();
    iCaptureSession =
        interface_cast<ICaptureSession>(sessionHolder.get()->getSession());
    
    // busy requesting capture
    while (sendRequest)
    {
        uint32_t frameId = iCaptureSession->capture(request);
        // CONSUMER_PRINT("requesting framid: %d\n", frameId);
        if (frameId == 0)
            ORIGINATE_ERROR("Failed to submit request");
    }

    return true;
}

void ThreadWorker::captureFrame()
{
    /* stop the current preview request */
    CONSUMER_PRINT("thread capturing..\n");
    while (consumerThread->allFramesEmpty()){
        qDebug() << "waiting for emptying frames...";
        QThread::msleep(100);
    }
    /* open the save_flag in consumerThread*/
    consumerThread->swtichCaptureMode();
    /* send a request for captureFrame */
    iCaptureSession->capture(request);
    emit captureSignal();
}
