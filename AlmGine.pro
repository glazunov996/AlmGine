QT += qml quick

CONFIG += qmltypes
QML_IMPORT_NAME = AlmGine
QML_IMPORT_MAJOR_VERSION = 1

HEADERS += \
    AlmGine/agcamera.h \
    AlmGine/aghelper.h \
    AlmGine/aglight.h \
    AlmGine/agmaterial.h \
    AlmGine/agmesh.h \
    AlmGine/agmodel.h \
    AlmGine/agobject.h \
    AlmGine/agrenderer.h \
    AlmGine/agrenderstats.h \
    AlmGine/agtexture.h \
    AlmGine/agtriangle.h \
    AlmGine/agutils.h \
    AlmGine/agviewport.h \
    Demos/terrain.h \
    Demos/waveterrain.h
SOURCES += main.cpp \
    AlmGine/agcamera.cpp \
    AlmGine/aghelper.cpp \
    AlmGine/aglight.cpp \
    AlmGine/agmaterial.cpp \
    AlmGine/agmesh.cpp \
    AlmGine/agmodel.cpp \
    AlmGine/agobject.cpp \
    AlmGine/agrenderer.cpp \
    AlmGine/agrenderstats.cpp \
    AlmGine/agtexture.cpp \
    AlmGine/agtriangle.cpp \
    AlmGine/agutils.cpp \
    AlmGine/agviewport.cpp \
    Demos/terrain.cpp \
    Demos/waveterrain.cpp

RESOURCES += \
    AlmGine.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
