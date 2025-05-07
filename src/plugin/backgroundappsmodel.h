/*
    SPDX-FileCopyrightText: 2025 Jin Liu <m.liu.jin@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QAbstractListModel>
#include <QStringList>
#include <qqmlregistration.h>

class QDBusServiceWatcher;

class BackgroundAppsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    enum BackgroundAppsModelRole {
        AppIdRole = Qt::UserRole + 1, // TODO this should be Qt::DisplayRole
        InstanceRole,
        MessageRole
    };

    struct App {
        QString appId;
        QString instance;
        QString message;
    };

public:
    explicit BackgroundAppsModel(QObject *parent = nullptr);
    ~BackgroundAppsModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

private Q_SLOTS:
    void dbusPropertiesChanged(const QString &interfaceName,
        const QVariantMap &properties,
        const QStringList &invalidatedProperties);

private:
    void updateApps(const QList<QVariantMap> &apps);

    QList<App> m_apps;
};
