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

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(50 / 255, 50 / 255, 200 / 255)
    }

    Viewport {
        id: viewport
        anchors.fill: parent

        viewportWidth: viewportSize.width
        viewportHeight: viewportSize.height

        camera: Camera {
            id: camera
            position: Qt.vector3d(0, 0, -400)
            rotation: Qt.vector3d(0, 0, 0)
            clipNear: 10
            clipFar: 12000.0
            fov: 90.0
        }

        lights: [
            Light {
                color: Qt.rgba(100/255, 100/255, 100/255, 1)
                visible: ambientLightCheckBox.checked
            },
            Light {
                type: Light.DirectionalLight
                color: Qt.rgba(100/255, 100/255, 100/255, 1)
                worldDirection: Qt.vector3d(-1, 1, -1)
                visible: directionalLightCheckBox.checked
            },
            Light {
                id: greenLight
                type: Light.PointLight
                color: Qt.rgba(0, 1, 0, 1)
                position: Qt.vector3d(500 * Math.cos(angle * Math.PI / 180), 500, 500 * Math.sin(angle * Math.PI / 180))
                visible: pointLightCheckBox.checked
                castsShadow: true

                property real angle: 0
                NumberAnimation on angle {
                    loops: Animation.Infinite
                    duration: 10000
                    from: 0
                    to: 360
                    running: true
                }
            },
            Light {
                id: redLight
                type: Light.PointLight
                color: Qt.rgba(1, 0, 0, 1)
                linearAttenuation: 0.002
                position: Qt.vector3d(200 * Math.cos(-2 * angle * Math.PI / 180), 500, 200 * Math.sin(-2 * angle * Math.PI / 180))
                visible: pointLightCheckBox.checked
                castsShadow: true

                property real angle: 0
                NumberAnimation on angle {
                    loops: Animation.Infinite
                    duration: 10000
                    from: 0
                    to: 360
                    running: true
                }
            }
        ]

        models: [
            Model {
                id: redCube
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/cube.cob"
                position: redLight.position
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(10, 10, 10)
                pivot: Qt.vector3d(0.00153685, 1, -0.0110648)
                attributes: Mesh.SwapYZ

                materials: [
                    Material {
                        color: Qt.rgba(1, 0, 0, 1)
                        type: Material.Flat
                    }
                ]
            },

            Model {
                id: greenCube
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/cube.cob"
                position: greenLight.position
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(10, 10, 10)
                pivot: Qt.vector3d(0.00153685, 1, -0.0110648)
                attributes: Mesh.SwapYZ

                materials: [
                    Material {
                        color: Qt.rgba(0, 1, 0, 1)
                        type: Material.Flat
                    }
                ]
            },

            Terrain {
                id: terrain
                source: "qrc:///AlmGine/AlmGine/Assets/Images/height_grass_40_40_01.bmp"
                terrainWidth: 4000
                terrainHeight: 4000
                sourceWidth: 40
                sourceHeight: 40
                position: Qt.vector3d(0, 0, 0)
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(terrainWidth, 700, terrainHeight)
                pivot: Qt.vector3d(0, 0, 0)

                materials: [
                    Material {
                        color: "white"
                        type: Material.Flat
                        texture: Texture {
                            source: "qrc:///AlmGine/AlmGine/Assets/Images/stone256_256_01.bmp"
                        }
                    }
                ]
            },

            Model {
                id: model
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/sphere.cob"
                position: Qt.vector3d(
                    150 * Math.cos(angle * Math.PI / 180),
                    200 + 75 * Math.sin(angle * Math.PI / 180),
                    150 * Math.sin(angle * Math.PI / 180)
                )
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(40, 40, 40)
                pivot: Qt.vector3d(0.00153685, 1, -0.0110648)
                attributes: Mesh.SwapYZ
                castsShadow: true

                materials: [
                    Material {
                        type: Material.Gouraud
                        texture: Texture {
                            source: "qrc:///AlmGine/AlmGine/Assets/Images/earth.bmp"
                        }
                    }
                ]

                property real angle: 0
                NumberAnimation on angle {
                    loops: Animation.Infinite
                    duration: 10000
                    from: 0
                    to: 360
                    running: true
                }
            }
        ]
    }

    KeysController {
        id: input
        controlledObject: camera
        anchors.fill: parent
        delta: terrain.delta
        gravity: -0.20
        seaLevel: 50
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
