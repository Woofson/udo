#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QJsonObject>
#include <QVariantMap>
#include <QStandardPaths> // For standard path locations
#include <QDir>           // For directory operations

class ConfigManager
{
public:
    static ConfigManager& instance();

    QString startPage() const;
    QString defaultSearchEngine() const;
    QString websiteColorScheme() const;
    QJsonObject bangs() const;
    QJsonObject doubleBangs() const;
    QJsonObject tripleBangs() const;
    QJsonObject shortcuts() const;
    bool statusBarVisibleByDefault() const;
    QString clockFormat() const;
    QString downloadsPath() const;
    bool prioritizeHttps() const;
    QStringList chromiumFlags() const; // Re-add chromiumFlags getter
    QString configFilePath() const;

    static void setCustomConfigPath(const QString &path); // Re-add static setter

private:
    ConfigManager();
    ~ConfigManager();

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    void load();
    void ensureConfigDirectoryExists() const;
    void copyDefaultConfig() const;

    QJsonObject m_config;
    static QString s_customConfigPath; // Re-add static member
};

#endif // CONFIGMANAGER_H
