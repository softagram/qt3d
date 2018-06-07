TARGET   = Qt3DRender
MODULE   = 3drender

QT = core-private 3dcore-private
QT_FOR_PRIVATE = concurrent

include (backend/render-backend.pri)
include (geometry/geometry.pri)
include (framegraph/framegraph.pri)
include (frontend/render-frontend.pri)
include (jobs/jobs.pri)
include (lights/lights.pri)
include (materialsystem/materialsystem.pri)
include (renderstates/renderstates.pri)
include (io/io.pri)
include (picking/picking.pri)
include (raycasting/raycasting.pri)
include (services/services.pri)
include (texture/texture.pri)

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

gcov {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

HEADERS += \
    renderlogging_p.h \
    qt3drender_global.h \
    qt3drender_global_p.h \
    aligned_malloc_p.h \
    qrendererplugin_p.h \
    qrendererpluginfactory_p.h

SOURCES += \
    renderlogging.cpp \
    qrendererplugin.cpp \
    qrendererpluginfactory.cpp

MODULE_PLUGIN_TYPES = \
    sceneparsers \
    geometryloaders \
    renderplugins \
    renderers

load(qt_module)
