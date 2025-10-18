#include "objectview.hpp"

#include <QScrollBar>

ObjectView::ObjectView(QWidget* parent) : ObjectViewLayers::OVNavigationLayer(parent) {
    setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
}

ObjectView::~ObjectView() {}
