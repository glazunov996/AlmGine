import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property alias text: propText.text
    property alias checked: checkBox.checked

    width: rowLayout.width
    height: rowLayout.height

    RowLayout {
        id: rowLayout
        CheckBox {
            id: checkBox
            indicator: Rectangle {
                anchors.centerIn: parent
                implicitWidth: 16
                implicitHeight: 16
                radius: 1
                color: checkBox.pressed ? "#222840" : "#6b7080"
                border.color: "#848895"
                Rectangle {
                    anchors.centerIn: parent
                    implicitWidth: 12
                    implicitHeight: 12
                    radius: 3
                    color: "#ffffff"
                    border.color: "#848895"
                    visible: checkBox.checked
                }
            }
        }
        Label {
            id: propText
            color: "#ffffff"
            font.pointSize: 10
            Layout.minimumWidth: 150
            Layout.maximumWidth: 150
            opacity: checkBox.checked ? 1.0 : 0.5
        }
    }
}
