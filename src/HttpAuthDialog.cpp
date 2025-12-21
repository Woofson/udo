#include "HttpAuthDialog.h"
#include "ConfigManager.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFont>

HttpAuthDialog::HttpAuthDialog(QWidget *parent, const QString &realm, const QString &errorMessage)
    : QDialog(parent)
{
    setWindowTitle("Authentication Required");

    QVBoxLayout *layout = new QVBoxLayout(this);
    
    if (!errorMessage.isEmpty()) {
        QLabel *errorLabel = new QLabel(errorMessage, this);
        errorLabel->setObjectName("errorLabel");
        layout->addWidget(errorLabel);
    }

    QLabel *realmLabel = new QLabel(QString("Enter credentials for <b>%1</b>:").arg(realm), this);
    realmLabel->setWordWrap(true);
    
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("Username");

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &HttpAuthDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &HttpAuthDialog::reject);

    layout->addWidget(realmLabel);
    layout->addWidget(m_usernameEdit);
    layout->addWidget(m_passwordEdit);
    layout->addWidget(buttonBox);

    // Apply modular theme
    const ConfigManager &cfg = ConfigManager::instance();
    QString bg = cfg.getThemeValue("dialogs", "background", "#2c2f33ee");
    QString text = cfg.getThemeValue("dialogs", "text", "#ffc30b");
    QString btnBg = cfg.getThemeValue("dialogs", "button_background", "#eaa31f");
    QString btnText = cfg.getThemeValue("dialogs", "button_text", "#1a1d21");
    QString errorText = cfg.getThemeValue("dialogs", "error_text", "#d72638");

    setStyleSheet(QString(R"(
        HttpAuthDialog {
            background-color: %1;
        }
        QLabel {
            color: %2;
        }
        #errorLabel {
            color: %5;
            font-weight: bold;
        }
        QLineEdit {
            background-color: rgba(0, 0, 0, 0.2);
            color: %2;
            border: 1px solid %3;
            padding: 5px;
        }
        QPushButton {
            background-color: %3;
            color: %4;
            border: none;
            padding: 5px 15px;
            border-radius: 3px;
        }
    )").arg(bg, text, btnBg, btnText, errorText));
}

QString HttpAuthDialog::username() const
{
    return m_usernameEdit->text();
}

QString HttpAuthDialog::password() const
{
    return m_passwordEdit->text();
}