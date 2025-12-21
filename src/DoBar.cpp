#include "DoBar.h"
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

DoBar::DoBar(QWidget *parent)
    : QWidget(parent)
{
    m_lineEdit = new QLineEdit(this);
    m_doButton = new QPushButton("DO!", this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_doButton);
    setLayout(layout);

    // Emit the command when the button is clicked or Enter is pressed
    connect(m_doButton, &QPushButton::clicked, this, [this](){
        emit commandEntered(m_lineEdit->text());
    });

    connect(m_lineEdit, &QLineEdit::returnPressed, this, [this](){
        emit commandEntered(m_lineEdit->text());
    });
}

void DoBar::focusLineEdit()
{
    m_lineEdit->setFocus();
    m_lineEdit->selectAll();
}
