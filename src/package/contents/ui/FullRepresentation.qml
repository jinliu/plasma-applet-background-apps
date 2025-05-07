/*
    SPDX-FileCopyrightText: 2025 <m.liu.jin@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.plasmoid
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PC3
import org.kde.plasma.extras as PlasmaExtras

PC3.Page {
    id: root

    ListView {
        id: appList
        anchors.fill: parent

        model: backgroundapps.expanded ? backgroundapps.model : null
        spacing: Kirigami.Units.gridUnit

        delegate: PlasmaExtras.ListItem {
            width: appList.width
            implicitHeight: appRow.implicitHeight

            required property string appId
            required property string instance
            required property string message
            required property string appName
            required property string appIcon

            RowLayout {
                id: appRow
                anchors.fill: parent

                Kirigami.Icon {
                    source: appIcon
                }

                PC3.Label {
                    Layout.fillWidth: true
                    text: appName
                }

                PC3.Label {
                    Layout.fillWidth: true
                    text: message
                }

                PC3.Button {
                    text: i18n("Show")
                    onClicked: {
                        backgroundapps.model.activateApp(instance)
                    }
                }

                PC3.Button {
                    text: i18n("Quit")
                    onClicked: {
                        backgroundapps.model.quitApp(instance)
                    }
                }
            }            
        }
    }
}
