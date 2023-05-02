import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property alias sliderText: valueText.text
    property alias sliderValue: slider.value
    readonly property bool highlight: slider.hovered || slider.pressed

    width: rowLayout.width
    height: rowLayout.height

    RowLayout {
        id: rowLayout
        Slider {
            id: slider
            from: 0
            to: 5
            stepSize: 1
            Layout.minimumWidth: 200
            Layout.maximumWidth: 200
            background: Rectangle {
                x: slider.leftPadding
                y: slider.topPadding + slider.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 4
                width: slider.availableWidth
                height: implicitHeight
                radius: 1
                color: "#222840"

                Rectangle {
                    width: slider.visualPosition * parent.width
                    height: parent.height
                    color: "#848895"
                    radius: 1
                }
            }
            handle: Rectangle {
                x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
                y: slider.topPadding + slider.availableHeight / 2 - height / 2
                implicitWidth: 16
                implicitHeight: 16
                radius: 3
                color: slider.pressed ? "#222840" : "#6b7080"
                border.color: "#848895"
            }
        }
        Label {
            id: valueText
            color: "#ffffff"
            font.pointSize: 10
            Layout.minimumWidth: 80
            Layout.maximumWidth: 80
        }
    }
}
