TEMPLATE = app

TARGET = tst_sendrendercapturejob

QT += core-private 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_sendrendercapturejob.cpp

include(../../commons/commons.pri)
include(../../../core/common/common.pri)

# Link Against OpenGL Renderer Plugin
include(../opengl_render_plugin.pri)
