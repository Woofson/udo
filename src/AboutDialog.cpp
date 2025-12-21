#include "AboutDialog.h"
#include "ConfigManager.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QSvgWidget>
#include <QFile>
#include <QPixmap>
#include <QDir>
#include <QTextEdit>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("About µDo!");
    setFixedSize(500, 550); // Increased size for release log

    QVBoxLayout *layout = new QVBoxLayout(this);

    // --- Icon ---
    QWidget *iconWidget = nullptr;
    QStringList searchPaths;
    searchPaths << QCoreApplication::applicationDirPath() + "/assets/"
                << QDir::currentPath() + "/assets/"
                << QDir::currentPath() + "/../assets/";

    QString foundPath;
    for (const QString &path : searchPaths) {
        if (QFile::exists(path + "icon.svg") || QFile::exists(path + "icon.png")) {
            foundPath = path;
            break;
        }
    }

    if (!foundPath.isEmpty()) {
        if (QFile::exists(foundPath + "icon.svg")) {
            QSvgWidget *svg = new QSvgWidget(foundPath + "icon.svg", this);
            svg->setFixedSize(96, 96);
            iconWidget = svg;
        } else {
            QLabel *png = new QLabel(this);
            png->setPixmap(QPixmap(foundPath + "icon.png").scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            iconWidget = png;
        }
    }

    if (iconWidget) {
        layout->addWidget(iconWidget, 0, Qt::AlignCenter);
    }

    QLabel *appName = new QLabel("<b>µDo!</b>", this);
    appName->setAlignment(Qt::AlignCenter);
    QLabel *version = new QLabel(QString("Version %1").arg(QApplication::applicationVersion()), this);
    version->setAlignment(Qt::AlignCenter);
    
    const ConfigManager &cfg = ConfigManager::instance();
    QString linkColor = cfg.getThemeValue("dialogs", "link", "#ffffff");
    
    QLabel *author = new QLabel(QString("By Tintin (<a style='color: %1;' href=\"https://github.com/Woofson/udo\">github.com/Woofson/udo</a>)").arg(linkColor), this);
    author->setAlignment(Qt::AlignCenter);
    author->setTextFormat(Qt::RichText);
    author->setTextInteractionFlags(Qt::TextBrowserInteraction);
    author->setOpenExternalLinks(true);

    layout->addWidget(appName);
    layout->addWidget(version);
    layout->addWidget(author);

    // --- Release Log ---
    QLabel *logLabel = new QLabel("<b>Release Notes:</b>", this);
    layout->addWidget(logLabel);

    QTextEdit *releaseLog = new QTextEdit(this);
    releaseLog->setReadOnly(true);
    QString logText = R"(
        <h3>v2.0.2</h3>
        <ul>
            <li><b>Rebranding:</b> Application renamed to µDo!</li>
            <li><b>Portability:</b> Binary is now more standalone; assets and default config embedded as resources.</li>
            <li><b>Modular Theme:</b> Fully customizable colors, fonts, borders, and transparency via config.json.</li>
            <li><b>Fixed Persistent Storage:</b> Cookies and login sessions are now reliably saved.</li>
            <li><b>Configurable Chromium Flags:</b> Control engine-level features directly from the config file.</li>
            <li><b>Standalone Tab Bar:</b> Highly configurable tab bar, styled to match the status bar.</li>
            <li><b>Downloads:</b> Added "Save As..." dialog and configurable download path.</li>
            <li><b>HTTP Auth:</b> Improved authentication dialog with error feedback and special character support.</li>
            <li><b>Prioritize HTTPS:</b> New setting to automatically attempt HTTPS connections.</li>
            <li><b>Local Content:</b> Support for loading local files, PDFs, and images.</li>
            <li><b>Command Line:</b> Launch directly to a URL or specify a custom config directory.</li>
            <li><b>Keyboard Shortcuts:</b> Added standard refresh shortcuts (F5, Ctrl+R) and Escape to hide bar.</li>
        </ul>
    )";
    releaseLog->setHtml(logText);
    layout->addWidget(releaseLog);

    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &AboutDialog::accept);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);

    // Apply modular theme
    QString bg = cfg.getThemeValue("dialogs", "background", "#333333ee");
    QString text = cfg.getThemeValue("dialogs", "text", "#ffc30b");
    QString btnBg = cfg.getThemeValue("dialogs", "button_background", "#eaa31f");
    QString btnText = cfg.getThemeValue("dialogs", "button_text", "#1a1d21");
    int borderSize = cfg.getThemeInt("dialogs", "border_size", 2);
    QString borderColor = cfg.getThemeValue("dialogs", "border_color", "#ffc30b");

    setStyleSheet(QString(R"(
        AboutDialog {
            background-color: %1;
            border: %5px solid %6;
        }
        QLabel {
            color: %2;
        }
        QTextEdit {
            background-color: rgba(0, 0, 0, 0.2);
            color: %2;
            border: 1px solid %6;
        }
        QPushButton {
            background-color: %3;
            color: %4;
            border: none;
            padding: 5px 15px;
            border-radius: 3px;
        }
    )").arg(bg, text, btnBg, btnText).arg(borderSize).arg(borderColor));
}
