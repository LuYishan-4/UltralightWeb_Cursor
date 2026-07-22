import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    width: 700
    height: 520
    visible: true
    title: qsTr("UltralightWebCursor-GUI")

    // Folder Dialog for Theme Upload
    FolderDialog {
        id: themeUploadDialog
        title: qsTr("Choose Theme Folder")
        onAccepted: {
            backend.uploadTheme(selectedFolder.toString().replace("file://", ""))
        }
    }

    Page {
        anchors.fill: parent

        // Top Navigation Bar with Header & GitHub Icon
        header: ToolBar {
            contentHeight: tabBar.height

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12

                TabBar {
                    id: tabBar
                    Layout.fillWidth: true
                    background: Rectangle { color: "transparent" }

                    TabButton { text: qsTr("Theme") }
                    TabButton { text: qsTr("Blacklist") }
                    TabButton { text: qsTr("Setting") }
                }

                // GitHub Icon Button
                ToolButton {
                    id: githubButton
                    Layout.preferredWidth: 36
                    Layout.preferredHeight: 36
                    
                    contentItem: Image {
                        source: "assets/github.svg"
                        fillMode: Image.PreserveAspectFit
                        anchors.centerIn: parent
                        sourceSize.width: 24
                        sourceSize.height: 24
                    }

                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Open GitHub Repository")

                    onClicked: {
                        // Hook your backend or Qt.openUrlExternally here
                        if (typeof backend.openGitHub === "function") {
                            backend.openGitHub()
                        } else {
                            Qt.openUrlExternally("https://github.com/LuYishan-4/Cursor-animation")
                        }
                    }
                }
            }
        }

        StackLayout {
            anchors.fill: parent
            currentIndex: tabBar.currentIndex

            /* =========================================================
             * 1. Theme Tab
             * ========================================================= */
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 12

                    Label {
                        text: qsTr("Current Theme")
                        font.bold: true
                    }

                    ComboBox {
                        id: currentThemeBox
                        Layout.fillWidth: true
                        model: backend.themeList
                        currentIndex: backend.themeList.indexOf(backend.currentTheme)
                        onActivated: {
                            backend.useTheme(currentText)
                        }
                    }

                    Label {
                        text: qsTr("Installed Themes")
                        font.bold: true
                    }

                    ListView {
                        id: themeList
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: backend.themeList
                        spacing: 6

                        delegate: Rectangle {
                            width: themeList.width
                            height: 55
                            radius: 6
                            color: backend.currentTheme === modelData ? "#f0f4f8" : "#ffffff"
                            border.color: "#dcdcdc"
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 8
                                spacing: 10

                                RadioButton {
                                    checked: backend.currentTheme === modelData
                                    onClicked: backend.useTheme(modelData)
                                }

                                Label {
                                    text: modelData
                                    Layout.fillWidth: true
                                    font.pixelSize: 14
                                }

                                Button {
                                    text: qsTr("Apply")
                                    onClicked: backend.useTheme(modelData)
                                }

                                Button {
                                    text: qsTr("Open")
                                    onClicked: backend.openThemeFolder(modelData)
                                }

                                Button {
                                    text: qsTr("Delete")
                                    onClicked: backend.removeTheme(modelData)
                                }
                            }
                        }
                    }

                    Button {
                        text: qsTr("Upload Theme")
                        Layout.alignment: Qt.AlignRight
                        onClicked: themeUploadDialog.open()
                    }
                }
            }

            /* =========================================================
             * 2. Blacklist Tab
             * ========================================================= */
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 10

                    Label {
                        text: qsTr("Blacklist")
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        TextField {
                            id: blacklistInput
                            Layout.fillWidth: true
                            placeholderText: qsTr("Application name")
                            onAccepted: addBtn.clicked()
                        }

                        Button {
                            id: addBtn
                            text: qsTr("Add")
                            onClicked: {
                                if (blacklistInput.text.length > 0) {
                                    backend.addBlacklist(blacklistInput.text)
                                    blacklistInput.clear()
                                }
                            }
                        }
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: backend.blacklist
                        spacing: 6

                        delegate: Rectangle {
                            width: parent.width
                            height: 45
                            radius: 6
                            color: "#ffffff"
                            border.color: "#dcdcdc"
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 8

                                Label {
                                    text: modelData
                                    Layout.fillWidth: true
                                    font.pixelSize: 14
                                }

                                Button {
                                    text: qsTr("Delete")
                                    onClicked: backend.removeBlacklist(modelData)
                                }
                            }
                        }
                    }
                }
            }

            /* =========================================================
             * 3. Setting Tab
             * ========================================================= */
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Label {
                        text: qsTr("General")
                        font.bold: true
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
                            onClicked: backend.reload()
                        }

                        Button {
                            text: qsTr("Save")
                            onClicked: backend.save()
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