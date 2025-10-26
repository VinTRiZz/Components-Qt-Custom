#pragma once

#include <stdint.h>

#include <QtCore>

namespace ObjectItems {

// СОГЛАШЕНИЕ:
// ID НЕ ДОЛЖЕН БЫТЬ ОТРИЦАТЕЛЬНЫМ ДЛЯ ОБЫЧНЫХ ЭЛЕМЕНТОВ
// ДЛЯ СИСТЕМНЫХ ID ОТРИЦАТЕЛЬНЫЙ
using objectId_t = long long;

/**
 * @brief The ObjectDataRole enum Определяет, какие данные могут быть в объекте
 */
enum ObjectDataRole : int {
    OBJECTDATAROLE_ID = Qt::UserRole + 1000,   //! Короткое имя объекта для отображения
    OBJECTDATAROLE_PARENTITEM_ID,  //! Для комплексных объектов
    OBJECTDATAROLE_OBJECTTYPE,  //! Специальное поле для идентификации типа объекта
                             //! (желательно к использованию при сложной логике)

    OBJECTDATAROLE_NAME_SYSTEM,  //! Имя объекта в системе (изменять только в
                              //! конструкторе)

    OBJECTDATAROLE_DISPLAY_NAME,  //! Короткое имя объекта
    OBJECTDATAROLE_DESCRIPTION,   //! Описание объекта

    OBJECTDATAROLE_COLOR_BORDER,      //! Основной цвет (цвет пера)
    OBJECTDATAROLE_COLOR_BACKGROUND,  //! Фоновый цвет (заполнения в объекте)
    OBJECTDATAROLE_COLOR_SELECTED,    //! Цвет индикации выбора

    OBJECTDATAROLE_USERTYPE,   //! Для пользовательских типов
};

/**
 * @brief The ObjectType enum Определяет, какого типа этот объект. Используется
 * в регулировании взаимодействий
 */
enum ObjectType : int {
    OBJECTTYPE_NONE = 0,
    OBJECTTYPE_MARKER,
    OBJECTTYPE_PICTURE,
    OBJECTTYPE_CANVAS_CENTER,
    OBJECTTYPE_ARROWLINE,
};

/**
 * @brief The LineArrowType enum Возможные виды направления стрелки
 */
enum LineArrowType : short {
    None = 0,
    Forward = 0b01,
    Backward = 0b10,
    Bidirectional = Forward & Backward,
};

}  // namespace ObjectItems
