#pragma once

#include <QGraphicsItem>

#include <QPen>
#include <QBrush>

#include "objectitemscommon.hpp"

namespace ObjectItems {

class BasicItemInterface
{
private:
    ObjectItems::objectId_t m_id { NULL_OBJECT_ID };

    int m_itemType {ObjectType::OIT_Undefined};
    QString m_systemName {"Unknown item"};

    QString m_displayName;
    QString m_description;

    QPen m_linePen                      {Qt::black};
    QPen m_lineHoverPen                 {Qt::black};
    QPen m_selectionPen                 {QColor(180, 180, 130)};
    QBrush m_backgroundBrush            {Qt::transparent};
    QBrush m_backgroundHoverBrush       {Qt::transparent};
    QBrush m_backgroundSelectionBrush   {Qt::transparent};

    static objectId_t createSystemId();

public:
    bool isSystemObject() const;

    void setItemId(ObjectItems::objectId_t id);
    void setSystemId();
    ObjectItems::objectId_t getItemId() const;

    void setObjectType(int typ);
    int getObjectType() const;

    QString getSystemName() const;

    void setDisplayName(const QString& text);
    QString getDisplayName() const;

    void setDescription(const QString& text);
    QString getDescription() const;

    void setLinePen(const QPen& penC);
    QPen getLinePen() const;

    void setHoverPen(const QPen& penC);
    QPen getHoverPen() const;

    void setSelectionPen(const QPen& penC);
    QPen getSelectionPen() const;

    void setBackgroundBrush(const QBrush& brushC);
    QBrush getBackgroundBrush() const;

    void setBackgroundSelectionBrush(const QBrush& brushC);
    QBrush getBackgroundSelectionBrush() const;

    void setBackgroundHoverBrush(const QBrush& brushC);
    QBrush getBackgroundHoverBrush() const;

    void setSystemName(const QString& iText);
protected:
    virtual void processIdChange() = 0;
    virtual void processDisplayNameChange() = 0;
    virtual void processInternalDataChange() = 0;
    virtual void processColorChange() = 0;


    template<typename, typename>
    friend struct boost::hana::accessors_impl;
};

// Работа с дочерними типами (используется для корректной сериализации)
#define OBJECTITEMS_REGISTER_HIERARCHY(Derived, Base) \
    template<> struct ObjectItems::base_of<Derived> { using type = Base; };

template<typename T>
struct base_of { using type = void; };

template<typename T>
using base_of_t = typename base_of<T>::type;

}

BOOST_HANA_ADAPT_STRUCT(
    ObjectItems::BasicItemInterface,
    m_id,

    m_itemType,
    m_systemName,

    m_displayName,
    m_description

//    m_linePen                    ,
//    m_lineHoverPen               ,
//    m_selectionPen               ,
//    m_backgroundBrush            ,
//    m_backgroundHoverBrush       ,
//    m_backgroundSelectionBrush
);
