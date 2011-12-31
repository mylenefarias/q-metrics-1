#-------------------------------------------------
#
# Project created by QtCreator 2011-08-07T16:17:49
#
#-------------------------------------------------

TEMPLATE = app
CONFIG  -= qt
TARGET   = qmetrics

DEPENDPATH += . inc src
INCLUDEPATH += . inc

HEADERS += inc/mat.hpp  \
           inc/img.hpp  \
           inc/metric.h \
           inc/debug.h  \
           inc/global.h \
           inc/loader.h \
           inc/csf.h
#          inc/old/old_aux.hpp \
#          inc/old/old_img.hpp \
#          inc/old/old_metric.h \
#          inc/old/old_debug.h

SOURCES += src/main.cpp   \
           src/mat.cpp    \
           src/img.cpp    \
           src/metric.cpp \
           src/debug.cpp  \
           src/loader.cpp \
           src/csf.cpp
#          src/old/old_img.cpp \
#          src/old/old_metric.cpp \
#          src/old/old_debug.cpp

# Configuracoes para linkar com o OpenCV 2.1 [Ubuntu 10.10]
#INCLUDEPATH += /usr/include/opencv
#LIBS        += /usr/lib/libml.a       \
#               /usr/lib/libcvaux.a    \
#               /usr/lib/libhighgui.a  \
#               /usr/lib/libcv.a       \
#               /usr/lib/libcxcore.a

# Configuracoes para linkar com o OpenCV 2.2

CONFIG     += link_pkgconfig
PKGCONFIG  += opencv

#INCLUDEPATH +=  /usr/local/include          \
#                /usr/local/include/opencv

#LIBS        +=  /usr/local/lib/libopencv_core.so       \
#                /usr/local/lib/libopencv_imgproc.so    \
#                /usr/local/lib/libopencv_ml.so         \
#                /usr/local/lib/libopencv_video.so      \
#                /usr/local/lib/libopencv_flann.so      \
#                /usr/local/lib/libopencv_features2d.so \
#                /usr/local/lib/libopencv_calib3d.so    \
#                /usr/local/lib/libopencv_objdetect.so  \
#                /usr/local/lib/libopencv_contrib.so    \
#                /usr/local/lib/libopencv_legacy.so     \
#                /usr/local/lib/libopencv_highgui.so

LIBS += /usr/local/lib/libfftw3.a       \
        /usr/lib/libm.a
        #/usr/lib/i386-linux-gnu/libm.a
