import QtQuick
import QtQuick.Controls

import AlmGine 1.0

Window {
    id: window
    width: 800
    height: 600
    visible: true
    color: "black"

    Item {
        anchors.fill: parent

        Image {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: (parent.height - height) / 2
            anchors.rightMargin: (parent.width - width) / 5
            width: parent.height / 1.8
            height: parent.height / 1.5
            source: "qrc:/AlmGine/book.jpg"
        }
    }

    ListModel {
        id: demoModel
        ListElement {
            name: "Демо 1"
            url: "./Demos/Demo01.qml"
        }
        ListElement {
            name: "Демо 2"
            url: "./Demos/Demo02.qml"
        }
        ListElement {
            name: "Демо 3"
            url: "./Demos/Demo03.qml"
        }
        ListElement {
            name: "Демо 4"
            url: "./Demos/Demo04.qml"
        }
        ListElement {
            name: "Демо 5"
            url: "./Demos/Demo05.qml"
        }
        ListElement {
            name: "Демо 6"
            url: "./Demos/Demo06.qml"
        }
        ListElement {
            name: "Демо 7"
            url: "./Demos/Demo07.qml"
        }
        ListElement {
            name: "Демо 8"
            url: "./Demos/Demo08.qml"
        }
        ListElement {
            name: "Демо 9"
            url: "./Demos/Demo09.qml"
        }
        ListElement {
            name: "Демо 10"
            url: "./Demos/Demo10.qml"
        }
        ListElement {
            name: "Демо 11"
            url: "./Demos/Demo11.qml"
        }
        ListElement {
            name: "Демо 12"
            url: "./Demos/Demo12.qml"
        }
        ListElement {
            name: "Демо 13"
            url: "./Demos/Demo13.qml"
        }
        ListElement {
            name: "Демо 14"
            url: "./Demos/Demo14.qml"
        }
    }

    GridView {
        id: demoGrid
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.topMargin: 5
        model: demoModel
        flow: GridView.FlowTopToBottom
        cellWidth: 100
        cellHeight: 50
        delegate: Item {
            width: demoGrid.cellWidth
            height: demoGrid.cellHeight
            RoundButton {
                anchors.fill: parent
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                anchors.topMargin: 3
                anchors.bottomMargin: 3
                text: model.name
                onClicked: demoLoader.source = Qt.resolvedUrl(model.url);
            }
        }
    }

    Loader {
        id: demoLoader
        anchors.fill: parent
        focus: true
    }
}
