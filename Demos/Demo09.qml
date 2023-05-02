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

    Image {
        id: sky
        x: -(camera.rotation.y * 1.2) % parent.width
        y: 0
        width: parent.width
        height: parent.height
        source: "qrc:///AlmGine/AlmGine/Assets/Images/cloud03.bmp"
    }

    Image {
        x: sky.x + parent.width
        y: 0
        width: parent.width
        height: parent.height
        source: "qrc:///AlmGine/AlmGine/Assets/Images/cloud03.bmp"
    }

    Image {
        x: sky.x - parent.width
        y: 0
        width: parent.width
        height: parent.height
        source: "qrc:///AlmGine/AlmGine/Assets/Images/cloud03.bmp"
    }

    Item {
        id: game
        state: "restart"
        states: [
            State {
                name: "restart"
                PropertyChanges { target: camera; position: Qt.vector3d(1550, 800, 1950); rotation: Qt.vector3d(0, -140, 0) }
                PropertyChanges { target: introEffect; visible: true }
                PropertyChanges { target: delayTimer; interval: 3000; repeat: false; running: true }
            },
            State {
                name: "intro1"
                PropertyChanges { target: camera; position: Qt.vector3d(444, 200, -534); rotation: Qt.vector3d(10, 45, 0) }
                PropertyChanges { target: introEffect; visible: false }
                PropertyChanges { target: delayTimer; interval: 3000; repeat: false; running: true }
            },
            State {
                name: "intro2"
                PropertyChanges { target: camera; position: camera.position; rotation: camera.rotation }
                PropertyChanges { target: intro2Effect; visible: true }
                PropertyChanges { target: delayTimer; interval: 3000; repeat: false; running: true }
            },
            State {
                name: "play";
                PropertyChanges { target: camera; position: camera.position; rotation: camera.rotation }
                PropertyChanges { target: intro2Effect; visible: false }
                PropertyChanges { target: delayTimer; running: false }
            }
        ]
    }

    Timer {
        id: delayTimer
        onTriggered: {
            switch (game.state) {
            case "restart":
                game.state = "intro1";
                break;
            case "intro1":
                game.state = "intro2";
                break;
            case "intro2":
                game.state = "play";
                break;
            case "play":
                break;
            }
        }
    }

    Viewport {
        id: viewport
        anchors.fill: parent

        viewportWidth: viewportSize.width
        viewportHeight: viewportSize.height

        camera: Camera {
            id: camera
            position: Qt.vector3d(0, 500, -200)
            rotation: Qt.vector3d(0, 0, 0)
            clipNear: 10
            clipFar: 12000.0
            fov: 90.0
        }

        lights: [
            Light {
                color: Qt.rgba(100/256, 100/256, 100/256, 1)
                visible: ambientLightCheckBox.checked
            },
            Light {
                type: Light.DirectionalLight
                color: Qt.rgba(200/256, 200/256, 200/256, 1)
                worldDirection: Qt.vector3d(-1, 1, 0)
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
            WaveTerrain {
                id: terrain
                source: "qrc:///AlmGine/AlmGine/Assets/Images/water_track_height_04.bmp"
                terrainWidth: 5000
                terrainHeight: 5000
                sourceWidth: 40
                sourceHeight: 40
                waveRate: 0.03
                position: Qt.vector3d(0, 0, 0)
                rotation: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(terrainWidth, 700, terrainHeight)
                pivot: Qt.vector3d(0, 0, 0)

                materials: [
                    Material {
                        color: "white"
                        type: Material.Flat
                        texture: Texture {
                            source: "qrc:///AlmGine/AlmGine/Assets/Images/water_track_color_03.bmp"
                        }
                    }
                ]
            },

            Model {
                id: model
                visible: game.state !== "restart"
                source: "qrc:///AlmGine/AlmGine/Assets/Meshes/jetski.cob"
                position: Qt.vector3d(
                    camera.position.x + (130 + input.speed * 5) * Math.sin(camera.rotation.y * Math.PI / 180),
                    camera.position.y - 25 + 7.5 * Math.sin(terrain.waveCount),
                    camera.position.z + (130 + input.speed * 5) * Math.cos(camera.rotation.y * Math.PI / 180),
                )
                rotation: Qt.vector3d(-camera.rotation.x * 1.5, camera.rotation.y + turnAng, jetskiYaw)
                scale: Qt.vector3d(17.5, 17.5, 17.5)
                pivot: Qt.vector3d(0, 0, 0)
                attributes: Mesh.SwapYZ | Mesh.InvertV
                materials: [
                    Material {
                        texture: Texture {
                            source: "qrc:///AlmGine/AlmGine/Assets/Images/jetski.bmp"
                        }
                    }
                ]
                property real turnAng: 0
                property real jetskiYaw: 0
            }
        ]
    }

    Image {
        id: introImage
        source: "qrc:///AlmGine/AlmGine/Assets/Images/zbufferwr_intro01.bmp"
        visible: false
    }

    ShaderEffect {
        id: introEffect
        x: parent.width / 2 - 560 / 2
        y: 40
        width: 560
        height: 160
        property variant src: introImage
        vertexShader: "qrc:///AlmGine/Demos/black.vert.qsb"
        fragmentShader: "qrc:///AlmGine/Demos/black.frag.qsb"
        visible: false
    }

    Image {
        id: intro2Image
        source: "qrc:///AlmGine/AlmGine/Assets/Images/zbufferwr_ready01.bmp"
        visible: false
    }

    ShaderEffect {
        id: intro2Effect
        x: parent.width / 2 - 596 / 2
        y: 40
        width: 596
        height: 244
        property variant src: intro2Image
        vertexShader: "qrc:///AlmGine/Demos/black.vert.qsb"
        fragmentShader: "qrc:///AlmGine/Demos/black.frag.qsb"
        visible: false
    }

    Item {
        states: [
            State {
                name: "rotateRight"; when: input.rorateRight
                PropertyChanges { target: model; turnAng: 25.0 }
                PropertyChanges { target: model; jetskiYaw: 10.0 }
            },
            State {
                name: "rotateLeft"; when: input.rotateLeft
                PropertyChanges { target: model; turnAng: -25.0 }
                PropertyChanges { target: model; jetskiYaw: -10.0 }
            }
        ]

        transitions: Transition {
            PropertyAnimation { target: model; property: "turnAng"; duration: 1000 }
            PropertyAnimation { target: model; property: "jetskiYaw"; duration: 500 }
        }
    }

    KeysController {
        id: input
        controlledObject: camera
        anchors.fill: parent
        delta: terrain.delta
        gravity: -0.1
        seaLevel: 30
        keysEnabled: game.state === "play"
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
            checked: false
        }
        MyCheckBox {
            id: spotLightCheckBox
            text: qsTr("Spot Light")
            checked: false
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
