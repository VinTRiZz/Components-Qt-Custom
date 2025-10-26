#pragma once

#include <QGraphicsItem>

namespace ObjectItems {

// СОГЛАШЕНИЕ:
// ID НЕ ДОЛЖЕН БЫТЬ ОТРИЦАТЕЛЬНЫМ ДЛЯ ОБЫЧНЫХ ЭЛЕМЕНТОВ
// ДЛЯ СИСТЕМНЫХ ID ОТРИЦАТЕЛЬНЫЙ
using objectId_t = long long;
const objectId_t NULL_OBJECT_ID { 0 };

/**
 * @brief The ObjectDataRole enum Определяет, какие данные могут быть в объекте
 */
enum ObjectDataRole : int {
    OBJECTDATAROLE_ID = Qt::UserRole + 1000,   //! Короткое имя объекта для отображения
    OBJECTDATAROLE_PARENTITEM_ID,  //! Для комплексных объектов

    // СОГЛАШЕНИЕ: Все кастомные типы должны быть после USERTYPE
    OBJECTDATAROLE_USERTYPE,   //! Для пользовательских типов
};

class BasicItemInterface
{
private:
    ObjectItems::objectId_t m_id { NULL_OBJECT_ID };

    QString m_systemName {"Unknown item"};

    QString m_displayName;
    QString m_description;

    QColor m_lineColor;
    QColor m_backgroundColor;
    QColor m_selectionColor;

    static objectId_t createSystemId();;

public:
    bool isSystemObject() const;

    void setItemId(ObjectItems::objectId_t id);
    void setSystemId();
    ObjectItems::objectId_t getItemId() const;

    QString getSystemName() const;

    void setDisplayName(const QString& text);
    QString getDisplayName() const;

    void setDescription(const QString& text);
    QString getDescription() const;

    void setLineColor(const QColor& color);
    QColor getLineColor() const;

    void setBackgroundColor(const QColor& color);
    QColor getBackgroundColor() const;

    void setSelectionColor(const QColor& color);
    QColor getSelectionColor() const;

protected:
    virtual void processIdChange() = 0;
    virtual void processInternalDataChange() = 0;
    virtual void processColorChange() = 0;

    void setSystemName(const QString& iText);
};

}
