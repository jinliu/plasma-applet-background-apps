/*
    SPDX-FileCopyrightText: 2025 <m.liu.jin@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami

import io.github.jinliu.backgroundappsplugin

PlasmoidItem {
    id: backgroundapps

    property var model: BackgroundAppsModel {}

    switchWidth: Kirigami.Units.gridUnit * 12
    switchHeight: Kirigami.Units.gridUnit * 12

    Plasmoid.icon: "applications-all-symbolic"
    Plasmoid.status: model.count > 0 ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.HiddenStatus

    fullRepresentation: FullRepresentation {
    }
}
