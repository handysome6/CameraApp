#ifndef I2CWORKER
#define I2CWORKER

#include "i2c/i2c.h"

#include <QObject>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

class I2CWorker : public QObject
{
    Q_OBJECT
public:
    I2CWorker();
    void stop();

public slots:
    int readTemp();
    
signals:
    void tempSignal(float);

private:
    bool reading = true;
};


#endif // I2CWORKER