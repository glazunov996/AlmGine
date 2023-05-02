import QtQuick

Item {
    id: root

    property var controlledObject: undefined

    property real speed: 0
    property real maxMoveSpeed: 6
    property real accelerationStep: 0.2
    property real decelerationStep: 0.1
    property real rotateStep: 2
    property real delta: 0
    property real gravity: 0
    property real seaLevel: 0

    property alias moveForward: status.moveForward
    property alias moveBack: status.moveBack
    property alias rotateLeft: status.rotateLeft
    property alias rorateRight: status.rotateRight

    property bool mouseEnabled: true
    property bool keysEnabled: true
    focus: keysEnabled

    TapHandler {
        onTapped: root.forceActiveFocus()
    }

    Keys.onPressed: (event)=> { if (keysEnabled) handleKeyPress(event) }
    Keys.onReleased: (event)=> { if (keysEnabled && !event.isAutoRepeat) handleKeyRelease(event) }

    function forwardPressed(event) {
        status.moveForward = true;
        status.moveBack = false;
        status.decelerating = false;
    }

    function forwardReleased() {
        status.moveForward = false;
        status.decelerating = true;
    }

    function backPressed() {
        status.moveBack = true;
        status.moveForward = false;
        status.decelerating = false;
    }

    function backReleased() {
        status.moveBack = false
        status.decelerating = true;
    }

    function leftPressed() {
        status.rotateLeft = true;
        status.rotateRight = false;
    }

    function leftReleased() {
        status.rotateLeft = false;
    }

    function rightPressed() {
        status.rotateRight = true;
        status.rotateLeft = false;
    }

    function rightReleased() {
        status.rotateRight = false;
    }

    function handleKeyPress(event)
    {
        switch (event.key) {
        case Qt.Key_Up:
            forwardPressed();
            break;
        case Qt.Key_Down:
            backPressed();
            break;
        case Qt.Key_Left:
            leftPressed();
            break;
        case Qt.Key_Right:
            rightPressed();
            break;
        }
    }

    function handleKeyRelease(event)
    {
        switch (event.key) {
        case Qt.Key_Up:
            forwardReleased();
            break;
        case Qt.Key_Down:
            backReleased();
            break;
        case Qt.Key_Left:
            leftReleased();
            break;
        case Qt.Key_Right:
            rightReleased();
            break;
        }
    }

    FrameAnimation {
        id: updateTimer
        running: true
        onTriggered: status.processInput(frameTime * 100)
    }

    Item {
        id: status

        property bool moveForward: false
        property bool moveBack: false
        property bool rotateLeft: false
        property bool rotateRight: false
        property real velocityY: 0

        property bool decelerating: false

        function updatePosition(vector, speed, position, gravity, delta)
        {
            var direction = vector;
            var velocity = Qt.vector3d(direction.x * speed,
                                       velocityY,
                                       direction.z * speed);

            if (delta > 0) {
                velocity.y += delta * 0.015;
                position.y += delta * 0.15;
            }

            velocity.y += gravity;

            if (position.y < seaLevel) {
                velocity.y = 0;
                position.y = seaLevel;
            }

            position = Qt.vector3d(position.x + velocity.x,
                                   position.y + velocity.y,
                                   position.z + velocity.z);

            controlledObject.position = position;
            velocityY = velocity.y;
        }

        function processInput(frameDelta) {
            if (controlledObject == undefined)
                return;

            if (moveForward) {
                speed += accelerationStep;
                if (speed > maxMoveSpeed)
                    speed = maxMoveSpeed;

            } else if (moveBack) {
                speed -= accelerationStep;
                if (speed < -maxMoveSpeed)
                    speed = -maxMoveSpeed;
            }

            if (decelerating) {
                if (speed > decelerationStep) {
                    speed -= decelerationStep
                } else if (speed < -decelerationStep) {
                    speed += decelerationStep
                } else {
                    speed = 0
                    decelerating = false;
                }
            }

            updatePosition(controlledObject.forward, speed * frameDelta, controlledObject.position, gravity, delta);

            var rotationVector = controlledObject.rotation;

            if (rotateLeft) {
                let rotateX = -rotateStep * frameDelta;
                rotationVector.y += rotateX;
            } else if (rotateRight) {
                let rotateX = rotateStep * frameDelta;
                rotationVector.y += rotateX;
            }

            controlledObject.rotation = rotationVector;
        }
    }
}
