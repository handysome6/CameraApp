######################################################################
# Automatically generated by qmake (3.1) Fri Aug 26 09:22:48 2022
######################################################################

TEMPLATE = app
TARGET = CameraApp
INCLUDEPATH += . \
    $$PWD/headers \
    $$PWD/third_party/include \
    $$PWD/third_party/utils \
    $$PWD/third_party/include/libjpeg-8b \
    /usr/include/libdrm 

DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += warn_off
CONFIG += c++11
QT += concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += \
	-lNvJpegEncoder -lNvEglRenderer -lNvLogging \
	-lNvElementProfiler -lNvElement -lThread \
	-lpthread -lv4l2 -lEGL -lGLESv2 -lX11 \
	-lnvbuf_utils -lnvjpeg -lnvosd -ldrm \
	-lnveglstream_camconsumer -lnvargus_socketclient \
	-L"/usr/lib/aarch64-linux-gnu" \
	-L"/usr/lib/aarch64-linux-gnu/tegra" \
	-L"$$PWD/third_party/libs" 


# Input
HEADERS += headers/mainwidget.h \
		   headers/sessionholder.h \
		   headers/cameraholder.h \
		   headers/providerthread.h \
		   headers/consumerthread.h \
		   headers/constants.h
SOURCES += main.cpp \
    src/mainwidget.cpp \
	src/sessionholder.cpp \
	src/cameraholder.cpp \
	src/providerthread.cpp \
	src/consumerthread.cpp 
FORMS += src/mainwidget.ui
RESOURCES += resources/resources.qrc


