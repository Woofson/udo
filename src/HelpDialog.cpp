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
    setMinimumSize(500, 400);

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

    // Config File Location
    helpText += "<h2>Configuration</h2>";
    helpText += "<p>Your configuration file is located at:<br><b>" + ConfigManager::instance().configFilePath() + "</b></p>";
    helpText += "<p>Your browser data (cache, cookies, history) is also stored in this directory.</p>";


    // Commands
    helpText += "<h2>!!! Commands</h2>";
    helpText += "<ul>";
    QJsonObject tripleBangs = ConfigManager::instance().tripleBangs();
    for(auto it = tripleBangs.begin(); it != tripleBangs.end(); ++it) {
        helpText += QString("<li><b>!!!%1</b>: %2</li>").arg(it.key(), it.value().toString());
    }
    helpText += "</ul>";

    // Bookmarks
    helpText += "<h2>!! Bookmarks</h2>";
    helpText += "<ul>";
    QJsonObject doubleBangs = ConfigManager::instance().doubleBangs();
    for(auto it = doubleBangs.begin(); it != doubleBangs.end(); ++it) {
        helpText += QString("<li><b>!!%1</b>: %2</li>").arg(it.key(), it.value().toString());
    }
    helpText += "</ul>";

    // Searches
    helpText += "<h2>! Searches</h2>";
    helpText += "<ul>";
    QJsonObject singleBangs = ConfigManager::instance().bangs();
    for(auto it = singleBangs.begin(); it != singleBangs.end(); ++it) {
        helpText += QString("<li><b>!%1</b>: %2</li>").arg(it.key(), it.value().toString().replace("%s", "[query]"));
    }
    helpText += "</ul>";

    // Shortcuts
    helpText += "<h2>Shortcuts</h2>";
    helpText += "<ul>";
    QJsonObject shortcuts = ConfigManager::instance().shortcuts();
    for(auto it = shortcuts.begin(); it != shortcuts.end(); ++it) {
        helpText += QString("<li><b>%1</b>: %2</li>").arg(it.value().toString(), it.key());
    }
    helpText += "</ul>";

    textEdit->setHtml(helpText);

    // Apply some styling
    setStyleSheet(R"(
        HelpDialog {
            background-color: #2C2F33;
        }
        QTextEdit {
            background-color: #1A1D21;
            color: #FFC30B;
            border: 1px solid #EAA31F;
            font-size: 14px;
        }
        QPushButton {
            background-color: #EAA31F;
            color: #1A1D21;
            border: 1px solid #B86B00;
            padding: 5px 15px;
            border-radius: 3px;
            font-size: 14px;
        }
    )");
}
