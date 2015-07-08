import QtQuick 2.2
import QtQuick.Dialogs 1.0

FileDialog {
    property Grid figuresGrid

    id: openDialog
    title: "Open"
    folder: shortcuts.home
    nameFilters: [ "Save files (*.qcs)" ]
    onAccepted: {
        var success = boardState.loadFromFile(fileUrl);
        if (success) {
            boardState.initBoard(true);
            boardState.historyEnd();

        } else {
            console.log("open failed");
        }

        openDialog.destroy();
    }

    onRejected: {
        openDialog.destroy();
    }

    Component.onCompleted: visible = true
}
