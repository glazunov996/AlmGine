import QtQuick
import QtQuick.Controls

import AlmGine 1.0

import "../AlmGine/Qml/Helpers"
import ".."

Rectangle {
    id: root
    anchors.fill: parent
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

    Viewport {
        id: viewport
        anchors.fill: parent

        viewportWidth: viewportSize.width
        viewportHeight: viewportSize.height

        camera: Camera {
            id: camera
            position: Qt.vector3d(0, 0, -100)
            rotation: Qt.vector3d(0, 0, 0)
            clipNear: 50.0
            clipFar: 500.0
            fov: 90.0
        }

        lights: [
            Light {
                id: ambientLight
                color: "white"
            }
        ]

        models: [
            Model {
                id: model
                source: "#triangle"
                position: Qt.vector3d(0, 0, 100)
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(50, 50, 50)
                pivot: Qt.vector3d(0, 0, 0)
                cullBackface: false

                materials: [
                    Material {
                        color: "blue"
                    }
                ]

                Vector3dAnimation on rotation {
                    loops: Animation.Infinite
                    duration: 5000
                    from: Qt.vector3d(0, 0, 0)
                    to: Qt.vector3d(0, 360, 0)
                    running: true
                }
            }
        ]
    }

    KeysController {
        id: input
        controlledObject: camera
        anchors.fill: parent
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
