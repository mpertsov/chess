import QtQuick 2.0

DropArea {
    id: boardTarget
    property int indexKey
    property bool acceptFigure: false
    property Grid bottomGrid

    width: parent.width / 8
    height: width

    onDropped: {
        var figure = boardTarget.drag.source.internalFigure;
        boardState.moveFigureTo(figure, indexKey);

        var bottomItem = bottomGrid.children[indexKey]
        bottomItem.border.width = 0;
    }

    onExited: {
        var bottomItem = bottomGrid.children[indexKey]
        bottomItem.border.width = 0;
    }

    Rectangle {
        id: boardRectangle

        anchors.fill: parent
        color: "transparent"

        states: [
            State {
                when: boardTarget.containsDrag
                PropertyChanges {
                    target: boardTarget
                    acceptFigure: {
                        if (boardTarget.drag.source != null) {
                            var figure = boardTarget.drag.source.internalFigure;
                            var bottomItem = bottomGrid.children[indexKey];
                            bottomItem.border.width = 2;

                            if (boardState.isAvailableForFigure(figure, indexKey)) {
                                bottomItem.border.color = "green";
                                return true;
                            } else {
                                bottomItem.border.color = "red";
                                return false;
                            }

                        } else {
                            return false;
                        }
                    }
                }
            }
        ]
    }
}
