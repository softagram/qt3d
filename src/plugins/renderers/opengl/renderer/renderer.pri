INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/commandthread.cpp \
    $$PWD/glcommands.cpp \
    $$PWD/openglvertexarrayobject.cpp \
    $$PWD/rendercommand.cpp \
    $$PWD/renderer.cpp \
    $$PWD/renderqueue.cpp \
    $$PWD/renderview.cpp \
    $$PWD/renderviewbuilder.cpp \
    $$PWD/shaderparameterpack.cpp \
    $$PWD/glresourcemanagers.cpp \
    $$PWD/glshader.cpp \
    $$PWD/logging.cpp

HEADERS += \
    $$PWD/commandthread_p.h \
    $$PWD/glcommands_p.h \
    $$PWD/openglvertexarrayobject_p.h \
    $$PWD/renderercache_p.h \
    $$PWD/rendercommand_p.h \
    $$PWD/renderer_p.h \
    $$PWD/renderqueue_p.h \
    $$PWD/renderview_p.h \
    $$PWD/renderviewbuilder_p.h \
    $$PWD/shaderparameterpack_p.h \
    $$PWD/shadervariables_p.h \
    $$PWD/glshader_p.h \
    $$PWD/logging_p.h

include($$QT3D_BUILD_ROOT/src/core/qt3dcore-config.pri)
QT_FOR_CONFIG += 3dcore-private
qtConfig(qt3d-profile-jobs): {
  HEADERS += $$PWD/commandexecuter_p.h
  SOURCES += $$PWD/commandexecuter.cpp
}
