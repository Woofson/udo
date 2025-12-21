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

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("About µDo!");
    setFixedSize(400, 350);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // --- Icon ---
    QWidget *iconWidget = nullptr;
    // Check multiple potential locations for assets
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
            svg->setFixedSize(128, 128);
            iconWidget = svg;
        } else {
            QLabel *png = new QLabel(this);
            png->setPixmap(QPixmap(foundPath + "icon.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
    
    // Use configurable link color
    const ConfigManager &cfg = ConfigManager::instance();
    QString linkColor = cfg.getThemeValue("dialogs", "link", "#ffffff");
    
    QLabel *author = new QLabel(QString("By Tintin (<a style='color: %1;' href=\"https://github.com/Woofson/udo\">github.com/Woofson/udo</a>)").arg(linkColor), this);
    author->setAlignment(Qt::AlignCenter);
    author->setTextFormat(Qt::RichText);
    author->setTextInteractionFlags(Qt::TextBrowserInteraction);
    author->setOpenExternalLinks(true);

    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &AboutDialog::accept);

    layout->addWidget(appName);
    layout->addWidget(version);
    layout->addWidget(author);
    layout->addStretch();
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
        QPushButton {
            background-color: %3;
            color: %4;
            border: none;
            padding: 5px 15px;
            border-radius: 3px;
        }
    )").arg(bg, text, btnBg, btnText).arg(borderSize).arg(borderColor));
}