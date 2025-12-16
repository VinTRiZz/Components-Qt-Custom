#pragma once

#include <boost/hana.hpp>

#include <QString>
#include <QStringList>

namespace ObjectItems {

class ItemSaveMaster
{
    template <typename T>
    QString createString(const T& v) {
        return v;
    }

    template <typename T>
    T stringToValue(const QString& iString) {
        return iString;
    }

public:
    template <typename T>
    QString serialize(const T& item) {
        QString result;
        boost::hana::for_each(item, boost::hana::fuse([&result, this](auto, auto value) {
            result += createString(value) + ",";
        }));
        return result;
    }

    template <typename T>
    T deserialize(const QString& itemData) {
        T result;
        auto needSize = boost::hana::sizeof_(result);

        auto values = itemData.split(",");
        if (values.size() != needSize) {
            throw std::runtime_error("Invalid item data string");
            return {};
        }
        decltype(values.size()) valuePos = 0;
        boost::hana::for_each(boost::hana::keys(result), [&](auto key) {
            auto& member = boost::hana::at_key(result, key);
            member = stringToValue<std::remove_reference_t<decltype(member)>>(values[valuePos++]);
        });
        return std::move(result);
    }
};


}
