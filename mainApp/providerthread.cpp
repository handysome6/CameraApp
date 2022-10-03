#include "providerthread.h"
#include <QThread>

ProviderThread::ProviderThread(ICaptureSession* iCaptureSession, Request* request):
    iCaptureSession(iCaptureSession),
    request(request)
{
    if (iCaptureSession && request)
        CONSUMER_PRINT("init provider successful\n");
}

bool ProviderThread::sendRequest()
{
    sendingRequest = true;
    if (iCaptureSession && request)
        CONSUMER_PRINT("init provider successful\n");
    else
        CONSUMER_PRINT("init provider failed\n");

    while(sendingRequest)
    {
        uint32_t frameId = iCaptureSession->capture(request);
        // CONSUMER_PRINT("requesting framid: %d\n", frameId);
        if (frameId == 0)
            ORIGINATE_ERROR("Failed to submit request");
        // CONSUMER_PRINT("init provider failed\n");
        // QThread::msleep(100);
    }
    return true;
}


void ProviderThread::stop()
{
    sendingRequest = false;
}