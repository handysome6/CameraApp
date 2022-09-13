#ifndef CAMERAHOLDER_H
#define CAMERAHOLDER_H
#include <QObject>

#include "constants.h"
#include "sessionholder.h"
#include "consumerthread.h"
#include "providerthread.h"


class CameraHolder : public QObject
{
    Q_OBJECT

public:
    CameraHolder();
    ~CameraHolder();
    void captureFrame();
    
    bool sendRequest = true;

public slots:
    bool initAll();

protected:
    bool initSession();

signals:
    void xWindowReady(int);
    void startSignal();
    void finished();
    void captureSucceed(std::string);

private:
    // add your variables here
    NvEglRenderer* g_renderer = NULL;
    UniqueObj<CameraProvider>  g_cameraProvider;
    ICameraProvider* iCameraProvider;
    CaptureSession* captureSession;
    ICaptureSession* iCaptureSession;
    std::vector<CameraDevice*> cameraDevices;
    
    UniqueObj<SessionHolder> sessionHolder;
    std::vector<OutputStream*> captureStreams;
    QThread* thread;
    QThread* thread1;
    ConsumerThread *consumerThread;
    ProviderThread *providerThread;
    Request* request;

};

#endif // CAMERAHOLDER_H
