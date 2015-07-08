import QtQuick 2.0
import com.mpertsov.chess 1.0

Item {
    property alias internalFigure : figure.internalFigure
    property Grid targetGrid

    id: root
    width: parent.width
    height: width

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        drag.target: figure
        visible: internalFigure.visible

        onReleased: {
            var dropItem = figure.Drag.target;
            if (dropItem !== null && dropItem.acceptFigure) {
                figure.Drag.drop();
            }
        }

        Connections {
            target: internalFigure
            onBoardIndexChanged: {
                if (newBoardIndex >= 0) {
                    root.parent = targetGrid.children[newBoardIndex];
                    figure.prevParent = root.parent;
                }
            }
        }

        Image {
            property Figure internalFigure
            property Item prevParent : root.parent

            id: figure
            width: parent.width
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            Drag.active: {
                return mouseArea.drag.active &&
                       boardState.turnAvailable(internalFigure.color) &&
                       internalFigure.canMove;
            }
            Drag.hotSpot.x: width / 2
            Drag.hotSpot.y: height / 2

            source: {
                return "../images/images/" + internalFigure.type + "-" + internalFigure.color + ".svg";
            }

            states: State {
                when: {
                    return mouseArea.drag.active &&
                           boardState.turnAvailable(internalFigure.color) &&
                           internalFigure.canMove;
                }
                //ParentChange { target: root; parent: figure.Drag.target}
                AnchorChanges { target: figure; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
            }
        }
    }
}
