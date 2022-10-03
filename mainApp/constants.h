#ifndef CONSTANT_H
#define CONSTANT_H

#include <fcntl.h>

#include "Error.h"
#include "Thread.h"

#include <Argus/Argus.h>
#include <EGLStream/EGLStream.h>
#include <EGLStream/NV/ImageNativeBuffer.h>

#include <nvbuf_utils.h>
#include <NvEglRenderer.h>
#include <NvJpegEncoder.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <thread>

// #include <QString>
#include <string>

//#include <JetsonGPIO.h>

using namespace Argus;
using namespace EGLStream;
using namespace ArgusSamples;

/* Constants */
static const uint32_t            MAX_CAMERA_NUM = 2;
static const uint32_t            SENSOR_MODE = 0;
static const Argus::Size2D<uint32_t>    STREAM_SIZE(734, 554);
static const Argus::Size2D<uint32_t>    CAPTURE_SIZE(4032, 3040);




/* Globals */
static bool                DO_STAT = false;
static  uint32_t           DEFAULT_FPS = 30;


#define JPEG_BUFFER_SIZE    (CAPTURE_SIZE.area() * 3 )

/* Debug print macros */
#define PRODUCER_PRINT(...) printf("PRODUCER: " __VA_ARGS__)
#define CONSUMER_PRINT(...) printf("CONSUMER: " __VA_ARGS__)

#undef Bool
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted

#endif // CONSTANT_H
