#ifndef CONSUMERTHREAD_H
#define CONSUMERTHREAD_H

#include "constants.h"
#include <QObject>
#include <string>


class ConsumerThread : public QObject 
{
    Q_OBJECT
public:
    explicit ConsumerThread(std::vector<OutputStream*>& streams, NvEglRenderer* renderer);
    virtual ~ConsumerThread();
    void swtichCaptureMode();
    bool allFramesEmpty();

signals:
    void captureSucceed(QString);

public slots:
    bool startPreview();
    void startCapture();

protected:
    bool threadInitialize();
    bool sbsBufferInitialize();
    bool consumerInitialize();
    bool fetchFrame(int deviceID, bool showInfo, bool modePreview);
    void capture();

    std::vector<OutputStream*>& m_streams;
    UniqueObj<FrameConsumer> m_consumers[MAX_CAMERA_NUM];
    int m_dmabufs[MAX_CAMERA_NUM];
    int m_compositedFrame;
    NvBufferCompositeParams m_compositeParam;

    int m_sbsFrame;
    NvBufferCompositeParams m_sbsParam;

    NvJPEGEncoder* m_JpegEncoder;
    unsigned char* m_OutputBuffer;
    NvEglRenderer* m_renderer;

    IEGLOutputStream* iEglOutputStreams[MAX_CAMERA_NUM];
    IFrameConsumer* iFrameConsumers[MAX_CAMERA_NUM];

    bool preview_mode = true;
    bool leftFrameEmpty = true;
    bool rightFrameEmpty = true;

};


#endif // CONSUMERTHREAD_H