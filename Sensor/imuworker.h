#ifndef IMUWORKER
#define IMUWORKER

#include "imu/JY901.h"

#include <QObject>

class IMUWorker : public QObject
{
    Q_OBJECT
public:
    IMUWorker();
    void stop();

public slots:
    int readXYZ();
    
signals:
    void xyzSignal(float, float, float);

private:
    bool reading = true;
};


#endif // IMUWORKER