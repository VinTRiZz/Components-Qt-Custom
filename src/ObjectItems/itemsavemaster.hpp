#pragma once

#include <boost/hana.hpp>

#include <QString>
#include <QStringList>
#include <QBuffer>

#include "basicitem.hpp"

namespace ObjectItems::ObjectSaving {


// ========================================= //
template <typename T>
QString createString(const T& v) {
    QByteArray res;
    QBuffer buf(&res);
    buf.open(QIODevice::WriteOnly);
    QDataStream str (&buf);
    str << v;
    return res.toHex();
}

// ========================================= //
template <typename T>
T stringToValue(const QString& iString) {
    QByteArray res = QByteArray::fromHex(iString.toUtf8());
    QBuffer buf(&res);
    buf.open(QIODevice::ReadOnly);
    QDataStream str (&buf);
    T v;
    str >> v;
    return v;
}


// ========================================= //
template <typename T>
QString serialize(T* const item) {
    assert(item != nullptr); // Это должно обязательно соблюдаться
    QString result;
    if constexpr (!std::is_same_v<ObjectItems::base_of_t<T>, void>) {
        result += serialize(static_cast<ObjectItems::base_of_t<T>* const>(item));
    }
    boost::hana::for_each(boost::hana::keys(*item), [&](auto&& key) {
        auto& value = boost::hana::at_key(*item, key);
        using memberType = std::remove_reference_t<decltype(value)>;
        if constexpr (std::is_pointer_v<memberType>) {
            result += serialize(value) + ";";
        } else {
            result += createString(value) + ";";
        }
    });
    return result;
}

template <typename T>
void deserialize(const QString& itemData, T* result, int startIndex = 0) {
    assert(result != nullptr);
    auto values = itemData.split(";");
    int currentIdx = startIndex;
    if constexpr (!std::is_same_v<ObjectItems::base_of_t<T>, void>) {
        deserialize(itemData, static_cast<ObjectItems::base_of_t<T>* const>(result), startIndex);
    }
    boost::hana::for_each(boost::hana::keys(*result), [&](auto&& key) {
        if (currentIdx >= values.size()) {
            throw std::invalid_argument("Invalid item data string");
        }
        auto& member = boost::hana::at_key(*result, key);
        using memberType = std::remove_reference_t<decltype(member)>;
        if constexpr (std::is_pointer_v<memberType>) {
            deserialize(values[currentIdx], member);
        } else {
            auto v = stringToValue<memberType>(values[currentIdx]);
            member = v;
        }
        ++currentIdx;
    });
}

}
