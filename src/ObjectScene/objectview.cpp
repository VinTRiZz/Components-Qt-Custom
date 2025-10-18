#include "objectview.hpp"

#include <QScrollBar>

ObjectView::ObjectView(QWidget* parent) : ObjectViewLayers::OVInformationLayer(parent) {
    setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
}

ObjectView::~ObjectView() {}
