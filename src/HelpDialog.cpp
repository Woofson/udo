#include "HelpDialog.h"
#include "ConfigManager.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonValue>

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Help");
    setMinimumSize(600, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);

    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &HelpDialog::accept);

    layout->addWidget(textEdit);
    layout->addWidget(closeButton);

    // --- Generate Help Text ---
    QString helpText;
    helpText += "<h1>µDo! Help</h1>";

    helpText += "<h2>Configuration</h2>";
    helpText += "<p>Your configuration file is located at:<br><b>" + ConfigManager::instance().configFilePath() + "</b></p>";
    helpText += "<p>Browser data (cache, cookies, history) is also stored in this directory.</p>";

    helpText += "<h2>!!! Commands</h2><ul>";
    QJsonObject tripleBangs = ConfigManager::instance().tripleBangs();
    for(auto it = tripleBangs.begin(); it != tripleBangs.end(); ++it) {
        helpText += QString("<li><b>!!!%1</b>: %2</li>").arg(it.key(), it.value().toString());
    }
    helpText += "</ul>";

    helpText += "<h2>!! Bookmarks</h2><ul>";
    QJsonObject doubleBangs = ConfigManager::instance().doubleBangs();
    for(auto it = doubleBangs.begin(); it != doubleBangs.end(); ++it) {
        helpText += QString("<li><b>!!%1</b>: %2</li>").arg(it.key(), it.value().toString());
    }
    helpText += "</ul>";

    helpText += "<h2>! Searches</h2><ul>";
    QJsonObject singleBangs = ConfigManager::instance().bangs();
    for(auto it = singleBangs.begin(); it != singleBangs.end(); ++it) {
        helpText += QString("<li><b>!%1</b>: %2</li>").arg(it.key(), it.value().toString().replace("%s", "[query]"));
    }
    helpText += "</ul>";

    helpText += "<h2>Shortcuts</h2><ul>";
    QJsonObject shortcuts = ConfigManager::instance().shortcuts();
    for(auto it = shortcuts.begin(); it != shortcuts.end(); ++it) {
        helpText += QString("<li><b>%1</b>: %2</li>").arg(it.value().toString(), it.key());
    }
    helpText += "</ul>";

    textEdit->setHtml(helpText);

    // Apply modular theme
    const ConfigManager &cfg = ConfigManager::instance();
    QString bg = cfg.getThemeValue("dialogs", "background", "#2c2f33ee");
    QString text = cfg.getThemeValue("dialogs", "text", "#ffc30b");
    QString btnBg = cfg.getThemeValue("dialogs", "button_background", "#eaa31f");
    QString btnText = cfg.getThemeValue("dialogs", "button_text", "#1a1d21");

    setStyleSheet(QString(R"(
        HelpDialog {
            background-color: %1;
        }
        QTextEdit {
            background-color: rgba(0, 0, 0, 0.3);
            color: %2;
            border: 1px solid %3;
            font-size: 14px;
        }
        QPushButton {
            background-color: %3;
            color: %4;
            border: none;
            padding: 5px 15px;
            border-radius: 3px;
        }
    )").arg(bg, text, btnBg, btnText));
}