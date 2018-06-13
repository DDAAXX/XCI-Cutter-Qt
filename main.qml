import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import Qt.labs.platform 1.0

import comp.xcicutter 1.0
import QtQuick.Layouts 1.0

Window {
    id: window
    visible: true
    width: 640
    height: 330
    minimumHeight: 330
    maximumHeight: 330
    minimumWidth: 640
    maximumWidth: 640
    title: qsTr("XCI Cutter")

    Button {
        id: btn_Batch
        x: 432
        y: 16
        width: 200
        height: 40
        text: qsTr("Batch (TODO)")
        enabled: false

        style: ButtonStyle {
            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 25
                border.width: control.activeFocus ? 2 : 1
                border.color: "#888"
                radius: 4
                gradient: Gradient {
                    GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                    GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                }
            }
        }
    }

    Button {
        id: btn_Source
        x: 10
        y: 107
        width: 200
        height: 40
        text: qsTr("Source")
        style: ButtonStyle {
            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 25
                border.width: control.activeFocus ? 2 : 1
                border.color: "#888"
                radius: 4
                gradient: Gradient {
                    GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                    GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                }
            }
        }

        onClicked: srcFileDialog.visible = true
    }

    FileDialog {
        id: srcFileDialog
        title: "Please choose a XCI file"
        acceptLabel: "Set XCI Source"
        defaultSuffix: "xci"
        fileMode: FileDialog.OpenFile
        nameFilters: ["Switch XCI Dump (*.xci)"]
        onAccepted: {
            var file = ""+srcFileDialog.currentFile
            file = file.replace("file://","")
            console.log("You chose: " + file)
            worker.setSource(file)
            lbl_Source.text = file

            btn_Process.enabled = (lbl_Source.text != "..." && lbl_Dest.text != "...")

            abortOp()
        }
        onRejected: {
            console.log("Canceled")
            worker.setSource("")
            lbl_Source.text = ""
        }
    }

    Button {
        id: btn_Dest
        x: 10
        y: 153
        width: 200
        height: 40
        text: qsTr("Destination")
        style: ButtonStyle {
            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 25
                border.width: control.activeFocus ? 2 : 1
                border.color: "#888"
                radius: 4
                gradient: Gradient {
                    GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                    GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                }
            }
        }

        onClicked: dstFileDialog.visible = true
    }

    FolderDialog {
        id: dstFileDialog
        title: "Please choose a Folder"
        onAccepted: {
            var file = ""+dstFileDialog.currentFolder
            file = file.replace("file://","")
            console.log("You chose: " + file)
            worker.setDest(file)
            lbl_Dest.text = file

            btn_Process.enabled = (lbl_Source.text != "..." && lbl_Dest.text != "...")
        }
        onRejected: {
            console.log("Canceled")
            worker.setDest("")
            lbl_Dest.text = ""
        }
    }

    Label {
        id: lbl_Source
        x: 215
        y: 107
        width: 409
        height: 40
        text: qsTr("...")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Label {
        id: lbl_Dest
        x: 215
        y: 153
        width: 409
        height: 40
        text: qsTr("...")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Label {
        id: label1
        x: 15
        y: 232
        width: 90
        height: 16
        text: qsTr("Cartridge Size:")
        horizontalAlignment: Text.AlignRight
    }

    Label {
        id: label2
        x: 15
        y: 254
        width: 90
        height: 16
        text: qsTr("Data Size:")
        horizontalAlignment: Text.AlignRight
    }

    Label {
        id: label3
        x: 15
        y: 276
        width: 90
        height: 16
        text: qsTr("XCI-File Size:")
        horizontalAlignment: Text.AlignRight
    }

    Label {
        id: lbl_CSize
        x: 119
        y: 232
        width: 90
        height: 16
        horizontalAlignment: Text.AlignHCenter
    }

    Label {
        id: lbl_DSize
        x: 119
        y: 254
        width: 90
        height: 16
        horizontalAlignment: Text.AlignHCenter
    }

    Label {
        id: lbl_XCISize
        x: 119
        y: 276
        width: 90
        height: 16
        horizontalAlignment: Text.AlignHCenter
    }

    Button {
        id: btn_Exit
        x: 432
        y: 242
        width: 200
        height: 40
        text: qsTr("EXIT")
        style: ButtonStyle {
            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 25
                border.width: control.activeFocus ? 2 : 1
                border.color: "#888"
                radius: 4
                gradient: Gradient {
                    GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                    GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                }
            }
        }

        onClicked: {
            worker.processClicked(Worker.EXIT)
        }
    }

    Button {
        id: btn_Process
        x: 221
        y: 242
        width: 200
        height: 40
        text: qsTr("PROCESS")
        enabled: false
        style: ButtonStyle {
            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 25
                border.width: control.activeFocus ? 2 : 1
                border.color: "#888"
                radius: 4
                gradient: Gradient {
                    GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                    GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                }
            }
        }

        onClicked: {
            if (text == "PROCESS")
            {
                text = "QUIT"
                msgBox.text = "Verifying XCI file free space..."

                disableButtons()

                worker.processClicked(Worker.PROCESS, lbl_Source,lbl_Dest, rb_Cut.checked ? checkBox.checked ? Worker.CUT_SPLIT : Worker.CUT : Worker.JOIN)
            }
            else
            {
                worker.processClicked(Worker.QUIT)

                abortOp()
            }
        }
    }

    ProgressBar {
        id: progressBar
        x: 10
        y: 300
        width: 621
        height: 21
        value: 0
    }

    RowLayout {
        x: 10
        y: 24
        spacing: 6.1

        Label {
            id: label
            text: qsTr("Mode:")
        }

        RadioButton {
            id: rb_Cut
            text: qsTr("Cut/Split")
            checked: true

            onCheckedChanged: {
                rb_Uncut.checked = !checked
                checkBox.enabled = checked
            }
        }

        RadioButton {
            id: rb_Uncut
            text: qsTr("Uncut/Join")
            enabled: false

            onCheckedChanged: {
                rb_Cut.checked = !checked
                if (checked)
                {
                    checkBox.enabled = false
                    checkBox.checked = false
                }
            }
        }

        CheckBox {
            id: checkBox
            text: qsTr("Split to 4GB parts (TODO)")
            Layout.fillWidth: true
            Layout.preferredHeight: 21
            Layout.preferredWidth: 168
            enabled: false
        }
    }

    Label {
        id: msgBox
        x: 15
        y: 199
        width: 609
        height: 16
        text: qsTr("")
        horizontalAlignment: Text.AlignHCenter
    }

    function getData(a,b,c)
    {
        console.log(a,b,c)

        lbl_CSize.text = a + " MB";
        lbl_DSize.text = (b/1024/1024).toFixed(0) + " MB";
        lbl_XCISize.text = (c/1024/1024).toFixed(0) + " MB";
    }

    function setProgessMax(val)
    {
        progressBar.minimumValue = 0
        progressBar.maximumValue = val
    }

    function setProgessVal(val)
    {
        progressBar.value = val
    }

    function setMsgBox(val)
    {
        msgBox.text = val
    }

    function abortOp()
    {
        btn_Process.text = "PROCESS"
        msgBox.text = ""
        progressBar.value = 0

        enableButtons()
    }

    function disableButtons()
    {
        btn_Source.enabled = false
        btn_Dest.enabled = false
        btn_Exit.enabled = false
    }

    function enableButtons()
    {
        btn_Source.enabled = true
        btn_Dest.enabled = true
        btn_Exit.enabled = true
    }

    function endProcess()
    {
        btn_Process.text = "PROCESS"
        msgBox.text = "DONE!"
        progressBar.value = progressBar.maximumValue
        enableButtons()
    }
}
