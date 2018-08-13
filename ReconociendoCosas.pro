#-------------------------------------------------
#
# Project created by QtCreator 2018-03-16T11:24:10
#
#-------------------------------------------------

QT       += core gui texttospeech websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

###############################################################################################
unix:DIR_OPENCV_LIBS = /usr/local/lib  ####################################################
###############################################################################################

unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_core.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_highgui.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_video.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_imgproc.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_objdetect.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_videoio.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_dnn.so

#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_core.so.3.4.1
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_highgui.so.3.4.1
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_video.so.3.4.1
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_imgproc.so.3.4.1
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_objdetect.so.3.4.1
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_videoio.so.3.4.1
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_dnn.so.3.4.1


SOURCES += \
        main.cpp \
    mainwindow.cpp \
    scene.cpp

HEADERS += \
    mainwindow.h \
    scene.h

FORMS += \
    mainwindow.ui
