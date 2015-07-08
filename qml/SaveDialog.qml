import QtQuick 2.2
import QtQuick.Dialogs 1.0

FileDialog {
    id: saveDialog
    title: "Save"
    folder: shortcuts.home
    selectExisting: false
    nameFilters: [ "Save files (*.qcs)" ]
    onAccepted: {
        var success = boardState.saveToFile(fileUrl);
        if (!success) {
            console.log("save failed");
        }

        saveDialog.destroy();
    }

    onRejected: {
        saveDialog.destroy();
    }

    Component.onCompleted: visible = true
}
