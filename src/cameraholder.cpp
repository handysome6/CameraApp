#include "cameraholder.h"
#include <QtConcurrent>
#include <string>

CameraHolder::CameraHolder()
{
}

// --- DECONSTRUCTOR ---
CameraHolder::~CameraHolder()
{
}

// --- PROCESS ---
bool CameraHolder::initAll()
{
    qDebug("Starting camera...");

    
    {   /* init xwindow, emit xid to GUI */
        g_renderer = NvEglRenderer::createEglRenderer("renderer0", STREAM_SIZE.width(),
            STREAM_SIZE.height(), 0, 0);
        if (!g_renderer)
            ORIGINATE_ERROR("Failed to create EGLRenderer.");
        emit xWindowReady(g_renderer->getXWindowId());
    }  

    {   /* init CameraProvider, get existing devices */
        g_cameraProvider = UniqueObj<CameraProvider>(CameraProvider::create());
        iCameraProvider = interface_cast<ICameraProvider>(g_cameraProvider);
        if (!iCameraProvider)
            ORIGINATE_ERROR("Failed to get ICameraProvider interface");
        printf("Argus Version: %s\n", iCameraProvider->getVersion().c_str());

        iCameraProvider->getCameraDevices(&cameraDevices);
        if (cameraDevices.size() < 2)
            ORIGINATE_ERROR("Not enough cameras available");
    }

    {   /* get capture session, create output stream, create capture request*/
        /* Initialize sessionHolder  */
        sessionHolder.reset(new SessionHolder);
        if (!sessionHolder.get()->initialize(cameraDevices, iCameraProvider, g_renderer))
            ORIGINATE_ERROR("Failed to initialize Camera session ");
        iCaptureSession = sessionHolder.get()->getSession();
        request = sessionHolder.get()->getRequest();
    }

    {   /* start consumer thread */
        sessionHolder.get()->getCaptureStreams(captureStreams);

        consumerThread = new ConsumerThread(captureStreams, g_renderer);
        thread = new QThread;
        consumerThread->moveToThread(thread);
        connect(thread, SIGNAL(started()), consumerThread, SLOT(startPreview()));
        connect(this, SIGNAL(startSignal()), consumerThread, SLOT(startPreview()));
        connect(consumerThread, SIGNAL(captureSucceed(std::string)), this, SIGNAL(captureSucceed(std::string)));
        thread->start();
    }

    {   /* start provider thread */
        providerThread = new ProviderThread(iCaptureSession, request);
        thread1 = new QThread;
        providerThread->moveToThread(thread1);
        connect(thread1, SIGNAL(started()), providerThread, SLOT(sendRequest()));
        connect(this, SIGNAL(startSignal()), providerThread, SLOT(sendRequest()));
        PRODUCER_PRINT("create providerThread ok\n");
        thread1->start();
    }

}


void CameraHolder::captureFrame()
{
    // stop preview feed
    CONSUMER_PRINT("Thread is runing: %d\n", thread->isRunning());
    providerThread->stop();
    while ( !consumerThread->allFramesEmpty() )
    {
        CONSUMER_PRINT("wait emptying frameConsumer\n");
        QThread::msleep(100);
    }
    consumerThread->swtichCaptureMode();
    // thread->wait();
    QThread::msleep(100);

    // send capture request
    // CONSUMER_PRINT("sending capture request...\n");
    // iCaptureSession->capture(request);
    consumerThread->startCapture();
    CONSUMER_PRINT("capture successful\n");

    /* start preview again */
    emit startSignal();

}