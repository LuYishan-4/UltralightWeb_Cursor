import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs


ApplicationWindow {
    id: root

    width: 520
    height: 360
    visible: true
    title: qsTr("UltralightWebCursor-GUI")


    FileDialog {
        id: htmlFileDialog
        title: qsTr("Choose HTML")
        nameFilters: ["HTML files (*.html *.htm)"]
        onAccepted: {
            backend.htmlPath = htmlFileDialog.selectedFile.toString().replace("file://", "")
        }
    }

    FolderDialog {
        id: sdkFolderDialog
        title: qsTr("Choose Ultralight SDK ")
        onAccepted: {
            backend.sdkPath = sdkFolderDialog.selectedFolder.toString().replace("file://", "")
        }
    }


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Label {
            text: qsTr("Animation-Setting")
            font.pixelSize: 20
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Enable animation")
                Layout.fillWidth: true
            }

            Switch {
                checked: backend.enabled
                onToggled: {
                    if(checked)
                        backend.enable()
                    else
                        backend.disable()
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4

            Label {
                text: qsTr("HTML Path徑")
                font.bold: true
            }

            RowLayout {
                Layout.fillWidth: true

                TextField {
                    id: htmlPathField
                    Layout.fillWidth: true
                    text: backend.htmlPath
                    placeholderText: qsTr("not setting")
                    onEditingFinished: backend.htmlPath = text

                    color: backend.pathExists(text) ? "black" : "red"
                }

                Button {
                    text: qsTr("find")
                    onClicked: htmlFileDialog.open()
                }
            }

            Label {
                visible: htmlPathField.text.length > 0 && !backend.pathExists(htmlPathField.text)
                text: qsTr("cant not find path")
                color: "red"
                font.pixelSize: 11
            }
        }


        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4

            Label {
                text: qsTr("Ultralight SDK path")
                font.bold: true
            }

            RowLayout {
                Layout.fillWidth: true

                TextField {
                    id: sdkPathField
                    Layout.fillWidth: true
                    text: backend.sdkPath
                    placeholderText: qsTr("not setting")
                    onEditingFinished: backend.sdkPath = text

                    color: backend.pathExists(text) ? "black" : "red"
                }

                Button {
                    text: qsTr("Browse")
                    onClicked: sdkFolderDialog.open()
                }
            }

            Label {
                visible: sdkPathField.text.length > 0 && !backend.pathExists(sdkPathField.text)
                text: qsTr("not find")
                color: "red"
                font.pixelSize: 11
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Label {
            text: backend.statusMessage
            color: "gray"
            font.pixelSize: 12
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                text: qsTr("reload")
                onClicked: backend.reload()
            }

            Button {
                text: qsTr("reload HTML")
                onClicked: backend.reloadHtml()
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("save")
                onClicked: backend.save()
            }

            Button {
                text: qsTr("save and apply")
                highlighted: true
                onClicked: {
                    backend.save()
                    backend.reconfigureKWin()
                }
            }
        }
    }
}
