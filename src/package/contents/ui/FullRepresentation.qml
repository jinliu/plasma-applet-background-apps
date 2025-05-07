/*
    SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.plasma.plasmoid
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.extras as PlasmaExtras

PlasmaComponents.Page {
    id: root

    implicitWidth: backgroundapps.switchWidth
    implicitHeight: backgroundapps.switchHeight

    PlasmaComponents.ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth - (contentItem as ListView).leftMargin - (contentItem as ListView).rightMargin

        contentItem: ListView {
            id: appList

            model: !backgroundapps.expanded ? null : backgroundapps.model

            delegate: PlasmaComponents.Label {
                required property string appId
                required property string message
                id: appLabel
                text: appId
            }
        }
    }
}