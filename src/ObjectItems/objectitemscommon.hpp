#pragma once

#include <QString>

#include <boost/hana.hpp>

namespace ObjectItems
{

// СОГЛАШЕНИЕ:
// ID НЕ ДОЛЖЕН БЫТЬ ОТРИЦАТЕЛЬНЫМ ДЛЯ ОБЫЧНЫХ ЭЛЕМЕНТОВ
// ДЛЯ СИСТЕМНЫХ ID ОТРИЦАТЕЛЬНЫЙ
// ID НЕ ДОЛЖЕН БЫТЬ РАВЕН 0
using objectId_t = long long;
const objectId_t NULL_OBJECT_ID { 0 };

/**
 * @brief The ObjectDataRole enum Определяет, какие данные могут быть в объекте
 */
enum ObjectDataRole : int {
    OBJECTDATAROLE_ID = Qt::UserRole + 1000,   //! Короткое имя объекта для отображения
    OBJECTDATAROLE_PARENTITEM_ID,  //! Для комплексных объектов
    OBJECTDATAROLE_COMPLEX_PARENTITEM_ID,  //! Для объектов из комплексных объектов

    // СОГЛАШЕНИЕ: Все кастомные типы должны быть после USERTYPE
    OBJECTDATAROLE_USERTYPE,   //! Для пользовательских типов
};

/**
 * @brief The ObjectType enum Тип объекта, чтобы не заниматься динамик кастами
 */
enum ObjectType : int {
    OIT_Undefined = -1,
    OIT_BasicItem,

    OIT_AnchorItem,

    OIT_AbstractConnectionLine,
    OIT_ArrowedConnectionLine,
    OIT_ElegantConnectionLine,

    OIT_AbstractText,
    OIT_TextLabel,

    OIT_GroupItem,
    OIT_CommentItem,
    OIT_MarkerItem,

    OIT_UserItemType = 100, // Для пользовательских типов объекта
};

}
