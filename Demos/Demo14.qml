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

    property int skyOffset: 0

    Image {
        id: sky
        x: -(skyOffset + camera.rotation.y * 1.2) % parent.width
        y: 0
        width: parent.width
        height: parent.height
        source: "qrc:///AlmGine/AlmGine/Assets/Images/sunset800_600_03.bmp"
    }

    Image {
        x: sky.x + parent.width
        y: 0
        width: parent.width
        height: parent.height
        source: "qrc:///AlmGine/AlmGine/Assets/Images/sunset800_600_03.bmp"
    }

    Image {
        x: sky.x - parent.width
        y: 0
        width: parent.width
        height: parent.height
        source: "qrc:///AlmGine/AlmGine/Assets/Images/sunset800_600_03.bmp"
    }

    Timer {
        id: updateTimer
        running: true
        repeat: true
        interval: 1000/30
        onTriggered: skyOffset += 1
    }

    Viewport {
        id: viewport
        anchors.fill: parent

        viewportWidth: viewportSize.width
        viewportHeight: viewportSize.height

        camera: Camera {
            id: camera
            position: Qt.vector3d(0, 500, -400)
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
                id: orangeLight
                type: Light.PointLight
                color: Qt.rgba(1, 128/255, 0, 1)
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
                id: yellowLight
                type: Light.PointLight
                color: Qt.rgba(1, 1, 0, 1)
                linearAttenuation: 0.002
                position: Qt.vector3d(200 * Math.cos(-2 * angle * Math.PI / 180), 500, 200 * Math.sin(-2 * angle * Math.PI / 180))
                visible: pointLightCheckBox.checked
                castsShadow: false

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
                position: orangeLight.position
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(20, 20, 20)
                pivot: Qt.vector3d(0.00153685, 1, -0.0110648)
                attributes: Mesh.SwapYZ

                materials: [
                    Material {
                        color: "red"
                        type: Material.Flat
                    }
                ]
            },

            Model {
                id: yellowCube
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/cube.cob"
                position: yellowLight.position
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(20, 20, 20)
                pivot: Qt.vector3d(0.00153685, 1, -0.0110648)
                attributes: Mesh.SwapYZ

                materials: [
                    Material {
                        color: "yellow"
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
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/q2mdl-tekkblade/tris.md2"
                position: Qt.vector3d(0, 100, 0)
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(4, 4, 4)
                pivot: Qt.vector3d(0, 0, 0)
                attributes: Mesh.SwapYZ
                castsShadow: true

                materials: [
                    Material {
                        texture: Texture {
                            source: "qrc:///AlmGine/AlmGine/Assets/Meshes/q2mdl-tekkblade/blade_black.bmp"
                        }
                    }
                ]

                NumberAnimation on frameValue {
                    loops: Animation.Infinite
                    duration: 5000
                    from: 0.0
                    to: 39.0
                    running: true
                }
            },

            Model {
                id: model2
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/q2mdl-tekkblade/tris.md2"
                position: Qt.vector3d(0, 100, 200)
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(4, 4, 4)
                pivot: Qt.vector3d(0, 0, 0)
                attributes: Mesh.SwapYZ
                castsShadow: true

                materials: [
                    Material {
                        texture: Texture {
                            source: "qrc:///AlmGine/AlmGine/Assets/Meshes/q2mdl-tekkblade/blade_black.bmp"
                        }
                    }
                ]

                NumberAnimation on frameValue {
                    loops: Animation.Infinite
                    duration: 5000
                    from: 0.0
                    to: 39.0
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
