#-------------------------------------------------
#
# Project created by QtCreator 2017-08-23T20:32:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UguisViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        UguisViewer.cpp \
    QClickableLabel.cpp

HEADERS += \
        UguisViewer.h \
    QClickableLabel.h


DEPENDPATH  += "C:/opencv/opencv320/build_opencv-3.2.0_x86/install/include"
INCLUDEPATH += "C:/opencv/opencv320/build_opencv-3.2.0_x86/install/include"
LIBS += -L"C:/opencv/opencv320/build_opencv-3.2.0_x86/install/x86/vc12/lib"
LIBS += -lopencv_core320 -lopencv_highgui320 -lopencv_imgproc320 -lopencv_imgcodecs320


CONFIG(release,debug|release)
{
    LIBS += -lopencv_core320 -lopencv_highgui320 -lopencv_imgproc320 -lopencv_imgcodecs320
}
CONFIG(debug,debug|release)
{
    LIBS += -lopencv_core320d -lopencv_highgui320d -lopencv_imgproc320d -lopencv_imgcodecs320d
}

