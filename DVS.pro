# DV6000 Qt project

QT += core gui widgets network concurrent charts

CONFIG += c++11
CONFIG += thread
TEMPLATE = app
TARGET = DVS

# RK3562 Cortex-A53：预处理循环使用 OpenMP，多核和 NEON 由编译器优化。
 unix:QMAKE_CXXFLAGS_RELEASE += -O3 -mcpu=cortex-a53 -fopenmp
 unix:QMAKE_LFLAGS_RELEASE += -fopenmp

INCLUDEPATH += \
    $$PWD/include \
    $$PWD/src \
    $$PWD/include/vkFFT

DEFINES += QT_DEPRECATED_WARNINGS

# VkFFT 使用 OpenCL 后端
DEFINES += VKFFT_BACKEND=3

# 指定 OpenCL 版本，避免 CL_TARGET_OPENCL_VERSION 警告
DEFINES += CL_TARGET_OPENCL_VERSION=120

SOURCES += \
    $$PWD/src/GlobalVars.cpp \
    $$PWD/src/chartmanager.cpp \
    $$PWD/src/configtestparameterswidget.cpp \
    $$PWD/src/controller.cpp \
    $$PWD/src/dataacquisition.cpp \
    $$PWD/src/fpgacommunicator.cpp \
    $$PWD/src/main.cpp \
    $$PWD/src/mainwindow.cpp \
    $$PWD/src/message.cpp \
    $$PWD/src/networkconfigmanager.cpp \
    $$PWD/src/queryparameterswidget.cpp \
    $$PWD/src/rmsprocessor.cpp \
    $$PWD/src/rmstransmitter.cpp \
    $$PWD/src/servercommunicator.cpp \
    $$PWD/src/spectrumprocessor.cpp \
    $$PWD/src/spectrumtransmitter.cpp \
    $$PWD/src/systemgpiocontroller.cpp \
    $$PWD/src/testwidget.cpp

HEADERS += \
    $$PWD/include/GlobalVars.h \
    $$PWD/include/chartmanager.h \
    $$PWD/include/configtestparameterswidget.h \
    $$PWD/include/controller.h \
    $$PWD/include/dataacquisition.h \
    $$PWD/include/fpgacommunicator.h \
    $$PWD/include/mainwindow.h \
    $$PWD/include/message.h \
    $$PWD/include/networkconfigmanager.h \
    $$PWD/include/queryparameterswidget.h \
    $$PWD/include/rmsprocessor.h \
    $$PWD/include/rmstransmitter.h \
    $$PWD/include/servercommunicator.h \
    $$PWD/include/spectrumprocessor.h \
    $$PWD/include/spectrumtransmitter.h \
    $$PWD/include/systemgpiocontroller.h \
    $$PWD/include/testwidget.h

OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc
UI_DIR = $$PWD/build/ui
DESTDIR = $$PWD/build/bin

unix:LIBS += -latomic

# OpenCL 库，如果系统里有 libOpenCL.so，用这个
unix:LIBS += -lOpenCL

unix:!android {
    target.path = /opt/DVS/bin
    INSTALLS += target
}