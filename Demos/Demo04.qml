import QtQuick
import QtQuick.Controls

import AlmGine 1.0

import "../AlmGine/Qml/Helpers"
import ".."

Rectangle {
    id: riit
    anchors.fill: parent
    focus: true
    color: "black"

    property var viewportSizes: [
        { width: 320, height: 240 },
        { width: 400, height: 300 },
        { width: 640, height: 480 },
        { width: 800, height: 600 },
        { width: 1024, height: 768 },
        { width: 1280, height: 960 },
    ]

    property var viewportSize: viewportSizes[viewportSizeSlider.sliderValue]

    property int numObjects: 16
    property int objectSpacing: 250
    property int cameraDistance: 1750

    Viewport {
        id: viewport
        anchors.fill: parent

        viewportWidth: viewportSize.width
        viewportHeight: viewportSize.height

        camera: Camera {
            id: camera

            property real viewAngle: 0

            position: Qt.vector3d(
                cameraDistance * Math.cos(viewAngle * Math.PI / 180),
                cameraDistance * Math.sin(viewAngle * Math.PI / 180),
                2 * cameraDistance * Math.sin(viewAngle * Math.PI / 180)
            )

            property var direction: Qt.vector3d(0, 0, 0).minus(camera.position);
            property real yaw: Math.atan2(direction.x, direction.z) * 180 / Math.PI;
            property real pitch: Math.atan2(-direction.y, Math.sqrt(direction.x*direction.x + direction.z*direction.z))  * 180 / Math.PI;

            rotation: Qt.vector3d(pitch, yaw, 0);
            clipNear: 50.0
            clipFar: 10000.0
            fov: 90.0

            NumberAnimation on viewAngle {
                loops: Animation.Infinite
                duration: 10000
                from: 0
                to: 360
                running: true
            }
        }

        lights: [
            Light {
                id: ambientLight
                color: "white"
            }
        ]

        Material {
            id: material
            color: "blue"
        }

        Component {
            id: modelComponent
            Model {
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/tank.plg"
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(1, 1, 1)
                pivot: Qt.vector3d(0, 0, 0)
                cullBackface: false

                materials: [material]

                Vector3dAnimation on rotation {
                    loops: Animation.Infinite
                    duration: 10000
                    from: Qt.vector3d(0, 0, 0)
                    to: Qt.vector3d(0, 360, 0)
                    running: true
                }
            }
        }

        Component.onCompleted: {
            for (var x =- numObjects / 2; x < numObjects / 2; ++x) {
                for (var z =- numObjects / 2; z < numObjects / 2; ++z) {
                    const x_ = x * objectSpacing + objectSpacing / 2;
                    const z_ = z * objectSpacing + objectSpacing / 2;
                    var obj = modelComponent.createObject(viewport, { position: Qt.vector3d(x_, 0, z_) });
                    models.push(obj);
                }
            }
        }
    }

    Connections {
        target: viewport.renderStats
        function onFpsChanged() {
            fps.text = "FPS: " + viewport.renderStats.fps
        }
    }

    Text {
        id: fps
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.topMargin: 5
        color: "white"
    }

    RoundButton {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.bottomMargin: 10
        text: "Назад"
        width: 100
        height: 44
        onClicked: { demoLoader.source = "" }
    }

    MySlider {
        id: viewportSizeSlider
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 0
        sliderText: viewportSize.width + "x" + viewportSize.height
    }
}
