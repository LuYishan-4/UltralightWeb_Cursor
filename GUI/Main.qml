import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material

ApplicationWindow {
    id: root

    width: 760
    height: 560

    visible: true

    title: qsTr("UltralightWebCursor-GUI")

    Material.theme: Material.Light
    Material.accent: "#3daee9"

    FolderDialog {
        id: themeUploadDialog

        title: qsTr("Choose Theme Folder")

        onAccepted: {
            backend.uploadTheme(
                selectedFolder.toLocalFile()
            )
        }
    }

    Page {
        anchors.fill: parent

        background: Rectangle {
            color: "#f5f6f8"
        }

        header: ToolBar {
            height: 52

            background: Rectangle {
                color: "#ffffff"

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: "#dddddd"
                }
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16

                TabBar {
                    id: tabBar

                    Layout.fillWidth: true

                    background: Rectangle {
                        color: "transparent"
                    }

                    TabButton {
                        text: qsTr("Theme")
                    }

                    TabButton {
                        text: qsTr("Blacklist")
                    }

                    TabButton {
                        text: qsTr("Setting")
                    }
                }

                ToolButton {
                    id: githubButton

                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40

                    contentItem: Image {
                        source: "assets/github.svg"

                        fillMode: Image.PreserveAspectFit

                        sourceSize.width: 24
                        sourceSize.height: 24
                    }

                    ToolTip.visible: hovered
                    ToolTip.text:
                        qsTr("Open GitHub Repository")

                    onClicked: {
                        if (typeof backend.openGitHub === "function")
                            backend.openGitHub()
                        else
                            Qt.openUrlExternally(
                                "https://github.com/LuYishan-4/Cursor-animation"
                            )
                    }
                }
            }
        }

        StackLayout {
            anchors.fill: parent

            currentIndex: tabBar.currentIndex

            /*
             * Theme
             */
            Item {
                ColumnLayout {
                    anchors.fill: parent

                    anchors.margins: 20

                    spacing: 14

                    Label {
                        text: qsTr("Current Theme")

                        font.bold: true

                        font.pixelSize: 18
                    }

                    ComboBox {
                        id: currentThemeBox

                        Layout.fillWidth: true

                        model: backend.themeList

                        currentIndex:
                            backend.themeList.indexOf(
                                backend.currentTheme
                            )

                        onActivated: {
                            backend.useTheme(currentText)
                        }
                    }

                    Label {
                        text: qsTr("Installed Themes")

                        font.bold: true

                        font.pixelSize: 16
                    }

                    ListView {
                        id: themeList

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        spacing: 10

                        clip: true

                        model: backend.themeList

                        delegate: Rectangle {
                            width: themeList.width

                            height: 60

                            radius: 10

                            color:
                                backend.currentTheme === modelData
                                ?
                                "#e8f3ff"
                                :
                                "#ffffff"

                            border.width: 1

                            border.color:
                                backend.currentTheme === modelData
                                ?
                                "#3daee9"
                                :
                                "#dddddd"

                            Behavior on color {
                                ColorAnimation {
                                    duration: 150
                                }
                            }

                            RowLayout {
                                anchors.fill: parent

                                anchors.margins: 10

                                spacing: 12

                                RadioButton {
                                    checked:
                                        backend.currentTheme === modelData

                                    onClicked: {
                                        backend.useTheme(modelData)
                                    }
                                }

                                Label {
                                    text: modelData

                                    Layout.fillWidth: true

                                    font.pixelSize: 14
                                }

                                Button {
                                    text: qsTr("Apply")

                                    Layout.preferredHeight: 34

                                    onClicked:
                                        backend.useTheme(modelData)
                                }

                                Button {
                                    text: qsTr("Open")

                                    Layout.preferredHeight: 34

                                    onClicked:
                                        backend.openThemeFolder(modelData)
                                }

                                Button {
                                    text: qsTr("Delete")

                                    Layout.preferredHeight: 34

                                    onClicked:
                                        backend.removeTheme(modelData)
                                }
                            }
                        }
                    }

                    Button {
                        text: qsTr("Upload Theme")

                        Layout.alignment:
                            Qt.AlignRight

                        Layout.preferredHeight: 36

                        onClicked:
                            themeUploadDialog.open()
                    }
                }
            }

            /*
             * Blacklist
             */
            Item {
                ColumnLayout {
                    anchors.fill: parent

                    anchors.margins: 20

                    spacing: 14

                    Label {
                        text: qsTr("Blacklist")

                        font.bold: true

                        font.pixelSize: 18
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        spacing: 10

                        TextField {
                            id: blacklistInput

                            Layout.fillWidth: true

                            placeholderText:
                                qsTr("Application name")

                            onAccepted:
                                addBtn.clicked()
                        }

                        Button {
                            id: addBtn

                            text: qsTr("Add")

                            Layout.preferredHeight: 36

                            onClicked: {
                                if (blacklistInput.text.length > 0) {
                                    backend.addBlacklist(
                                        blacklistInput.text
                                    )

                                    blacklistInput.clear()
                                }
                            }
                        }
                    }

                    ListView {
                        Layout.fillWidth: true

                        Layout.fillHeight: true

                        spacing: 10

                        clip: true

                        model: backend.blacklist

                        delegate: Rectangle {
                            width: parent.width

                            height: 50

                            radius: 8

                            color: "#ffffff"

                            border.width: 1
                            border.color: "#dddddd"

                            RowLayout {
                                anchors.fill: parent

                                anchors.margins: 10

                                Label {
                                    text: modelData

                                    Layout.fillWidth: true
                                }

                                Button {
                                    text: qsTr("Delete")

                                    Layout.preferredHeight: 34

                                    onClicked:
                                        backend.removeBlacklist(modelData)
                                }
                            }
                        }
                    }
                }
            }

            /*
             * Setting
             */
            Item {
                ColumnLayout {
                    anchors.fill: parent

                    anchors.margins: 20

                    spacing: 18

                    Label {
                        text: qsTr("General")

                        font.bold: true

                        font.pixelSize: 18
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        Label {
                            text: qsTr("Enable Animation")

                            Layout.fillWidth: true
                        }

                        Switch {
                            checked: backend.enabled

                            onToggled: {
                                if (checked)
                                    backend.enable()
                                else
                                    backend.disable()
                            }
                        }
                    }

                    Label {
                        text: backend.statusMessage

                        color: "gray"

                        font.italic: true
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        spacing: 10

                        Button {
                            text: qsTr("Reload")

                            onClicked:
                                backend.reload()
                        }

                        Button {
                            text: qsTr("Save")

                            onClicked:
                                backend.save()
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Button {
                            highlighted: true

                            text: qsTr("Apply")

                            onClicked: {
                                backend.save()

                                backend.reconfigureKWin()
                            }
                        }
                    }
                }
            }
        }
    }
}