#include "HttpAuthDialog.h"
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
        errorLabel->setStyleSheet("color: #D72638;"); // Bright Red
        layout->addWidget(errorLabel);
    }

    QLabel *realmLabel = new QLabel(QString("Enter credentials for <b>%1</b>:").arg(realm), this);
    realmLabel->setWordWrap(true);
    
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("Username");
    m_usernameEdit->setFont(QFont("DejaVu Sans")); // Set universal font

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setFont(QFont("DejaVu Sans")); // Set universal font

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &HttpAuthDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &HttpAuthDialog::reject);

    layout->addWidget(realmLabel);
    layout->addWidget(m_usernameEdit);
    layout->addWidget(m_passwordEdit);
    layout->addWidget(buttonBox);
}

QString HttpAuthDialog::username() const
{
    return m_usernameEdit->text();
}

QString HttpAuthDialog::password() const
{
    return m_passwordEdit->text();
}
