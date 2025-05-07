/*
    SPDX-FileCopyrightText: 2025 Jin Liu <m.liu.jin@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "backgroundappsmodel.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusVariant>
#include <QJsonArray>

const QString DBUS_SERVICE(QString::fromLatin1("org.freedesktop.background.Monitor"));
const QString DBUS_PATH(QString::fromLatin1("/org/freedesktop/background/monitor"));
const QString FDO_DBUS_PROPERTIES(QString::fromLatin1("org.freedesktop.DBus.Properties"));
const QString FDO_BACKGROUND_MONITOR(QString::fromLatin1("org.freedesktop.background.Monitor"));
const QString PROPERTY_NAME(QString::fromLatin1("BackgroundApps"));

BackgroundAppsModel::BackgroundAppsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto dbusInterface = new QDBusInterface(DBUS_SERVICE,
        DBUS_PATH,
        FDO_DBUS_PROPERTIES,
        QDBusConnection::sessionBus(), this);
    QDBusPendingCall pcall = dbusInterface->asyncCall(QLatin1String("Get"), FDO_BACKGROUND_MONITOR, PROPERTY_NAME);
    auto watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
        [&](QDBusPendingCallWatcher *w) {
            QDBusPendingReply<QVariant> reply(*w);
            if (reply.isError()) {
                qWarning() << "Failed to get background apps:" << reply.error().message();
                return;
            }
            auto dbusReply = qdbus_cast<QList<QVariantMap>>(reply.value());
            qWarning() << "Background apps reply:" << dbusReply;
            updateApps(dbusReply);
        });
    QDBusConnection::sessionBus().connect(DBUS_SERVICE,
        DBUS_PATH,
        FDO_DBUS_PROPERTIES,
        QLatin1String("PropertiesChanged"),
        this,
        SLOT(dbusPropertiesChanged(QString, QVariantMap, QStringList)));
}

BackgroundAppsModel::~BackgroundAppsModel() = default;


void BackgroundAppsModel::dbusPropertiesChanged(const QString &interfaceName,
    const QVariantMap &properties,
    const QStringList &invalidatedProperties)
{
    qWarning() << "BackgroundAppsModel::dbusPropertiesChanged" << interfaceName << properties << invalidatedProperties;
    Q_UNUSED(invalidatedProperties);
    if (interfaceName != FDO_BACKGROUND_MONITOR) {
        return;
    }
    QVariantMap::const_iterator it = properties.constBegin();
    while (it != properties.constEnd()) {
        const QString property = it.key();
        if (property == PROPERTY_NAME) {
            auto dbusReply = qdbus_cast<QList<QVariantMap>>(it.value());
            updateApps(dbusReply);
        }
        ++it;
    }
}

void BackgroundAppsModel::updateApps(const QList<QVariantMap> &apps) {
    qWarning() << "BackgroundAppsModel::updateApps" << apps;
    beginResetModel();
    m_apps.clear();
    for (const QVariantMap &appMap : apps) {
        App a;
        a.appId = appMap.value(QStringLiteral("app_id")).toString();
        a.instance = appMap.value(QStringLiteral("instance")).toString();
        a.message = appMap.value(QStringLiteral("message")).toString();
        m_apps.append(a);
        qWarning() << "app:" << a.appId << a.instance << a.message;
    }
    endResetModel();
}

int BackgroundAppsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_apps.count();
}

QHash<int, QByteArray> BackgroundAppsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[AppIdRole] = QByteArrayLiteral("appId");
    roleNames[InstanceRole] = QByteArrayLiteral("instance");
    roleNames[MessageRole] = QByteArrayLiteral("message");
    return roleNames;
}

QVariant BackgroundAppsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.row() < 0 || index.row() >= m_apps.count()) {
        return QVariant();
    }

    const App &app = m_apps.at(index.row());
    switch (role) {
    case AppIdRole:
        return app.appId;
    case InstanceRole:
        return app.instance;
    case MessageRole:
        return app.message;
    }

    return QVariant();
}

#include "moc_backgroundappsmodel.cpp"
