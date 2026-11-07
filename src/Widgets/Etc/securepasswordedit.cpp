#include "securepasswordedit.hpp"

#include <QFocusEvent>
#include <QKeyEvent>

namespace QtCustom::Etc {

SecurePasswordEdit::SecurePasswordEdit(QWidget *parent)
    : QLineEdit(parent) {
    setEchoMode(QLineEdit::Normal);
    setMaxLength(256);

    m_hideTimer.setSingleShot(true);
    connect(&m_hideTimer, &QTimer::timeout, this, &SecurePasswordEdit::hidePassword);
}

void SecurePasswordEdit::setPassword(const Encryption::SafeString &pass)
{
    hidePassword();
    m_password = pass;
    updateDisplay();
}

const Encryption::SafeString &SecurePasswordEdit::getPassword() const {
    return m_password;
}

void SecurePasswordEdit::clearPassword() {
    m_hideTimer.stop();
    m_isRevealed = false;
    m_password.secureClear();
    QLineEdit::clear();
}

void SecurePasswordEdit::revealPassword(int msec) {
    if (m_password->empty()) {
        return;
    }

    m_isRevealed = true;
    updateDisplay();
    m_hideTimer.start(msec);
}

void SecurePasswordEdit::hidePassword() {
    m_hideTimer.stop();
    if (!m_isRevealed) {
        return;
    }
    m_isRevealed = false;
    updateDisplay();
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

void SecurePasswordEdit::focusOutEvent(QFocusEvent *event) {
    hidePassword();
    QLineEdit::focusOutEvent(event);
}

void SecurePasswordEdit::updateDisplay() {
    const int len = static_cast<int>(m_password->size());
    QString text = m_isRevealed
        ? QString::fromUtf8(m_password->data(), len)
        : QString(len, QChar('*'));

    QLineEdit::setText(text);
    setCursorPosition(len);

    if (m_isRevealed) {
        text.fill(QChar('\0'));
    }
}

}