#include "securepasswordedit.hpp"

#include <QKeyEvent>

namespace QtCustom::Etc {

SecurePasswordEdit::SecurePasswordEdit(QWidget *parent)
    : QLineEdit(parent) {
    setEchoMode(QLineEdit::Normal);
    setMaxLength(256);
}

void SecurePasswordEdit::setPassword(const Encryption::SafeString &pass)
{
    m_password = pass;
}

const Encryption::SafeString &SecurePasswordEdit::getPassword() const {
    return m_password;
}

void SecurePasswordEdit::clearPassword() {
    m_password.secureClear();
    QLineEdit::clear();
}

void SecurePasswordEdit::keyPressEvent(QKeyEvent *event) {
    const int key = event->key();

    if (key == Qt::Key_Backspace) {
        auto selectionBegin = this->selectionStart();
        auto selectedLength = this->selectionLength();
        if ((selectedLength > 0) && ((selectionBegin + selectedLength) < m_password->size())) {
            m_password->erase(selectionBegin, selectionBegin + selectedLength);
        } else if (m_password->size() > 0) {
            m_password->pop_back();
        }
        updateDisplay();
        event->accept();
        return;
    }

    if (key == Qt::Key_Return || key == Qt::Key_Enter) {
        QLineEdit::keyPressEvent(event);
        return;
    }

    if (event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) {
        QLineEdit::keyPressEvent(event);
        return;
    }

    const QString text = event->text();
    if (!text.isEmpty() && text.at(0).isPrint()) {
        m_password->append(text.toStdString());
        updateDisplay();
        event->accept();
        return;
    }

    QLineEdit::keyPressEvent(event);
}

void SecurePasswordEdit::updateDisplay() {
    const int len = static_cast<int>(m_password->size());
    QString masked(len, QChar('*'));
    QLineEdit::setText(masked);
    setCursorPosition(len);
}

}