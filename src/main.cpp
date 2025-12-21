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
#include <QCoreApplication>
#include <QDir>

// Pre-flight function to set environment variables and custom config path
void preFlightSetup(int argc, char *argv[], QString &outPersistentStoragePath, QUrl &outInitialUrl)
{
    QCoreApplication::setApplicationName("udo");
    QCoreApplication::setApplicationVersion("2.0.2-rc1");

    QString customConfigPath;
    for (int i = 1; i < argc; ++i) {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith("--configdir=")) {
            customConfigPath = arg.mid(12);
        } else if (arg == "-c" && i + 1 < argc) {
            customConfigPath = QString::fromLocal8Bit(argv[++i]);
        } else if (!arg.startsWith("-")) {
            outInitialUrl = QUrl::fromUserInput(arg);
            if (!outInitialUrl.isValid()) {
                outInitialUrl = QUrl();
            }
        }
    }

    if (!customConfigPath.isEmpty()) {
        if (customConfigPath == ".") {
            customConfigPath = QDir::currentPath();
        } else if (customConfigPath.startsWith("~/")) {
            customConfigPath.replace(0, 1, QDir::homePath());
        }
        ConfigManager::setCustomConfigPath(customConfigPath);
    }

    ConfigManager &config = ConfigManager::instance(); 
    outPersistentStoragePath = config.configFilePath().section('/', 0, -2);

    QStringList flags = config.chromiumFlags();
    if (!flags.isEmpty()) {
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", flags.join(" ").toUtf8());
    }
}

