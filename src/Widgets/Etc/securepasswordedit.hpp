#pragma once

#include <QLineEdit>
#include <QTimer>

#include <Components/Encryption/Common.h>

namespace QtCustom::Etc {

class SecurePasswordEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit SecurePasswordEdit(QWidget* parent = nullptr);

    void setPassword(const Encryption::SafeString& pass);
    const Encryption::SafeString& getPassword() const;

    void clearPassword();

public slots:
    /// Shows the actual password for msec milliseconds (default 5 s) and then masks it again.
    /// @note Displaying the password necessarily materializes it in Qt's text buffer,
    ///       so this is a best-effort reveal and the cleartext is wiped from our copy afterwards.
    void revealPassword(int msec = 5000);

    /// Masks the password again and cancels a pending reveal.
    void hidePassword();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    void updateDisplay();

    Encryption::SafeString m_password;
    QTimer m_hideTimer;
    bool m_isRevealed {false};
};

}