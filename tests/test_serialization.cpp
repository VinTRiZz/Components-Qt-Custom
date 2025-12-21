#include <gtest/gtest.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>
#include <Components/CustomQt/ObjectView/ItemSaveMaster.h>
#include <QApplication>

#include <random>

#include <QDebug>

using namespace ObjectItems;

namespace {
enum PenColorType {
    PCT_COMMON = 0,
    PCT_HOVER,
    PCT_SELECTED,
};


static std::random_device rd;
static std::mt19937 rng(rd());

int genValue(int min, int max) {
    if (min >= max) throw std::invalid_argument("genValue: max >= min");
    std::uniform_int_distribution<int> uni(min, max);
    return uni(rng);
}

QColor genColor() {
    return QColor(genValue(0, 255),
                  genValue(0, 255),
                  genValue(0, 255));
}
}

template <typename T>
void testItemSerialization() {
    qInfo() << "Testing basic functionality of object type:" << typeid(T).name();

    T testItem;
    T testItemDeser;

    testItem.setDisplayName("Test text of item");
    testItem.setDescription("Test description");

    testItem.setItemId(3);

    std::array<QBrush, 3> testBrushes = {
        QBrush(genColor()),
        QBrush(genColor()),
        QBrush(genColor()),
    };

    testItem.setBackgroundBrush(testBrushes[PCT_COMMON]);
    testItem.setBackgroundHoverBrush(testBrushes[PCT_HOVER]);
    testItem.setBackgroundSelectionBrush(testBrushes[PCT_SELECTED]);

    std::array<QPen, 3> testPens = {
        QPen(genColor(), 13),
        QPen(genColor(), 9),
        QPen(genColor(), 2),
    };
    testItem.setLinePen(testPens[PCT_COMMON]);
    testItem.setLineHoverPen(testPens[PCT_HOVER]);
    testItem.setLineSelectionPen(testPens[PCT_SELECTED]);

    auto ser = ObjectSaving::serialize(&testItem);
    ASSERT_FALSE(ser.isEmpty());

    ObjectSaving::deserialize(ser, &testItemDeser);
    auto ser2 = ObjectSaving::serialize(&testItemDeser);

    ASSERT_EQ(ser, ser2);

    ASSERT_EQ(testItem.getBackgroundBrush(), testBrushes[PCT_COMMON]);
    ASSERT_EQ(testItem.getBackgroundHoverBrush(), testBrushes[PCT_HOVER]);
    ASSERT_EQ(testItem.getBackgroundSelectionBrush(), testBrushes[PCT_SELECTED]);

    ASSERT_EQ(testItem.getLinePen(), testPens[PCT_COMMON]);
    ASSERT_EQ(testItem.getLineHoverPen(), testPens[PCT_HOVER]);
    ASSERT_EQ(testItem.getLineSelectionPen(), testPens[PCT_SELECTED]);
}

TEST(QtCustom_ObjectItems_ItemSaver, BasicItem) {
    int argc {0};
    char** argv = nullptr;
    QApplication app(argc, argv);

    testItemSerialization<BasicItem>();

    testItemSerialization<AbstractText>();
    testItemSerialization<TextLabel>();

    testItemSerialization<GroupItem>();
    testItemSerialization<CommentItem>();

    testItemSerialization<MarkerItem>();
    testItemSerialization<AnchorItem>();
}


template <typename T>
void testConnectionLineSerialization() {
    testItemSerialization<T>();
    qInfo() << "Testing connection type:" << typeid(T).name();

    T testItem;
    T testItemDeser;

    testItem.setDirection(LineDirectionType::Backward);
    testItem.setArrowAngle(LineAngleType::A_45);
    testItem.setArrowHeight(13.234);
    testItem.setPositionFrom(QPointF(genValue(-50, 50), genValue(-50, 50)));
    testItem.setPositionTo(QPointF(genValue(-50, 50), genValue(-50, 50)));


    auto ser = ObjectSaving::serialize(&testItem);
    ASSERT_FALSE(ser.isEmpty());

    ObjectSaving::deserialize(ser, &testItemDeser);
    auto ser2 = ObjectSaving::serialize(&testItemDeser);

    qDebug() << "Ser deser:" << Qt::endl
             << ser << Qt::endl
             << ser2 << Qt::endl;
    ASSERT_EQ(ser, ser2);
}

TEST(QtCustom_ObjectItems_ItemSaver, ConnectionLines) {
    testConnectionLineSerialization<AbstractConnectionLine>();
    testConnectionLineSerialization<ArrowedConnectionLine>();
    testConnectionLineSerialization<ElegantConnectionLine>();
}
