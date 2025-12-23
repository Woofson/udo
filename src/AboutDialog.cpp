#include "AboutDialog.h"
#include "ConfigManager.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QSvgWidget>
#include <QFile>
#include <QPixmap>
#include <QTextEdit>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("About µDo!");
    setFixedSize(500, 550); // Increased size for release log

    QVBoxLayout *layout = new QVBoxLayout(this);

    // --- Icon from Resource ---
    QWidget *iconWidget = nullptr;
    
    if (QFile::exists(":/assets/assets/icon.svg")) {
        QSvgWidget *svg = new QSvgWidget(":/assets/assets/icon.svg", this);
        svg->setFixedSize(96, 96);
        iconWidget = svg;
    } else if (QFile::exists(":/assets/assets/icon.png")) {
        QLabel *png = new QLabel(this);
        png->setPixmap(QPixmap(":/assets/assets/icon.png").scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        iconWidget = png;
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
        <h3>v2.0.4</h3>
        <ul>
            <li><b>Notifications:</b> Added Desktop Notification support via D-Bus.</li>
            <li><b>Downloads:</b> Get notified when downloads complete or fail (works with mako/dunst).</li>
        </ul>
        <h3>v2.0.3</h3>
        <ul>
            <li><b>Theme Refinement:</b> Fixed Do!-bar borders, button text, and menu highlighting.</li>
            <li><b>Tab Bar Fix:</b> Eliminated the white bar at the bottom of the tab bar.</li>
            <li><b>Standalone Binary:</b> Icons and default config are now fully embedded as resources.</li>
            <li><b>Portability:</b> Improved asset loading for better standalone performance.</li>
        </ul>
        <h3>v2.0.2</h3>
        <ul>
            <li><b>Rebranding:</b> Application renamed to µDo!.</li>
            <li><b>Modular Theme:</b> Fully customizable via config.json.</li>
            <li><b>Standalone Tab Bar:</b> Highly configurable bar, styled to match status bar.</li>
            <li><b>Downloads:</b> Added "Save As..." dialog and configurable path.</li>
            <li><b>HTTP Auth:</b> Improved dialog with error feedback and special character support.</li>
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
