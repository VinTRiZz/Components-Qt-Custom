#pragma once

#include <stdint.h>

namespace ObjectViewItems {

// СОГЛАШЕНИЕ:
// ID НЕ ДОЛЖЕН БЫТЬ ОТРИЦАТЕЛЬНЫМ ДЛЯ ОБЫЧНЫХ ЭЛЕМЕНТОВ
// ДЛЯ СИСТЕМНЫХ ID ОТРИЦАТЕЛЬНЫЙ
using objectId_t = long long;

/**
 * @brief The ObjectField enum Определяет, какие данные могут быть в объекте
 */
enum ObjectField : int {
    OBJECTFIELD_PARENTITEM_ID = 0,  //! Для комплексных объектов
    OBJECTFIELD_ID,                 //! Короткое имя объекта для отображения
    OBJECTFIELD_OBJECTTYPE,  //! Специальное поле для идентификации типа объекта
                             //! (желательно к использованию при сложной логике)

    OBJECTFIELD_NAME_SYSTEM,  //! Имя объекта в системе (изменять только в
                              //! конструкторе)

    OBJECTFIELD_PICTURE_HASH,  //! Хеш картинки для сверки с задаваемым и т.д.
    OBJECTFIELD_DISPLAY_NAME,  //! Короткое имя объекта
    OBJECTFIELD_DESCRIPTION,   //! Описание объекта

    OBJECTFIELD_COLOR_BORDER,      //! Основной цвет (цвет пера)
    OBJECTFIELD_COLOR_BACKGROUND,  //! Фоновый цвет (заполнения в объекте)
    OBJECTFIELD_COLOR_SELECTED,    //! Цвет индикации выбора

    OBJECTFIELD_USERTYPE,   //! Для пользовательских типов
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

}  // namespace ObjectViewItems
