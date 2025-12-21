#include "ConfigManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication> // For QCoreApplication::applicationDirPath()

QString ConfigManager::s_customConfigPath; // Initialize static member

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
    // Ensure the config directory exists
    ensureConfigDirectoryExists();

    QString appConfigPath = configFilePath();
    QFile configFile(appConfigPath);

    // If config file doesn't exist, copy the default one
    if (!configFile.exists()) {
        qDebug() << "Config file not found at" << appConfigPath << ". Copying default.";
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
    QString configDir;
    if (!s_customConfigPath.isEmpty()) {
        configDir = s_customConfigPath;
    } else {
        configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    }
    QDir dir(configDir);
    if (!dir.exists()) {
        qDebug() << "Config directory does not exist. Attempting to create:" << configDir;
        if (!dir.mkpath(".")) { // Create the directory if it doesn't exist
            qWarning() << "Failed to create config directory:" << configDir;
        } else {
            qDebug() << "Config directory created:" << configDir;
        }
    } else {
        qDebug() << "Config directory already exists:" << configDir;
    }
}

void ConfigManager::copyDefaultConfig() const
{
    QFile defaultConfigFile(":/config/config.json"); // Use embedded resource
    if (!defaultConfigFile.exists()) {
        qWarning() << "Default config.json resource not found. Cannot copy.";
        return;
    }

    QString appConfigPath = configFilePath();
    qDebug() << "Attempting to copy default config from resource to" << appConfigPath;
    if (!defaultConfigFile.copy(appConfigPath)) {
        qWarning() << "Failed to copy default config.json to" << appConfigPath;
    }
    else {
        qDebug() << "Default config.json copied successfully to" << appConfigPath;
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
    return path.replace("~", QDir::homePath());
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