#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QJsonObject>
#include <QVariantMap>
#include <QStandardPaths>
#include <QDir>
#include <QStringList>

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
    bool tabBarVisibleByDefault() const;
    QString tabBarPosition() const;
    QString clockFormat() const;
    QString downloadsPath() const;
    bool prioritizeHttps() const;
    QStringList chromiumFlags() const;
    QString configFilePath() const;

    // Modular Theme Getters
    QJsonObject getThemeModule(const QString &module) const;
    QString getThemeValue(const QString &module, const QString &key, const QString &defaultValue = "") const;
    QString getThemeSubValue(const QString &module, const QString &subModule, const QString &key, const QString &defaultValue = "") const;
    int getThemeInt(const QString &module, const QString &key, int defaultValue = 0) const;

    static void setCustomConfigPath(const QString &path);

private:
    ConfigManager();
    ~ConfigManager();

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    void load();
    void ensureConfigDirectoryExists() const;
    void copyDefaultConfig() const;

    QJsonObject m_config;
    static QString s_customConfigPath;
};

#endif // CONFIGMANAGER_H