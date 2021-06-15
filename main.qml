// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

import QtQuick 2.10
import QtQuick.Controls 2.10
import org.kde.idk 1.0
import org.kde.kirigami 2.10 as Kirigami

import "test.qml" as Gay

Kirigami.ApplicationWindow {
    Rectangle {
        color: "green"
        width: 200
        height: 200
        visible: true

        Button {
            anchors.centerIn: parent
            text: "eee"
            onClicked: Singleton.foo().then((val) => {
                console.warn(val)
            })
        }
    }
}