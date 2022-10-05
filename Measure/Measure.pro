TARGET = Measure
TEMPLATE = lib
CONFIG += staticlib

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


SOURCES += \
    crosswidget.cpp \
    mygraphicsview.cpp \
    stereorectify.cpp \
    automatcher.cpp \
    guimeasure.cpp

HEADERS += \
    crosswidget.h \
    mygraphicsview.h \
    stereorectify.h \
    automatcher.h \
    guimeasure.h

FORMS += \
    guimeasure.ui



INCLUDEPATH += . \
        /usr/local/include \
        /usr/local/include/opencv4

LIBS += \
        -lopencv_stitching -lopencv_aruco -lopencv_bgsegm \
        -lopencv_bioinspired -lopencv_ccalib -lopencv_dnn_objdetect \
        -lopencv_dnn_superres -lopencv_dpm -lopencv_highgui \
        -lopencv_face -lopencv_fuzzy -lopencv_hfs -lopencv_img_hash \
        -lopencv_line_descriptor -lopencv_quality -lopencv_reg \
        -lopencv_rgbd -lopencv_saliency -lopencv_shape \
        -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping \
        -lopencv_superres -lopencv_optflow -lopencv_surface_matching \
        -lopencv_tracking -lopencv_datasets -lopencv_text \
        -lopencv_dnn -lopencv_plot -lopencv_ml -lopencv_videostab \
        -lopencv_videoio -lopencv_ximgproc \
        -lopencv_video -lopencv_xobjdetect -lopencv_objdetect \
        -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d \
        -lopencv_flann -lopencv_xphoto -lopencv_photo \
        -lopencv_imgproc -lopencv_core \
        -L /usr/local/lib
