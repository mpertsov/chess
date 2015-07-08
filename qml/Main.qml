import QtQuick 2.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.0

Window {
    id: appWindow
    visible: true
    width: 520
    height: 630
    maximumWidth: width
    minimumWidth: width
    maximumHeight: height
    minimumHeight: height

    Grid {
        id:  bottomBoard
        anchors.left: parent.left
        anchors.top: blacksText.bottom
        width: (parent.width < parent.height ? parent.width : parent.height)
        height: width
        columns: 8

        Repeater {
            model: 8 * 8
            Rectangle {
                width: parent.width / 8
                height: width
                color: {
                    var row = Math.floor(index / 8);
                    var col = index % 8;
                    if (row % 2 == 0) {
                        if (col % 2 == 0) {
                            return "#ffce9e";
                        } else {
                             return "#d18b47";
                        }
                    } else {
                        if (col % 2 == 0) {
                            return "#d18b47";
                        } else {
                             return "#ffce9e";
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: boardState
        onStateChanged: {
            if (state == 0) {
                startBtn.visible = true;
                stopBtn.visible = false;
                prevBtn.visible = false;
                nextBtn.visible = false;
                saveBtn.visible = false;
                loadBtn.visible = true;
                figuresBoard.visible = false;
            } else if (state == 2) {
                startBtn.visible = false;
                stopBtn.visible = true;
                prevBtn.visible = false;
                nextBtn.visible = false;
                saveBtn.visible = true;
                loadBtn.visible = false;
                figuresBoard.visible = true;
            } else if (state == 1) {
                startBtn.visible = true;
                stopBtn.visible = false;
                prevBtn.visible = true;
                nextBtn.visible = true;
                saveBtn.visible = false;
                loadBtn.visible = true;
                figuresBoard.visible = true;
            }
        }
    }

    Grid {
        id: figuresBoard
        anchors.left:  bottomBoard.left
        anchors.right:  bottomBoard.right
        anchors.bottom:  bottomBoard.bottom
        anchors.top:  bottomBoard.top
        columns: 8
        visible: false

        Repeater {
            model: 8 * 8
            Rectangle {
                width: parent.width / 8
                height: width
                color: "transparent"
            }
        }
    }

    Grid {
        id: dropBoard
        anchors.left:  bottomBoard.left
        anchors.right:  bottomBoard.right
        anchors.bottom:  bottomBoard.bottom
        anchors.top:  bottomBoard.top
        columns: 8

        Repeater {
            model: 8 * 8
            BoardItem { indexKey: index; bottomGrid:  bottomBoard }
        }
    }

    Rectangle {
        id: whitesText
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top:  bottomBoard.bottom
        height: 30
        color: "white"

        Text {
            id: whitesTurnText
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 16
            font.weight: Font.Bold
            color: "#4d4d4d"
            text: "now turns whites"
            visible: boardState.whitesTurnAvailable
        }
    }

    Rectangle {
        id: blacksText
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 30
        color: "black"

        Text {
            id: blacksTurnText
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 16
            font.weight: Font.Bold
            color: "lightgrey"
            text: "now turns blacks"
            visible: boardState.blacksTurnAvailable
        }
    }

    Rectangle {
        id: buttonsArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: whitesText.bottom
        height: 50
        color: "black"

        Button {
            id: stopBtn
            text: "STOP"
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: parent.width / 4
            height: parent.height
            visible: false

            onClicked: {
                boardState.stopGame();
            }
        }

        Button {
            id :startBtn
            text: "START"
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: parent.width / 4
            height: parent.height
            visible: true

            onClicked: {
                boardState.initBoard(false);
            }
        }

        Button {
            id: saveBtn
            text: "SAVE"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: parent.width / 4
            height: parent.height
            visible: false

            onClicked: {
                var component = Qt.createComponent("SaveDialog.qml");
                component.createObject(appWindow, { });
            }
        }

        Button {
            id: nextBtn
            text: "NEXT >"
            anchors.right: loadBtn.left
            anchors.bottom: parent.bottom
            width: parent.width / 4
            height: parent.height
            visible: false

            onClicked: {
                boardState.historyForward();
            }
        }

        Button {
            id: prevBtn
            text: "< PREV"
            anchors.left: startBtn.right
            anchors.bottom: parent.bottom
            width: parent.width / 4
            height: parent.height
            visible: false

            onClicked: {
                boardState.historyBackward();
            }
        }

        Button {
            id: loadBtn
            text: "LOAD"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: parent.width / 4
            height: parent.height
            visible: true

            onClicked: {
                if (boardState.state == 1) {
                    boardState.loadAccepted();
                } else {
                    var component = Qt.createComponent("OpenDialog.qml");
                    component.createObject(appWindow, { "figuresGrid": figuresBoard });
                }
            }
        }
    }

    Component.onCompleted: {

        boardState.init();

        for (var i = 0; i < 16; ++i) {
            var whiteFigure = boardState.getFigureAt(48 + i);
            var component = Qt.createComponent("FigureItem.qml");
            component.createObject(figuresBoard.children[48 + i], { "internalFigure": whiteFigure, "turnAvailable" : boardState.whitesTurnAvailable,
                                                                  "targetGrid" : figuresBoard});

            var blackFigure = boardState.getFigureAt(i);
            component = Qt.createComponent("FigureItem.qml");
            component.createObject(figuresBoard.children[i], { "internalFigure": blackFigure, "turnAvailable" : boardState.blacksTurnAvailable,
                                                             "targetGrid" : figuresBoard});
        }
    }
}
