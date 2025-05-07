/*
    SPDX-FileCopyrightText: 2025 <m.liu.jin@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "backgroundappsmodel.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusVariant>
#include <QJsonArray>

#include <KService>

const QString DBUS_SERVICE_BGMON(QString::fromLatin1("org.freedesktop.background.Monitor"));
const QString DBUS_PATH_BGMON(QString::fromLatin1("/org/freedesktop/background/monitor"));
const QString DBUS_INTERFACE_BGMON(QString::fromLatin1("org.freedesktop.background.Monitor"));
const QString PROPERTY_NAME_BGMON(QString::fromLatin1("BackgroundApps"));
const QString DBUS_INTERFACE_FDO_DBUS_PROPERTIES(QString::fromLatin1("org.freedesktop.DBus.Properties"));
const QString DBUS_INTERFACE_FDO_APPLICATION(QStringLiteral("org.freedesktop.Application"));

BackgroundAppsModel::BackgroundAppsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto dbusInterface = new QDBusInterface(DBUS_SERVICE_BGMON,
        DBUS_PATH_BGMON,
        DBUS_INTERFACE_FDO_DBUS_PROPERTIES,
        QDBusConnection::sessionBus(), this);
    QDBusPendingCall pcall = dbusInterface->asyncCall(QLatin1String("Get"), DBUS_INTERFACE_BGMON, PROPERTY_NAME_BGMON);
    auto watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
        [this, dbusInterface](QDBusPendingCallWatcher *w) {
            QDBusPendingReply<QVariant> reply(*w);
            if (reply.isError()) {
                qWarning() << "Failed to get background apps:" << reply.error().message();
                return;
            }
            auto dbusReply = qdbus_cast<QList<QVariantMap>>(reply.value());
            updateApps(dbusReply);
            w->deleteLater();
            dbusInterface->deleteLater();
        });
    QDBusConnection::sessionBus().connect(DBUS_SERVICE_BGMON,
        DBUS_PATH_BGMON,
        DBUS_INTERFACE_FDO_DBUS_PROPERTIES,
        QLatin1String("PropertiesChanged"),
        this,
        SLOT(dbusPropertiesChanged(QString, QVariantMap, QStringList)));
}

BackgroundAppsModel::~BackgroundAppsModel() = default;


void BackgroundAppsModel::dbusPropertiesChanged(const QString &interfaceName,
    const QVariantMap &properties,
    const QStringList &invalidatedProperties)
{
    Q_UNUSED(invalidatedProperties);
    if (interfaceName != DBUS_INTERFACE_BGMON) {
        return;
    }
    QVariantMap::const_iterator it = properties.constBegin();
    while (it != properties.constEnd()) {
        const QString property = it.key();
        if (property == PROPERTY_NAME_BGMON) {
            auto dbusReply = qdbus_cast<QList<QVariantMap>>(it.value());
            updateApps(dbusReply);
        }
        ++it;
    }
}

void BackgroundAppsModel::updateApps(const QList<QVariantMap> &apps) {
    beginResetModel();
    m_apps.clear();
    for (const QVariantMap &appMap : apps) {
        App a;
        a.appId = appMap.value(QStringLiteral("app_id")).toString();
        a.instance = appMap.value(QStringLiteral("instance")).toString();
        a.message = appMap.value(QStringLiteral("message")).toString();
        auto servicePtr = KService::serviceByDesktopName (a.appId);
        if (servicePtr) {
            a.appName = servicePtr->name();
            a.appIcon = servicePtr->icon();
        }
        m_apps.append(a);
    }
    endResetModel();
    Q_EMIT countChanged();
}

int BackgroundAppsModel::count() const
{
    return rowCount();
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
    roleNames[AppNameRole] = QByteArrayLiteral("appName");
    roleNames[AppIconRole] = QByteArrayLiteral("appIcon");
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
    case AppNameRole:
        return app.appName;
    case AppIconRole:
        return app.appIcon;
    }

    return QVariant();
}

void BackgroundAppsModel::activateApp(const QString &instance) {
    for (auto &app: m_apps) {
        if (app.instance != instance) {
            continue;
        }

        const QString dbugService = app.appId;
        const QString dbusPath = QStringLiteral("/") + app.appId.replace(QLatin1Char('.'), QLatin1Char('/')).replace(QLatin1Char('-'), QLatin1Char('_'));
        auto dbusInterface = new QDBusInterface(dbugService, dbusPath, DBUS_INTERFACE_FDO_APPLICATION, QDBusConnection::sessionBus(), this);
        QDBusPendingCall pcall = dbusInterface->asyncCall(QLatin1String("Activate"), QVariantMap());
        auto watcher = new QDBusPendingCallWatcher(pcall, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [dbusInterface](QDBusPendingCallWatcher *w) {
                QDBusPendingReply reply(*w);
                if (reply.isError()) {
                    qWarning() << "Failed to activate background apps via dbus:" << reply.error().message();
                    return;
                }
                
                w->deleteLater();
                dbusInterface->deleteLater();
            });        
    }
}

void BackgroundAppsModel::quitApp(const QString &instance) {
    for (auto &app: m_apps) {
        if (app.instance != instance) {
            continue;
        }

        const QString dbugService = app.appId;
        const QString dbusPath = QStringLiteral("/") + app.appId.replace(QLatin1Char('.'), QLatin1Char('/')).replace(QLatin1Char('-'), QLatin1Char('_'));
        auto dbusInterface = new QDBusInterface(dbugService, dbusPath, DBUS_INTERFACE_FDO_APPLICATION, QDBusConnection::sessionBus(), this);
        QDBusPendingCall pcall = dbusInterface->asyncCall(QLatin1String("ActivateAction"), QLatin1String("quit"), QList<QVariant>(), QVariantMap());
        auto watcher = new QDBusPendingCallWatcher(pcall, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [dbusInterface](QDBusPendingCallWatcher *w) {
                QDBusPendingReply reply(*w);
                if (reply.isError()) {
                    qWarning() << "Failed to kill background apps via dbus:" << reply.error().message();
                    return;
                }
                
                w->deleteLater();
                dbusInterface->deleteLater();
            });        
    }
}

#include "moc_backgroundappsmodel.cpp"
