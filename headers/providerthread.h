#ifndef PROVIDERTHREAD_H
#define PROVIDERTHREAD_H


#include "constants.h"
#include <QObject>
class ProviderThread : public QObject
{
    Q_OBJECT
public:
    ProviderThread(ICaptureSession* iCaptureSession, Request* request);
    // ~ProviderThread();
    
    void stop();
    

public slots:
    bool sendRequest();

private:
    CaptureSession* captureSession;
    ICaptureSession* iCaptureSession;
    Request* request;
    bool sendingRequest = true;

};
#endif //PROVIDERTHREAD_H