#pragma once

#include <QLineEdit>

#include <Components/Encryption/Common.h>

namespace QtCustom::Etc {

class SecurePasswordEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit SecurePasswordEdit(QWidget* parent = nullptr);

    void setPassword(const Encryption::SafeString& pass);
    const Encryption::SafeString& getPassword() const;

    void clearPassword();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void updateDisplay();

    Encryption::SafeString m_password;
};

}