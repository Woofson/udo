#include "ConfigManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

QString ConfigManager::s_customConfigPath;

ConfigManager& ConfigManager::instance()
{
    static ConfigManager inst;
    return inst;
}

ConfigManager::ConfigManager()
{
    load();
}

ConfigManager::~ConfigManager()
{
}

void ConfigManager::load()
{
    ensureConfigDirectoryExists();

    QString appConfigPath = configFilePath();
    QFile configFile(appConfigPath);

    if (!configFile.exists()) {
        copyDefaultConfig();
    }

    if (!configFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open config file at" << appConfigPath;
        return;
    }

    QJsonDocument configDoc = QJsonDocument::fromJson(configFile.readAll());
    configFile.close();

    if (configDoc.isNull()) {
        qWarning() << "Couldn't parse config file at" << appConfigPath;
        return;
    }

    m_config = configDoc.object();
}

QString ConfigManager::configFilePath() const
{
    QString configDir;
    if (!s_customConfigPath.isEmpty()) {
        configDir = s_customConfigPath;
    } else {
        configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    }
    return QDir(configDir).filePath("config.json");
}

void ConfigManager::setCustomConfigPath(const QString &path)
{
    s_customConfigPath = path;
}

void ConfigManager::ensureConfigDirectoryExists() const
{
    QString path = configFilePath();
    QFileInfo fi(path);
    QDir dir = fi.absoluteDir();
    if (!dir.exists()) {
        qDebug() << "Creating config directory:" << dir.absolutePath();
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create directory:" << dir.absolutePath();
        }
    }
}

void ConfigManager::copyDefaultConfig() const
{
    QString resourcePath = ":/config/config.json";
    QFile defaultConfigFile(resourcePath); 
    
    if (!defaultConfigFile.exists()) {
        qWarning() << "Default config resource not found:" << resourcePath;
        // Fallback to local file for development
        defaultConfigFile.setFileName("config.json");
    }

    if (!defaultConfigFile.exists()) {
        qWarning() << "Could not find default config template anywhere.";
        return;
    }

    QString appConfigPath = configFilePath();
    qDebug() << "Copying default config to:" << appConfigPath;
    
    if (defaultConfigFile.copy(appConfigPath)) {
        QFile::setPermissions(appConfigPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser);
    } else {
        qWarning() << "Failed to copy default config:" << defaultConfigFile.errorString();
    }
}

QString ConfigManager::startPage() const
{
    return m_config["start_page"].toString("https://www.google.com");
}

QString ConfigManager::defaultSearchEngine() const
{
    return m_config["default_search_engine"].toString("g");
}

QString ConfigManager::websiteColorScheme() const
{
    return m_config["website_color_scheme"].toString("dark");
}

QJsonObject ConfigManager::bangs() const
{
    return m_config["!bangs"].toObject();
}

QJsonObject ConfigManager::doubleBangs() const
{
    return m_config["!!bangs"].toObject();
}

QJsonObject ConfigManager::tripleBangs() const
{
    return m_config["!!!bangs"].toObject();
}

QJsonObject ConfigManager::shortcuts() const
{
    return m_config["shortcuts"].toObject();
}

bool ConfigManager::statusBarVisibleByDefault() const
{
    return m_config["statusbar_visible_by_default"].toBool(true);
}

bool ConfigManager::tabBarVisibleByDefault() const
{
    return m_config["tabbar_visible_by_default"].toBool(false);
}

QString ConfigManager::tabBarPosition() const
{
    return m_config["tabbar_position"].toString("bottom");
}

QString ConfigManager::clockFormat() const
{
    return m_config["clock_format"].toString("yyyy-MM-dd HH:mm");
}

QString ConfigManager::downloadsPath() const
{
    QString path = m_config["downloads_path"].toString();
    if (path.isEmpty() || path == "~") {
        return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    }
    if (path.startsWith("~/")) {
        path.replace(0, 1, QDir::homePath());
    }
    return path;
}

bool ConfigManager::prioritizeHttps() const
{
    return m_config["prioritize_https"].toBool(true);
}

QStringList ConfigManager::chromiumFlags() const
{
    QStringList flags;
    QJsonArray flagsArray = m_config["chromium_flags"].toArray();
    for (const QJsonValue &value : flagsArray) {
        flags.append(value.toString());
    }
    return flags;
}

QJsonObject ConfigManager::getThemeModule(const QString &module) const
{
    return m_config["theme"].toObject()[module].toObject();
}

QString ConfigManager::getThemeValue(const QString &module, const QString &key, const QString &defaultValue) const
{
    QJsonObject mod = getThemeModule(module);
    if (mod.contains(key)) {
        return mod[key].toString();
    }
    // Fallback to global
    QJsonObject global = getThemeModule("global");
    if (global.contains(key)) {
        return global[key].toString();
    }
    return defaultValue;
}

QString ConfigManager::getThemeSubValue(const QString &module, const QString &subModule, const QString &key, const QString &defaultValue) const
{
    QJsonObject mod = getThemeModule(module);
    if (mod.contains(subModule)) {
        QJsonObject sub = mod[subModule].toObject();
        if (sub.contains(key)) {
            return sub[key].toString();
        }
    }
    return getThemeValue(module, key, defaultValue);
}

int ConfigManager::getThemeInt(const QString &module, const QString &key, int defaultValue) const
{
    QJsonObject mod = getThemeModule(module);
    if (mod.contains(key)) {
        return mod[key].toInt();
    }
    // Fallback to global
    QJsonObject global = getThemeModule("global");
    if (global.contains(key)) {
        return global[key].toInt();
    }
    return defaultValue;
}
