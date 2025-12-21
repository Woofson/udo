#ifndef DOBAR_H
#define DOBAR_H

#include <QWidget>

class QLineEdit;
class QPushButton;

class DoBar : public QWidget
{
    Q_OBJECT

public:
    explicit DoBar(QWidget *parent = nullptr);
    void focusLineEdit();

signals:
    void commandEntered(const QString& command);

private:
    QLineEdit *m_lineEdit;
    QPushButton *m_doButton;
};

#endif // DOBAR_H
