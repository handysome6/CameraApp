extern "C"
{
    #include"JY901.h"
}
extern "C"
{
    extern void CopeSerialData(unsigned char ucRxBuffer[]);
}
#include "imuworker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <iconv.h>
#include <string>
#include <iostream>

int open_port(int com_port);
int set_uart_config(int fd, int baud_rate, char parity);

IMUWorker::IMUWorker()
{
}

void IMUWorker::stop()
{
    reading = false;
}


int IMUWorker::readXYZ()
{
    int UART_fd = open_port(1);
	if (set_uart_config(UART_fd, 115200, 'S') < 0)
	{
		perror("Failed to set UART.");
		exit(1);
	}
    
    static unsigned char data[11];
	char buff[1];
    int len = 0;

    while(reading)
	{
		int byteCount = read(UART_fd, buff, 1);
        if(byteCount > 0)
        {
            data[len++] = buff[0];
            //printf("content:%X\n", buff[0]);

            if (data[0] != 0x55)
            {
                len = 0;
                continue;
            }

            if(len < 11)
            {
                continue;
            }
            else
            {
                // printf("One group of data received.");
                CopeSerialData(data);
                len = 0;
            }
        }
        else if(byteCount == 0)
        {
            printf("Transmission OK but no serial data received.");
            continue;
        }
        else
        {
            perror("Serial transmission failed.");
        }

        float imuDataAngleX = (float)stcAngle.Angle[0]/32768*180; //Roll  //unit: degree
        float imuDataAngleY = (float)stcAngle.Angle[1]/32768*180; //Pitch
        float imuDataAngleZ = (float)stcAngle.Angle[2]/32768*180; //Yaw

        // printf("Angle X, Y, Z: %.3f, %.3f, %.3f\n", imuDataAngleX, imuDataAngleY, imuDataAngleZ);
        emit(xyzSignal(imuDataAngleX, imuDataAngleY, imuDataAngleZ));
	}
 
    return 0;
}
 
/*
* 打开串口
*/
int open_port(int com_port)
{
    int fd;
    /* 使用普通串口 */
    // TODO::在此处添加串口列表
    char const* dev[] = { "/dev/ttyTHS1", "/dev/ttyUSB0" };
 
    //O_NDELAY 同 O_NONBLOCK。
    fd = open(dev[com_port], O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror("open serial port");
        return(-1);
    }
 
    //恢复串口为阻塞状态 
    //非阻塞：fcntl(fd,F_SETFL,FNDELAY)  
    //阻塞：fcntl(fd,F_SETFL,0) 
    if (fcntl(fd, F_SETFL, 0) < 0)
    {
        perror("fcntl F_SETFL\n");
    }
    /*测试是否为终端设备*/
    if (isatty(STDIN_FILENO) == 0)
    {
        perror("standard input is not a terminal device");
    }
 
    return fd;
}
 
/*
* 串口设置
*/
int set_uart_config(int fd, int baud_rate, char parity)
{
    struct termios opt;
    int speed;
    if (tcgetattr(fd, &opt) != 0)
    {
        perror("tcgetattr");
        return -1;
    }
 
    /*设置波特率*/
    switch (baud_rate)
    {
    case 2400:  speed = B2400;  break;
    case 4800:  speed = B4800;  break;
    case 9600:  speed = B9600;  break;
    case 19200: speed = B19200; break;
    case 38400: speed = B38400; break;

    case 57600: speed = B57600; break;
    case 115200: speed = B115200; break;
    case 230400: speed = B230400; break;
    }
    cfsetispeed(&opt, speed);
    cfsetospeed(&opt, speed);
    tcsetattr(fd, TCSANOW, &opt);
 
    opt.c_cflag &= ~CSIZE;
 
    
 
    /*设置奇偶校验位*/
    switch (parity) //N
    {
    case 'n':case 'N':
    {
        opt.c_cflag &= ~PARENB;//校验位使能     
        opt.c_iflag &= ~INPCK; //奇偶校验使能  
    }break;
    case 'o':case 'O':
    {
        opt.c_cflag |= (PARODD | PARENB);//PARODD使用奇校验而不使用偶校验 
        opt.c_iflag |= INPCK;
    }break;
    case 'e':case 'E':
    {
        opt.c_cflag |= PARENB;
        opt.c_cflag &= ~PARODD;
        opt.c_iflag |= INPCK;
    }break;
    case 's':case 'S': /*as no parity*/
    {
        opt.c_cflag &= ~PARENB;
        opt.c_cflag &= ~CSTOPB;
    }break;
    default:
    {
        opt.c_cflag &= ~PARENB;//校验位使能     
        opt.c_iflag &= ~INPCK; //奇偶校验使能          	
    }break;
    }

 
    /*处理未接收字符*/
    tcflush(fd, TCIFLUSH);
 
    /*设置等待时间和最小接收字符*/
    opt.c_cc[VTIME] = 1000;
    opt.c_cc[VMIN] = 0;
 
    /*关闭串口回显*/
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | NOFLSH);
 
    /*禁止将输入中的回车翻译为新行 (除非设置了 IGNCR)*/
    opt.c_iflag &= ~ICRNL;
    /*禁止将所有接收的字符裁减为7比特*/
    opt.c_iflag &= ~ISTRIP;
 
    /*激活新配置*/
    if ((tcsetattr(fd, TCSANOW, &opt)) != 0)
    {
        perror("tcsetattr");
        return -1;
    }
 
    return 0;
}