QString generateStyleSheet()
{
    const ConfigManager &cfg = ConfigManager::instance();
    
    // Global
    QString globalFont = cfg.getThemeValue("global", "font_family", "DejaVu Sans");
    int globalFontSize = cfg.getThemeInt("global", "font_size", 14);
    int globalBorderSize = cfg.getThemeInt("global", "border_size", 1);
    QString globalBorderColor = cfg.getThemeValue("global", "border_color", "#eaa31f");
    QString globalBg = cfg.getThemeValue("global", "background", "#1a1d21cc");
    QString globalText = cfg.getThemeValue("global", "text", "#ffc30b");
    
    // Statusbar
    QString sbBg = cfg.getThemeValue("statusbar", "background", "#2c2f33cc");
    QString sbText = cfg.getThemeValue("statusbar", "text", "#eaa31f");
    int sbBorderSize = cfg.getThemeInt("statusbar", "border_size", globalBorderSize);
    QString sbBorderColor = cfg.getThemeValue("statusbar", "border_color", globalBorderColor);
    
    // Tabbar
    QString tbBg = cfg.getThemeValue("tabbar", "background", "#2c2f33cc");
    int tbMainBorderSize = cfg.getThemeInt("tabbar", "border_size", globalBorderSize);
    QString tbMainBorderColor = cfg.getThemeValue("tabbar", "border_color", globalBorderColor);
    
    // Active Tab
    QString tbActBg = cfg.getThemeSubValue("tabbar", "active", "background", "#eaa31fcc");
    QString tbActText = cfg.getThemeSubValue("tabbar", "active", "text", "#1a1d21");
    QString tbActBT = cfg.getThemeSubValue("tabbar", "active", "border_top", "#ffc30b");
    QString tbActBB = cfg.getThemeSubValue("tabbar", "active", "border_bottom", "#eaa31f");
    QString tbActBL = cfg.getThemeSubValue("tabbar", "active", "border_left", "#eaa31f");
    QString tbActBR = cfg.getThemeSubValue("tabbar", "active", "border_right", "#eaa31f");

    // Inactive Tab
    QString tbInBg = cfg.getThemeSubValue("tabbar", "inactive", "background", "transparent");
    QString tbInText = cfg.getThemeSubValue("tabbar", "inactive", "text", "#eaa31f");
    QString tbInBT = cfg.getThemeSubValue("tabbar", "inactive", "border_top", "transparent");
    QString tbInBB = cfg.getThemeSubValue("tabbar", "inactive", "border_bottom", "transparent");
    QString tbInBL = cfg.getThemeSubValue("tabbar", "inactive", "border_left", "transparent");
    QString tbInBR = cfg.getThemeSubValue("tabbar", "inactive", "border_right", "#eaa31f");
    
    // DoBar
    QString dbOuterBg = cfg.getThemeValue("do_bar", "outer_background", "#2c2f33cc");
    QString dbOuterBorderColor = cfg.getThemeValue("do_bar", "outer_border", "#eaa31f");
    int dbOuterBorderSize = cfg.getThemeInt("do_bar", "outer_border_size", globalBorderSize);
    
    QString dbInputBg = cfg.getThemeValue("do_bar", "input_background", "#1a1d21");
    QString dbInputBorder = cfg.getThemeValue("do_bar", "input_border", "#eaa31f");
    QString dbInputText = cfg.getThemeValue("do_bar", "input_text", "#ffc30b");
    
    QString dbBtnBg = cfg.getThemeValue("do_bar", "button_background", "#eaa31f");
    QString dbBtnText = cfg.getThemeValue("do_bar", "button_text", "#1a1d21");
    
    // Menu
    QString menuBg = cfg.getThemeValue("menu", "background", "#2c2f33ee");
    QString menuBorder = cfg.getThemeValue("menu", "border", "#eaa31f");
    QString menuText = cfg.getThemeValue("menu", "text", "#ffc30b");
    QString menuSelBg = cfg.getThemeValue("menu", "selected_background", "#eaa31f");
    QString menuSelText = cfg.getThemeValue("menu", "selected_text", "#1a1d21");

    // Dialogs
    int diagBorderSize = cfg.getThemeInt("dialogs", "border_size", 2);
    QString diagBorderColor = cfg.getThemeValue("dialogs", "border_color", "#ffc30b");

    QString qss = R"(
        * {
            font-family: "@GLOBAL_FONT@";
            font-size: @GLOBAL_FONT_SIZE@px;
            color: @GLOBAL_TEXT_COLOR@;
        }
        QMainWindow {
            background-color: transparent;
            border: none;
        }
        QTabWidget {
            border: none;
            background-color: transparent;
        }
        QTabWidget::pane {
            border: @GLOBAL_BORDER_SIZE@px solid @GLOBAL_BORDER_COLOR@;
            background-color: @GLOBAL_BG_COLOR@;
        }
        QStatusBar {
            background-color: @SB_BG_COLOR@;
            color: @SB_TEXT_COLOR@;
            border-top: @SB_BORDER_SIZE@px solid @SB_BORDER_COLOR@;
            border-bottom: none; border-left: none; border-right: none;
        }
        QStatusBar QLabel {
            color: @SB_TEXT_COLOR@;
        }
        QTabBar {
            background-color: @TB_BG_COLOR@;
            border: @TB_MAIN_BORDER_SIZE@px solid @TB_MAIN_BORDER_COLOR@;
            qproperty-drawBase: 0;
        }
        QTabBar::tab {
            background-color: @TB_IN_BG@;
            color: @TB_IN_TEXT@;
            padding: 5px 15px;
            border-top: 1px solid @TB_IN_BT@;
            border-bottom: 1px solid @TB_IN_BB@;
            border-left: 1px solid @TB_IN_BL@;
            border-right: 1px solid @TB_IN_BR@;
        }
        QTabBar::tab:selected {
            background-color: @TB_ACT_BG@;
            color: @TB_ACT_TEXT@;
            border-top: 1px solid @TB_ACT_BT@;
            border-bottom: 1px solid @TB_ACT_BB@;
            border-left: 1px solid @TB_ACT_BL@;
            border-right: 1px solid @TB_ACT_BR@;
        }
        QTabBar::close-button {
            subcontrol-origin: padding;
            subcontrol-position: right;
        }

        #doBarFrame {
            background-color: @DB_OUTER_BG@;
            border: @DB_OUTER_BORDER_SIZE@px solid @DB_OUTER_BORDER_COLOR@;
            border-radius: 5px;
        }
        DoBar { background: transparent; }
        DoBar QLineEdit {
            background-color: @DB_INPUT_BG@;
            color: @DB_INPUT_TEXT@ !important;
            border: 1px solid @DB_INPUT_BORDER@ !important;
            padding: 5px;
            border-radius: 3px;
        }
        DoBar QPushButton {
            background-color: @DB_BTN_BG@ !important;
            color: @DB_BTN_TEXT@ !important;
            border: none;
            padding: 5px 15px;
            border-radius: 3px;
        }
        QMenu {
            background-color: @MENU_BG@;
            border: 1px solid @MENU_BORDER@;
            color: @MENU_TEXT@;
        }
        QMenu::item {
            padding: 5px 20px;
            background-color: transparent;
            color: @MENU_TEXT@;
        }
        QMenu::item:selected {
            background-color: @MENU_SEL_BG@ !important;
            color: @MENU_SEL_TEXT@ !important;
        }
        QMenu::separator {
            height: 1px;
            background: @MENU_BORDER@;
            margin: 5px 10px;
        }
        QDialog {
            background-color: transparent;
            border: @DIAG_BORDER_SIZE@px solid @DIAG_BORDER_COLOR@;
        }
    )";

    qss.replace("@GLOBAL_FONT@", globalFont);
    qss.replace("@GLOBAL_FONT_SIZE@", QString::number(globalFontSize));
    qss.replace("@GLOBAL_TEXT_COLOR@", globalText);
    qss.replace("@GLOBAL_BG_COLOR@", globalBg);
    qss.replace("@GLOBAL_BORDER_SIZE@", QString::number(globalBorderSize));
    qss.replace("@GLOBAL_BORDER_COLOR@", globalBorderColor);
    
    qss.replace("@SB_BG_COLOR@", sbBg);
    qss.replace("@SB_TEXT_COLOR@", sbText);
    qss.replace("@SB_BORDER_SIZE@", QString::number(sbBorderSize));
    qss.replace("@SB_BORDER_COLOR@", sbBorderColor);
    
    qss.replace("@TB_BG_COLOR@", tbBg);
    qss.replace("@TB_MAIN_BORDER_SIZE@", QString::number(tbMainBorderSize));
    qss.replace("@TB_MAIN_BORDER_COLOR@", tbMainBorderColor);
    
    qss.replace("@TB_ACT_BG@", tbActBg);
    qss.replace("@TB_ACT_TEXT@", tbActText);
    qss.replace("@TB_ACT_BT@", tbActBT);
    qss.replace("@TB_ACT_BB@", tbActBB);
    qss.replace("@TB_ACT_BL@", tbActBL);
    qss.replace("@TB_ACT_BR@", tbActBR);

    qss.replace("@TB_IN_BG@", tbInBg);
    qss.replace("@TB_IN_TEXT@", tbInText);
    qss.replace("@TB_IN_BT@", tbInBT);
    qss.replace("@TB_IN_BB@", tbInBB);
    qss.replace("@TB_IN_BL@", tbInBL);
    qss.replace("@TB_IN_BR@", tbInBR);

    qss.replace("@DB_OUTER_BG@", dbOuterBg);
    qss.replace("@DB_OUTER_BORDER_COLOR@", dbOuterBorderColor);
    qss.replace("@DB_OUTER_BORDER_SIZE@", QString::number(dbOuterBorderSize));
    qss.replace("@DB_INPUT_BG@", dbInputBg);
    qss.replace("@DB_INPUT_BORDER@", dbInputBorder);
    qss.replace("@DB_INPUT_TEXT@", dbInputText);
    qss.replace("@DB_BTN_BG@", dbBtnBg);
    qss.replace("@DB_BTN_TEXT@", dbBtnText);
    
    qss.replace("@MENU_BG@", menuBg);
    qss.replace("@MENU_BORDER@", menuBorder);
    qss.replace("@MENU_TEXT@", menuText);
    qss.replace("@MENU_SEL_BG@", menuSelBg);
    qss.replace("@MENU_SEL_TEXT@", menuSelText);
    
    qss.replace("@DIAG_BORDER_SIZE@", QString::number(diagBorderSize));
    qss.replace("@DIAG_BORDER_COLOR@", diagBorderColor);

    return qss;
}

int main(int argc, char *argv[])
{
    QString persistentStoragePath;
    QUrl initialUrl;
    
    QCoreApplication::setApplicationName("udo");
    QCoreApplication::setApplicationVersion("2.0.2-rc1");

    preFlightSetup(argc, argv, persistentStoragePath, initialUrl);

    QApplication app(argc, argv);

    QWebEngineProfile *profile = new QWebEngineProfile("udoProfile", &app);
    profile->setPersistentStoragePath(persistentStoragePath);
    profile->setCachePath(persistentStoragePath + "/cache");

    app.setStyleSheet(generateStyleSheet());
    
    MainWindow mainWindow(profile, initialUrl);
    mainWindow.resize(1280, 720);
    mainWindow.show();

    return app.exec();
}