#include <QGuiApplication>

#include <QtQuick/QQuickView>
#include <QtQml/QQmlEngine>
#include <QQmlApplicationEngine>

#include "AlmGine/agviewport.h"
#include "AlmGine/agobject.h"
#include "AlmGine/agcamera.h"
#include "AlmGine/agmesh.h"
#include "AlmGine/agmodel.h"
#include "AlmGine/agtexture.h"
#include "AlmGine/agmaterial.h"
#include "AlmGine/aglight.h"
#include "Demos/terrain.h"
#include "Demos/waveterrain.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
    qputenv("QSG_RHI", "1"); // ### Qt 6 remove, this will be the default anyway
    qputenv("QML_XHR_ALLOW_FILE_READ", "1");

    qmlRegisterType<AGViewport>("AlmGine", 1, 0, "Viewport");
    qmlRegisterType<AGObject>("AlmGine", 1, 0, "Object");
    qmlRegisterType<AGCamera>("AlmGine", 1, 0, "Camera");
    qmlRegisterUncreatableType<AGMesh>("AlmGine", 1, 0, "Mesh", "Internal");
    qmlRegisterType<AGModel>("AlmGine", 1, 0, "Model");
    qmlRegisterType<AGTexture>("AlmGine", 1, 0, "Texture");
    qmlRegisterType<AGMaterial>("AlmGine", 1, 0, "Material");
    qmlRegisterType<AGLight>("AlmGine", 1, 0, "Light");
    qmlRegisterType<Terrain>("AlmGine", 1, 0, "Terrain");
    qmlRegisterType<WaveTerrain>("AlmGine", 1, 0, "WaveTerrain");

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/AlmGine/main.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
