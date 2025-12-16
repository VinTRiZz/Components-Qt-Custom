#include <gtest/gtest.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>
#include <QApplication>

using namespace ObjectItems;

TEST(QtCustom_ObjectItems, ItemSaver) {
    int argc {0};
    char** argv = nullptr;
    QApplication app(argc, argv);

    BasicItem testItem;
    BasicItem testItemDeser;

    testItem.setSystemName("Aboba");
    testItem.setDisplayName("Testovi hren");

    auto ser = ObjectSaving::serialize(&testItem);
    ASSERT_FALSE(ser.isEmpty());

    ObjectSaving::deserialize(ser, &testItemDeser);
    auto ser2 = ObjectSaving::serialize(&testItemDeser);
    ASSERT_EQ(ser, ser2);
}
