import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.1
import QtGraphicalEffects 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Messenger")

    //flags: Qt.FramelessWindowHint

    Material.theme: Material.Light
    Material.accent: Material.Blue

    header: Rectangle {
        height: 40

        Rectangle {
            id: headerToolBar
            height: 40
            width: parent.width
            color: Material.color(Material.Blue)

            Row {
                anchors.fill: parent
                Rectangle {
                    width: 250
                    height: parent.height
                    color: Qt.darker(headerToolBar.color)

                    Row {
                        anchors.fill: parent
                        TextInput {
                            id: textInputSearch
                            width: parent.width - parent.height - 10
                            color: "white"
                            verticalAlignment: Text.AlignVCenter
                            padding: 10
                            font.pixelSize: 14

                            Text {
                                anchors.fill: parent
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 14
                                padding: 10
                                color: "#aaa"
                                text: "Search..."
                                visible: !textInputSearch.text
                            }
                        }

                        Image {
                            id: imgPlus
                            width: parent.height / 3
                            height: parent.height / 3
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            source: "plus.png"

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    cursorShape = Qt.PointingHandCursor
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width - 250
                    height: parent.height
                    color: "transparent"

                    Row {
                        anchors.fill: parent
                        Text {
                            height: parent.height
                            verticalAlignment: Text.AlignVCenter
                            padding: 10
                            color: "white"
                            text: "Andrzej"
                            font.pixelSize: 12
                            font.bold: true
                        }

                        Rectangle {
                            color: "#8BC34A"
                            radius: 45
                            height: 5
                            width: 5
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Text {
                            height: parent.height
                            verticalAlignment: Text.AlignVCenter
                            padding: 5
                            color: "white"
                            text: "online"
                            font.pixelSize: 10
                        }
                    }
                }
            }

        }
    }

    ListModel {
        id: myModel
        ListElement {
            name: "Mariusz"
            msg: "This is last message sent to this person. Hello."
        }
        ListElement {
            name: "Andrzej"
            msg: "This is last message sent to this person. Hello."
        }
    }

    Pane {
        Material.elevation: 2
        width: 250
        height: root.height

        padding: 0

        ListView {
            id: list
            anchors.fill: parent
            model: myModel
            delegate: ItemDelegate {
                id: delegate
                width: parent.width
                height: 50

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: {
                        if(mouse.button === Qt.RightButton) {
                            contextMenu.visible = true
                            contextMenu.x = mouseX < 125 ? mouseX : 125
                        }
                        delegate.onClicked
                    }
                }

                Menu {
                    id: contextMenu
                    title: "Action"

                    width: parent.width / 2

                    MenuItem {
                        text: "Add"
                    }

                    MenuItem {
                        text: "Edit"
                    }

                    MenuItem {
                        text: "Remove"
                    }
                }

                Row {
                    width: parent.width
                    padding: 5
                    spacing: 5
                    Image {
                        id: img
                        width: 40
                        height: 40
                        anchors.verticalCenter: parent.verticalCenter
                        x: 50
                        source: "img.png"
                        //smooth: true
                        mipmap: true

                        fillMode: Image.PreserveAspectCrop
                        clip: true

                        layer.enabled: true
                        layer.effect: OpacityMask {
                            maskSource: Item {
                                width: img.width
                                height: img.height

                                Rectangle {
                                    anchors.fill: parent
                                    radius: 45
                                }
                            }
                        }
                    }

                    Column {
                        id: column
                        spacing: 5
                        width: parent.width - img.width
                        anchors.verticalCenter: parent.verticalCenter
                        Text {
                            text: name
                            font.pixelSize: 14
                        }
                        Text {
                            text: msg
                            font.pixelSize: 11
                            color: "#838383"
                            elide: Text.ElideRight
                            width: parent.width - 20
                        }
                    }
                }

                Rectangle {
                    anchors.top: parent.bottom
                    width: parent.width
                    height: 1
                    color: "#d3d3d3"
                }
            }
        }
    }

    ListModel {
        id: msgModel
        ListElement  {
            author: "Mariusz"
            author_index: 0
            content: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam auctor."
        }
        ListElement {
            author: "Mariusz"
            author_index: 0
            content: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis sit amet egestas velit, id dignissim purus. In hac habitasse platea dictumst. Sed a nulla at turpis blandit consequat dictum non neque. Duis sit amet massa non nibh iaculis vulputate. Ut tempus feugiat orci, non posuere quam placerat in. Orci varius."
        }
        ListElement {
            author: "Andrzej"
            author_index: 1
            content: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras risus libero, tempor a ipsum vel, congue venenatis nunc. Nulla vel."
        }
    }

    Pane {
        id: messages
        x: 255
        anchors.leftMargin: 20
        anchors.top: root.top
        padding: 5

        width: root.width - list.width - 5
        height: root.height - header.height

        Rectangle {
            anchors.fill: parent
            color: "#fafafa"
            Rectangle {
                id: msgView
                width: parent.width
                anchors.top: parent.top
                anchors.bottom: textBox.top
                anchors.bottomMargin: 5
                color: "#fafafa"

                //.. fill with messages
                ListView {
                    id: msgListView
                    anchors.fill: parent
                    spacing: 10
                    model: msgModel

                    delegate: Rectangle {
                        width: parent.width
                        height: chatimg.height < messageText.implicitHeight ? messageText.implicitHeight : chatimg.height
                        color: "#fafafa"
                        /*Text {
                            text: author + ": " + content
                        }*/

                        Row {
                            anchors.fill: parent
                            spacing: 5
                            layoutDirection: author_index % 2 == 0 ? "RightToLeft" : "LeftToRight"
                            Image {
                                id: chatimg
                                width: 40
                                height: 40
                                anchors.top: parent.top
                                x: 50
                                source: "img.png"
                                //smooth: true
                                mipmap: true

                                fillMode: Image.PreserveAspectCrop
                                clip: true

                                layer.enabled: true
                                layer.effect: OpacityMask {
                                    maskSource: Item {
                                        width: chatimg.width
                                        height: chatimg.height

                                        Rectangle {
                                            anchors.fill: parent
                                            radius: 45
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                height: parent.height
                                width: parent.width - chatimg.width - 5
                                //border.width: 1
                                //border.color: "#d3d3d3"
                                radius: 5

                                color: author_index % 2 == 0 ? Material.color(Material.Blue) : "#d3d3d3"
                                Material.elevation: 2

                                Text {
                                    id: messageText
                                    anchors.fill: parent
                                    padding: 5
                                    text: content
                                    wrapMode: Text.WordWrap
                                    clip: true
                                    color: author_index % 2 == 0 ? "white" : "black"
                                    font.weight: Font.Medium
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#d3d3d3"
                }

                id: textBox
                width: parent.width
                height: 50
                anchors.bottom: parent.bottom
                color: "#fafafa"

                TextInput {
                    id: textBoxMsg
                    width: parent.width
                    height: parent.height
                    padding: 10
                    font.pixelSize: 14
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: TextInput.Wrap

                    onAccepted: {
                        msgModel.append({
                                            author: "Mariusz",
                                            content: textBoxMsg.text
                                        });
                        textBoxMsg.text = "";
                        msgListView.positionViewAtEnd();
                    }

                    Text {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                        padding: 10
                        color: "#aaa"
                        text: "Enter your message here..."
                        visible: !textBoxMsg.text
                    }
                }
            }

        }
    }
}
