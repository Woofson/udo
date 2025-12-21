#include <QApplication>
#include "MainWindow.h"
#include "ConfigManager.h"
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QCoreApplication> // For QCoreApplication

// Pre-flight function to set environment variables and custom config path
void preFlightSetup(int argc, char *argv[], QString &outPersistentStoragePath, QUrl &outInitialUrl)
{
    // Set app details first, so QStandardPaths can find the correct path
    QApplication::setApplicationName("udo");
    QApplication::setApplicationVersion("2.0.1-rc1");

    // Need a temporary QCoreApplication for QStandardPaths to work
    QCoreApplication preflightApp(argc, argv);
    
    // Manual command-line argument parsing for --configdir and initial URL
    QString customConfigPath;
    for (int i = 1; i < argc; ++i) {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg == "--configdir" || arg == "-c") {
            if (i + 1 < argc) {
                customConfigPath = QString::fromLocal8Bit(argv[++i]);
            }
        } else if (!arg.startsWith("-")) {
            outInitialUrl = QUrl::fromUserInput(arg);
            if (!outInitialUrl.isValid()) {
                qWarning() << "Invalid URL provided in command line argument:" << arg;
                outInitialUrl = QUrl();
            }
        }
    }

    if (!customConfigPath.isEmpty()) {
        ConfigManager::setCustomConfigPath(customConfigPath);
        qDebug() << "Custom config path set to:" << customConfigPath;
    }

    // Now ConfigManager is safe to use and will honor customConfigPath
    ConfigManager &config = ConfigManager::instance(); // Access instance
    outPersistentStoragePath = config.configFilePath().section('/', 0, -2);
    qDebug() << "Pre-flight: Persistent storage path resolved to:" << outPersistentStoragePath;

    // Load chromium flags and set environment variables
    QStringList flags = config.chromiumFlags();
    if (!flags.isEmpty()) {
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", flags.join(" ").toUtf8());
        qDebug() << "Pre-flight: Setting QTWEBENGINE_CHROMIUM_FLAGS to:" << flags.join(" ");
    }
}


int main(int argc, char *argv[])
{
    QString persistentStoragePath;
    QUrl initialUrl;
    preFlightSetup(argc, argv, persistentStoragePath, initialUrl); // Call preflight

    QApplication app(argc, argv); // Real app

    // Set storage path for WebEngine
    qDebug() << "Main: Persistent storage path for profile:" << persistentStoragePath;
    QWebEngineProfile *profile = new QWebEngineProfile("udoProfile", &app);
    profile->setPersistentStoragePath(persistentStoragePath);

    // Apply QSS theme
    QString styleSheet = R"(
        QMainWindow {
            background-color: #1A1D21; /* Very Dark Gray */
            color: #FFC30B; /* Bright Yellow */
        }
        QTabWidget::pane { /* The tab widget frame */
            border: 1px solid #2C2F33;
            background-color: #1A1D21;
        }
        QTabWidget::tab-bar {
            left: 5px; /* move to the right by 5px */
        }
        QTabBar::tab {
            background: #2C2F33; /* Dark Gray */
            border: 1px solid #2C2F33;
            border-bottom-color: #1A1D21; /* Same as pane color */
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            min-width: 8ex;
            padding: 2px;
            color: #FFC30B;
        }
        QTabBar::tab:selected, QTabBar::tab:hover {
            background: #EAA31F; /* Darker Yellow */
            color: #1A1D21; /* Very Dark Gray */
        }
        QTabBar::tab:selected {
            border-color: #EAA31F;
            border-bottom-color: #EAA31F; /* Same as selected tab color */
        }
        QTabBar::tab:!selected {
            margin-top: 2px; /* make non-selected tabs look smaller */
        }
        QWebEngineView {
            border: none;
        }
        #doBarFrame {
            background-color: #2c2f33;
            border: 1px solid #EAA31F;
            border-radius: 5px;
        }
        DoBar {
            background: transparent;
        }
        QLineEdit {
            background-color: #2C2F33;
            color: #FFC30B;
            border: 1px solid #EAA31F;
            padding: 5px;
            border-radius: 3px;
        }
        QPushButton {
            background-color: #EAA31F;
            color: #1A1D21;
            border: 1px solid #B86B00;
            padding: 5px 10px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: #FFC30B;
        }
        QPushButton:pressed {
            background-color: #B86B00;
        }
        QStatusBar {
            background-color: #2c2f33;
            color: #eaa31f;
            border-top: 1px solid #EAA31F;
        }
        QStatusBar QLabel {
            color: #eaa31f;
        }
        QMenu {
            background-color: #2c2f33;
            border: 1px solid #EAA31F;
            color: #FFC30B;
        }
        QMenu::item {
            padding: 5px 20px 5px 20px;
        }
        QMenu::item:selected {
            background-color: #EAA31F;
            color: #1A1D21;
        }
        QMenu::separator {
            height: 1px;
            background: #B86B00;
            margin-left: 10px;
            margin-right: 10px;
        }
    )";
    app.setStyleSheet(styleSheet);
    
    MainWindow mainWindow(profile, initialUrl);
    mainWindow.resize(1280, 720);
    mainWindow.show();

    return app.exec();
}