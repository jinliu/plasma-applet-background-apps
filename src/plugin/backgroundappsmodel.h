/*
    SPDX-FileCopyrightText: 2025 <m.liu.jin@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
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
        MessageRole,
        AppNameRole,
        AppIconRole
    };

    struct App {
        QString appId;
        QString instance;
        QString message;
        QString appName;
        QString appIcon;
    };

public:
    explicit BackgroundAppsModel(QObject *parent = nullptr);
    ~BackgroundAppsModel() override;

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_INVOKABLE void activateApp(const QString &instance);
    Q_INVOKABLE void quitApp(const QString &instance);

    int count() const;

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void countChanged();

private Q_SLOTS:
    void dbusPropertiesChanged(const QString &interfaceName,
        const QVariantMap &properties,
        const QStringList &invalidatedProperties);

private:
    void updateApps(const QList<QVariantMap> &apps);

    QList<App> m_apps;
};
