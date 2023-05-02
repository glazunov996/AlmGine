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

    property int numObjects: 100

    function randomRange(min, max) {
        return Math.random() * (max - min + 1) + min;
    }

    Image {
        anchors.fill: parent
        source: "qrc:///AlmGine/AlmGine/Assets/Images/checkerboard800.bmp"
        fillMode: Image.PreserveAspectCrop
    }

    Component {
        id: texSphereComponent
        Model {
            id: cube
            source: "qrc:///AlmGine/AlmGine/Assets/Meshes/sphere_bw.cob"
            position: Qt.vector3d(randomRange(-2000, 2000), randomRange(-1000, 1000), randomRange(-2000, 2000))
            rotation: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(50, 50, 50)
            pivot: Qt.vector3d(0, 0, 0)
            attributes: Mesh.SwapYZ

            materials: [
                Material {
                    color: Qt.rgba(1, 1, 1, 0.5)
                    type: Material.Gouraud
                    texture: Texture {
                        source: "qrc:///AlmGine/AlmGine/Assets/Images/checker64.bmp"
                    }
                }
            ]
        }
    }

    Component {
        id: redSphereComponent
        Model {
            id: cube
            source: "qrc:///AlmGine/AlmGine/Assets/Meshes/sphere.cob"
            position: Qt.vector3d(randomRange(-2000, 2000), randomRange(-1000, 1000), randomRange(-2000, 2000))
            rotation: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(50, 50, 50)
            pivot: Qt.vector3d(0, 0, 0)
            attributes: Mesh.SwapYZ

            materials: [
                Material {
                    color: Qt.rgba(1, 0, 0, 0.5)
                    type: Material.Gouraud
                }
            ]
        }
    }

    Component {
        id: greenSphereComponent
        Model {
            id: cube
            source: "qrc:///AlmGine/AlmGine/Assets/Meshes/sphere.cob"
            position: Qt.vector3d(randomRange(-2000, 2000), randomRange(-1000, 1000), randomRange(-2000, 2000))
            rotation: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(50, 50, 50)
            pivot: Qt.vector3d(0, 0, 0)
            attributes: Mesh.SwapYZ

            materials: [
                Material {
                    color: Qt.rgba(0, 1, 0, 0.5)
                    type: Material.Gouraud
                }
            ]
        }
    }

    Component {
        id: blueSphereComponent
        Model {
            id: cube
            source: "qrc:///AlmGine/AlmGine/Assets/Meshes/sphere.cob"
            position: Qt.vector3d(randomRange(-2000, 2000), randomRange(-1000, 1000), randomRange(-2000, 2000))
            rotation: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(50, 50, 50)
            pivot: Qt.vector3d(0, 0, 0)
            attributes: Mesh.SwapYZ

            materials: [
                Material {
                    color: Qt.rgba(0, 0, 1, 0.5)
                    type: Material.Gouraud
                }
            ]
        }
    }

    Component {
        id: whiteSphereComponent
        Model {
            id: cube
            source: "qrc:///AlmGine/AlmGine/Assets/Meshes/sphere.cob"
            position: Qt.vector3d(randomRange(-2000, 2000), randomRange(-1000, 1000), randomRange(-2000, 2000))
            rotation: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(50, 50, 50)
            pivot: Qt.vector3d(0, 0, 0)
            attributes: Mesh.SwapYZ

            materials: [
                Material {
                    color: Qt.rgba(1, 1, 1, 0.5)
                    type: Material.Gouraud
                }
            ]
        }
    }

    property var objectComponents: [
        texSphereComponent,
        redSphereComponent,
        greenSphereComponent,
        blueSphereComponent,
        whiteSphereComponent
    ]

    Viewport {
        id: viewport
        anchors.fill: parent

        viewportWidth: viewportSize.width
        viewportHeight: viewportSize.height

        camera: Camera {
            id: camera
            position: Qt.vector3d(0, 0, 0)
            rotation: Qt.vector3d(0, 0, 0)
            clipNear: 10
            clipFar: 12000.0
            fov: 120.0
        }

        lights: [
            Light {
                color: Qt.rgba(100/255, 100/255, 100/255, 1)
                visible: ambientLightCheckBox.checked
            },
            Light {
                type: Light.DirectionalLight
                color: Qt.rgba(150/255, 150/255, 150/255, 1)
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

        models: []

        property var velocities: []

        function moveObjects(frameTime) {
            for (let i = 0; i < numObjects; ++i) {
                const obj = models[i];
                const vel = velocities[i];
                obj.position.x += vel.x * frameTime;
                obj.position.y += vel.y * frameTime;
                obj.position.z += vel.z * frameTime;

                if (obj.position.x >= 2000 || obj.position.x <= -2000) {
                    vel.x = -vel.x;
                    obj.position.x += vel.x;
                }

                if (obj.position.y >= 1000 || obj.position.y <= -1000) {
                    vel.y = -vel.y;
                    obj.position.y += vel.y;
                }

                if (obj.position.z >= 2000 || obj.position.z <= -2000) {
                    vel.z = -vel.z;
                    obj.position.z += vel.z;
                }

                models[i] = obj;
                velocities[i] = vel;
            }
        }

        Component.onCompleted: {
            for (let i = 0; i < numObjects; ++i) {
                const n = Math.floor(randomRange(0, 4));
                models.push(objectComponents[n].createObject(viewport, {}))
                const vel = Qt.vector3d(randomRange(-6, 6), randomRange(-6, 6), randomRange(-6, 6))
                velocities.push(vel);
            }
        }
    }

    FrameAnimation {
        running: true
        onTriggered: viewport.moveObjects(frameTime * 100)
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
