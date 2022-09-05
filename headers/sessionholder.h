#ifndef SESSIONHOLDER_H
#define SESSIONHOLDER_H

#include "constants.h"

/*
 * Counterclockwise rotation value, in degree
 */

/* An utility class to hold all resources of one capture session */
class SessionHolder : public Destructable
{
public:
    explicit SessionHolder();
    virtual ~SessionHolder();

    bool initialize(std::vector<CameraDevice*> &cameraDevices, ICameraProvider* iCameraProvider, NvEglRenderer* g_renderer);

    ICaptureSession* getSession() const
    {
        return iCaptureSession;
    }


    void getCaptureStreams(std::vector<OutputStream*> &captureStreams) const
    {
        captureStreams.push_back(leftCaptureStream.get());
        captureStreams.push_back(rightCaptureStream.get());
    }

    Request* getRequest() const
    {
        return m_request.get();
    }

    virtual void destroy()
    {
        delete this;
    }

private:
    UniqueObj<CaptureSession> m_captureSession;
    ICaptureSession* iCaptureSession;
    UniqueObj<OutputStream> leftCaptureStream;
    UniqueObj<OutputStream> rightCaptureStream;
    UniqueObj<Request> m_request;
};

#endif // SESSIONHOLDER_H