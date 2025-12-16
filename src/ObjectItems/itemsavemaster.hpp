#pragma once

#include <boost/hana.hpp>

#include <QString>
#include <QStringList>

#include "basicitem.hpp"

#include <QDebug>

namespace ObjectItems::ObjectSaving {


// ========================================= //
template <typename T>
QString createString(const T& v);

template <>
inline QString createString(const long long& v) {
    return QString::number(v);
}

template <>
inline QString createString(const int& v) {
    return QString::number(v);
}

template <>
inline QString createString(const QString& v) {
    return v;
}

inline QString createString(const char* v) {
    return QString(v);
}




// ========================================= //
template <typename T>
T stringToValue(const QString& iString);

template <>
inline QString stringToValue(const QString& iString) {
    return iString;
}

template <>
inline long long stringToValue(const QString& iString) {
    return iString.toLongLong();
}

template <>
inline int stringToValue(const QString& iString) {
    return iString.toInt();
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
            result += serialize(value) + ",";
        } else {
            result += createString(value).toUtf8().toHex() + ",";
        }
    });
    return result;
}

template <typename T>
void deserialize(const QString& itemData, T* result, int startIndex = 0) {
    assert(result != nullptr);
    auto values = itemData.split(",");
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
            if (!deserialize(values[currentIdx], member)) {
                throw std::invalid_argument("Invalid member data string");
            }
        } else {
            auto v = stringToValue<memberType>(QByteArray::fromHex(values[currentIdx].toUtf8()));
            member = v;
        }
        ++currentIdx;
    });
}

}
