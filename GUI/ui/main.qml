import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami
import org.kde.kcmutils

SimpleKCM {
    id: root

    title: qsTr("WebCursor")

    property int currentTab: 0

    FolderDialog {
        id: themeUploadDialog
        title: qsTr("Choose Theme Folder")

        onAccepted: {
            backend.uploadTheme(
                selectedFolder.toLocalFile()
            )
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        // =========================
        // Header
        // =========================

        RowLayout {
            Layout.fillWidth: true

            Kirigami.Heading {
                text: qsTr("Ultralight Web Cursor")
                level: 2
                Layout.fillWidth: true
            }

            Button {
                icon.name: "internet-services"
                text: qsTr("GitHub")

                onClicked: {
                    Qt.openUrlExternally(
                        "https://github.com/LuYishan-4/Cursor-animation"
                    )
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        // =========================
        // Tabs
        // =========================

        TabBar {
            id: tabBar
            Layout.fillWidth: true

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

        StackLayout {
            Layout.fillWidth: true
            currentIndex: tabBar.currentIndex

            // =====================================================
            // Theme
            // =====================================================

            ColumnLayout {
                spacing: Kirigami.Units.largeSpacing

                Kirigami.Heading {
                    text: qsTr("Current Theme")
                    level: 3
                }

                ComboBox {
                    id: themeBox
                    Layout.fillWidth: true
                    model: backend.themeList

                    currentIndex:
                        backend.themeList.indexOf(
                            backend.currentTheme
                        )

                    onActivated: {
                        backend.useTheme(
                            currentText
                        )
                    }
                }

                Kirigami.Heading {
                    text: qsTr("Installed Themes")
                    level: 3
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 300
                    clip: true
                    spacing: Kirigami.Units.smallSpacing
                    model: backend.themeList

                    delegate: Kirigami.Card {
                        width: ListView.view.width

                        header:
                        Kirigami.Heading {
                            text: modelData
                            level: 4
                        }

                        contentItem:
                        RowLayout {
                            Button {
                                text: qsTr("Apply")
                                icon.name:
                                    "dialog-ok"

                                onClicked: {
                                    backend.useTheme(
                                        modelData
                                    )
                                }
                            }

                            Button {
                                text: qsTr("Open")

                                icon.name:
                                    "document-open-folder"

                                onClicked: {
                                    backend.openThemeFolder(
                                        modelData
                                    )
                                }
                            }

                            Button {
                                text: qsTr("Delete")

                                icon.name:
                                    "edit-delete"

                                onClicked: {
                                    backend.removeTheme(
                                        modelData
                                    )
                                }
                            }
                        }
                    }
                }

                Button {
                    text: qsTr("Upload Theme")

                    icon.name:
                        "folder-upload"

                    Layout.alignment:
                        Qt.AlignRight

                    onClicked:
                        themeUploadDialog.open()
                }
            }

            // =====================================================
            // Blacklist
            // =====================================================

            ColumnLayout {
                spacing:
                    Kirigami.Units.largeSpacing

                Kirigami.Heading {
                    text:
                        qsTr("Blacklist")

                    level: 3
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 300

                    model:
                        backend.blacklist

                    delegate:
                    Kirigami.Card {
                        width:
                            ListView.view.width

                        contentItem:
                        RowLayout {
                            Label {
                                text: modelData
                                Layout.fillWidth: true
                            }

                            Button {
                                text:
                                    qsTr("Remove")

                                icon.name:
                                    "edit-delete"

                                onClicked: {
                                    backend.removeBlacklist(
                                        modelData
                                    )
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    TextField {
                        id: blacklistInput

                        placeholderText:
                            qsTr("Application name")

                        Layout.fillWidth: true
                    }

                    Button {
                        text:
                            qsTr("Add")

                        onClicked: {
                            if (
                                blacklistInput.text.length > 0
                            ) {
                                backend.addBlacklist(
                                    blacklistInput.text
                                )

                                blacklistInput.clear()
                            }
                        }
                    }
                }
            }

            // =====================================================
            // Settings
            // =====================================================

            ColumnLayout {
                spacing:
                    Kirigami.Units.largeSpacing

                Kirigami.FormLayout {
                    Layout.fillWidth: true

                    TextField {
                        Kirigami.FormData.label:
                            qsTr("HTML Path")

                        text:
                            backend.htmlPath

                        onEditingFinished:
                            backend.htmlPath =
                                text
                    }

                    TextField {
                        Kirigami.FormData.label:
                            qsTr("SDK Path")

                        text:
                            backend.sdkPath

                        onEditingFinished:
                            backend.sdkPath =
                                text
                    }

                    SpinBox {
                        Kirigami.FormData.label:
                            qsTr("Cursor Width")

                        value:
                            backend.cursorWidth

                        from: 1
                        to: 512

                        onValueModified:
                            backend.cursorWidth =
                                value
                    }

                    SpinBox {
                        Kirigami.FormData.label:
                            qsTr("Cursor Height")

                        value:
                            backend.cursorHeight

                        from: 1
                        to: 512

                        onValueModified:
                            backend.cursorHeight =
                                value
                    }
                }

                RowLayout {
                    Button {
                        text:
                            qsTr("Enable")

                        icon.name:
                            "media-playback-start"

                        onClicked:
                            backend.enable()
                    }

                    Button {
                        text:
                            qsTr("Disable")

                        icon.name:
                            "media-playback-stop"

                        onClicked:
                            backend.disable()
                    }

                    Button {
                        text:
                            qsTr("Reload HTML")

                        icon.name:
                            "view-refresh"

                        onClicked:
                            backend.reconfigureKWin()
                    }
                }
            }
        }

        // =========================
        // Status
        // =========================

        Kirigami.InlineMessage {
            Layout.fillWidth: true

            visible:
                backend.statusMessage.length > 0

            text:
                backend.statusMessage

            type:
                Kirigami.MessageType.Information
        }
    }
}