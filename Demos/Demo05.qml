import QtQuick
import QtQuick.Controls

import AlmGine 1.0

import "../AlmGine/Qml/Helpers"
import ".."

Rectangle {
    id: root
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

    Viewport {
        id: viewport
        anchors.fill: parent

        viewportWidth: viewportSize.width
        viewportHeight: viewportSize.height

        camera: Camera {
            id: camera
            position: Qt.vector3d(0, 0, 0)
            rotation: Qt.vector3d(0, 0, 0)
            clipNear: 20
            clipFar: 10000.0
            fov: 120.0
        }

        lights: [
            Light {
                color: Qt.rgba(100/255, 100/255, 100/255, 1)
                visible: ambientLightCheckBox.checked
            },
            Light {
                type: Light.DirectionalLight
                color: Qt.rgba(100/255, 100/255, 100/255, 1)
                worldDirection: Qt.vector3d(-1, 0, -1)
                visible: directionalLightCheckBox.checked
            },
            Light {
                type: Light.PointLight
                color: "green"
                position: Qt.vector3d(1000 * Math.cos(angle * Math.PI / 180), 100, 1000 * Math.sin(angle * Math.PI / 180))
                visible: pointLightCheckBox.checked

                property real angle: 0
                NumberAnimation on angle {
                    loops: Animation.Infinite
                    duration: 5000
                    from: 0
                    to: 360
                    running: true
                }

            },
            Light {
                type: Light.SpotLight
                color: "red"
                worldDirection: Qt.vector3d(-1, 0, -1)
                position: Qt.vector3d(1000 * Math.cos(angle * Math.PI / 180), 200, 1000 * Math.sin(angle * Math.PI / 180))
                visible: spotLightCheckBox.checked

                property real angle: 0
                NumberAnimation on angle {
                    loops: Animation.Infinite
                    duration: 5000
                    from: 0
                    to: 360
                    running: true
                }
            }
        ]

        models: [
            Model {
                id: waterConstant
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/water.cob"
                position: Qt.vector3d(-50, 0, 120)
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(10, 10, 10)
                pivot: Qt.vector3d(-0.0419818, 1, -0.00115802)
                attributes: Mesh.SwapYZ

                materials: [
                    Material {
                        color: "blue"
                        type: Material.Constant
                    },
                    Material {
                        color: "red"
                        type: Material.Constant
                    }

                ]

                Vector3dAnimation on rotation {
                    loops: Animation.Infinite
                    duration: 5000
                    from: Qt.vector3d(0, 0, 0)
                    to: Qt.vector3d(360, 360, 360)
                    running: true
                }
            },

            Model {
                id: waterFlat
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/water.cob"
                position: Qt.vector3d(0, 0, 120)
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(10, 10, 10)
                pivot: Qt.vector3d(-0.0419818, 1, -0.00115802)
                attributes: Mesh.SwapYZ

                materials: [
                    Material {
                        color: "red"
                        type: Material.Flat
                    },
                    Material {
                        color: "blue"
                        type: Material.Flat
                    }
                ]

                Vector3dAnimation on rotation {
                    loops: Animation.Infinite
                    duration: 5000
                    from: Qt.vector3d(0, 0, 0)
                    to: Qt.vector3d(360, 360, 360)
                    running: true
                }
            },

            Model {
                id: waterGouraud
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/water.cob"
                position: Qt.vector3d(50, 0, 120)
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(10, 10, 10)
                pivot: Qt.vector3d(-0.0419818, 1, -0.00115802)
                attributes: Mesh.SwapYZ

                materials: [
                    Material {
                        color: "white"
                        type: Material.Gouraud
                    },
                    Material {
                        color: "white"
                        type: Material.Gouraud
                    }
                ]

                Vector3dAnimation on rotation {
                    loops: Animation.Infinite
                    duration: 5000
                    from: Qt.vector3d(0, 0, 0)
                    to: Qt.vector3d(360, 360, 360)
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

    Column {
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        MyCheckBox {
            id: ambientLightCheckBox
            text: qsTr("Ambient Light")
            checked: true
        }
        MyCheckBox {
            id: directionalLightCheckBox
            text: qsTr("Directional Light")
            checked: true
        }
        MyCheckBox {
            id: pointLightCheckBox
            text: qsTr("Point Light")
            checked: true
        }
        MyCheckBox {
            id: spotLightCheckBox
            text: qsTr("Spot Light")
            checked: true
        }
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
