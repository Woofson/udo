#ifndef HTTPAUTHDIALOG_H
#define HTTPAUTHDIALOG_H

#include <QDialog>

class QLineEdit;

class HttpAuthDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HttpAuthDialog(QWidget *parent, const QString &realm, const QString &errorMessage = QString());
    QString username() const;
    QString password() const;

private:
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
};

#endif // HTTPAUTHDIALOG_H
