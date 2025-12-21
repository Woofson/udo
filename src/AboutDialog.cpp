#include "AboutDialog.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("About µDo!");
    setFixedSize(300, 200);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *appName = new QLabel("<b>µDo!</b>", this);
    QLabel *version = new QLabel(QString("Version %1").arg(QApplication::applicationVersion()), this);
    QLabel *author = new QLabel("By Tintin (<a href=\"https://github.com/Woofson/udo\">github.com/Woofson/udo</a>)", this);
    author->setTextFormat(Qt::RichText);
    author->setTextInteractionFlags(Qt::TextBrowserInteraction);
    author->setOpenExternalLinks(true);

    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &AboutDialog::accept);

    layout->addWidget(appName, 0, Qt::AlignCenter);
    layout->addWidget(version, 0, Qt::AlignCenter);
    layout->addWidget(author, 0, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(closeButton, 0, Qt::AlignCenter);

    // Apply some styling
    setStyleSheet(R"(
        AboutDialog {
            background-color: #2C2F33; /* Dark Gray */
            color: #FFC30B; /* Bright Yellow */
        }
        QLabel {
            color: #FFC30B;
            font-size: 14px;
        }
        QLabel a {
            color: #004E64; /* Dark Teal for link */
        }
        QPushButton {
            background-color: #EAA31F; /* Darker Yellow */
            color: #1A1D21; /* Very Dark Gray */
            border: 1px solid #B86B00;
            padding: 5px 15px;
            border-radius: 3px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #FFC30B;
        }
        QPushButton:pressed {
            background-color: #B86B00;
        }
    )");
}
