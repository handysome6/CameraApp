#include "i2cworker.h"

I2CWorker::I2CWorker()
{
}

float getTemperature(unsigned char buffer[])
{
    int16_t i16 = (buffer[0] << 8) | buffer[1];
    float temp = i16 / 256.0;
    return temp;
}


int I2CWorker::readTemp()
{
    int fd;
    I2CDevice device;
    unsigned char buffer[2];
    ssize_t size = sizeof(buffer);
    memset(buffer, 0, sizeof(buffer));

    /* First open i2c bus */
    if ((fd = i2c_open("/dev/i2c-0")) == -1)
    {

        perror("Open i2c bus error\n");
        return -1;
    }

    /* Fill i2c device struct */
    device.bus = fd;
    device.addr = 0x48;
    device.tenbit = 0;
    device.delay = 10;
    device.flags = 0;
    device.page_bytes = 8;
    device.iaddr_bytes = 0; /* Set this to zero, and using i2c_ioctl_xxxx API will ignore chip internal address */

    while(reading)
    {
        /* From i2c 0x00 address read 256 bytes data to buffer */
        if ((i2c_ioctl_read(&device, 0x00, buffer, size)) != size)
        {
            perror("Read register failed.\n");
        }
        else
        {
            float temp = getTemperature(buffer);
            // printf("Temperature: %f\n", temp);
            emit(tempSignal(temp));
        }
        sleep(1);
    }

    i2c_close(fd);
    
    return 0;
}

void I2CWorker::stop()
{
    reading = false;
}