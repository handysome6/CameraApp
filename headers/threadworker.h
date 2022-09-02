#ifndef THREADWORKER_H
#define THREADWORKER_H
#include <QObject>

#include "constants.h"
#include "sessionholder.h"
#include "consumerthread.h"


class ThreadWorker : public QObject
{
    Q_OBJECT

public:
    ThreadWorker();
    ~ThreadWorker();
    void captureFrame();
    
    bool sendRequest = true;

public slots:
    bool process();

signals:
    void finished();
    void xWindowReady(int);
    void captureSignal();

private:
    // add your variables here
    UniqueObj<CameraProvider>  g_cameraProvider;
    NvEglRenderer* g_renderer = NULL;
    uint32_t                   g_stream_num = MAX_CAMERA_NUM;
    int input_pin = 10; // BOARD Label 19, BCM 10.
    ConsumerThread *consumerThread;
    ICaptureSession* iCaptureSession;
    UniqueObj<SessionHolder> sessionHolder;
    Request* request;

};

#endif // THREADWORKER_H
