import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtGraphicalEffects 1.15
import com.yrd.VTKCTMLoader 1.0
import QtQuick.Dialogs 1.3
import com.yrd.VTKItem 1.0

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "NEXUS • 3D模型查看器"
    minimumWidth: 1000
    minimumHeight: 700

    // 灰黑色主题
    Material.theme: Material.Dark
    Material.accent: "#4CAF50"  // 绿色强调色
    Material.primary: "#37474F" // 深灰蓝色

    // 深色渐变背景
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1a1a1a" }
            GradientStop { position: 1.0; color: "#2d2d2d" }
        }
    }

    // 标题栏
    Rectangle {
        id: titleBar
        width: parent.width
        height: 70
        color: "#212121"

        // 发光底部边框
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            gradient: Gradient {
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 0.3; color: "#4CAF50" }
                GradientStop { position: 0.7; color: "#4CAF50" }
                GradientStop { position: 1.0; color: "transparent" }
            }
        }

        Label {
            anchors.centerIn: parent
            text: "3D模型查看器"
            color: "white"
            font.pixelSize: 22
            font.bold: true
            font.family: "Microsoft YaHei"
        }
    }

    // 主内容区域
    Rectangle {
        id: contentArea
        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }
        color: "transparent"

        // 3D视图区域
        Rectangle {
            id: viewContainer
            width: parent.width
            height: parent.height * 0.75
            radius: 8
            color: "#263238"
            border.color: "#37474F"
            border.width: 1

            // 内阴影效果
            layer.enabled: true
            layer.effect: InnerShadow {
                radius: 8
                samples: 16
                color: "#40000000"
                spread: 0.2
            }

            VTKItem {
                id: vtkView
                anchors {
                    fill: parent
                    margins: 1
                }
                loader: loader
            }

            // 视图标签
            Rectangle {
                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 12
                }
                width: 80
                height: 28
                color: "#4CAF50"
                radius: 4

                Label {
                    anchors.centerIn: parent
                    text: "3D视图"
                    color: "white"
                    font.pixelSize: 12
                    font.bold: true
                }
            }
        }

        // 控制面板
        Rectangle {
            id: controlPanel
            anchors {
                top: viewContainer.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                topMargin: 20
            }
            radius: 8
            color: "#263238"
            border.color: "#37474F"
            border.width: 1

            // 控制按钮区域
            Column {
                anchors {
                    centerIn: parent
                }
                spacing: 15

                // 第一行按钮
                Row {
                    spacing: 15
                    anchors.horizontalCenter: parent.horizontalCenter

                    // 加载模型按钮
                    TechButton {
                        text: "加载模型"
                        width: 140
                        primary: true
                        onClicked: fileDialog.open()
                    }

                    // 正视图按钮
                    TechButton {
                        text: "正视图"
                        width: 120
                        onClicked: vtkView.setFrontView()
                    }

                    // 放大按钮
                    TechButton {
                        text: "放大"
                        width: 100
                        onClicked: vtkView.zoomIn()
                    }

                    // 缩小按钮
                    TechButton {
                        text: "缩小"
                        width: 100
                        onClicked: vtkView.zoomOut()
                    }
                }

                // 第二行控制项
                Row {
                    spacing: 30
                    anchors.horizontalCenter: parent.horizontalCenter

                    // 鼠标控制开关
                    Rectangle {
                        width: 180
                        height: 40
                        color: "transparent"

                        Row {
                            anchors.centerIn: parent
                            spacing: 12

                            Label {
                                text: "鼠标交互控制:"
                                color: "#B0BEC5"
                                font.pixelSize: 14
                                font.bold: true
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            TechSwitch {
                                checked: vtkView.mouseEnabled
                                onCheckedChanged: vtkView.mouseEnabled = checked
                            }
                        }
                    }

                    // 状态显示
                    Rectangle {
                        width: 200
                        height: 40
                        color: "#37474F"
                        radius: 6

                        Label {
                            anchors.centerIn: parent
                            text: "● 系统就绪"
                            color: "#4CAF50"
                            font.pixelSize: 14
                            font.bold: true
                        }
                    }
                }
            }

            // 底部状态栏
            Rectangle {
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
                height: 40
                color: "#1a1a1a"
                radius: 8

                Row {
                    anchors {
                        fill: parent
                        margins: 15
                    }
                    spacing: 20

                    Label {
                        text: "状态: 就绪"
                        color: "#4CAF50"
                        font.pixelSize: 12
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Label {
                        text: "模型: 未加载"
                        color: "#B0BEC5"
                        font.pixelSize: 12
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Label {
                        text: "渲染: 实时"
                        color: "#B0BEC5"
                        font.pixelSize: 12
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Item { width: 1; height: 1 } // 占位

                    Label {
                        text: "3D Viewer v1.0"
                        color: "#78909C"
                        font.pixelSize: 11
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }

    VTKCTMLoader {
        id: loader
    }

    // 文件对话框
    FileDialog {
        id: fileDialog
        title: "选择3D模型文件"
        nameFilters: ["3D模型文件 (*.ctm *.obj)", "所有文件 (*)"]
        folder: "file:///D:/project/model"

        onAccepted: {
            loader.source = fileDialog.fileUrl.toString().replace("file://", "")
            loader.loadModel()
        }
    }

    // 科技感按钮组件
    component TechButton: Rectangle {
        property string text: ""
        property bool primary: false

        width: 120
        height: 42
        radius: 6
        color: primary ? "#4CAF50" : "transparent"
        border.color: primary ? "#4CAF50" : "#546E7A"
        border.width: 1

        // 悬停效果
        states: [
            State {
                name: "hovered"
                when: mouseArea.containsMouse
                PropertyChanges {
                    target: buttonRoot
                    color: primary ? "#45a049" : "#37474F"
                }
            },
            State {
                name: "pressed"
                when: mouseArea.pressed
                PropertyChanges {
                    target: buttonRoot
                    color: primary ? "#3d8b40" : "#263238"
                }
            }
        ]

        transitions: Transition {
            ColorAnimation { duration: 150 }
        }

        id: buttonRoot

        Label {
            anchors.centerIn: parent
            text: parent.text
            color: primary ? "white" : "#E0E0E0"
            font.pixelSize: 14
            font.bold: true
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: buttonRoot.clicked()
        }

        signal clicked()
    }

    // 科技感开关组件
    component TechSwitch: Rectangle {
        property bool checked: false

        width: 50
        height: 26
        radius: 13
        color: checked ? "#4CAF50" : "#546E7A"
        border.color: checked ? "#4CAF50" : "#78909C"
        border.width: 1

        Rectangle {
            width: 20
            height: 20
            radius: 10
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            x: parent.checked ? parent.width - width - 3 : 3

            Behavior on x {
                NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.checked = !parent.checked
        }
    }
}
