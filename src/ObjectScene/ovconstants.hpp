#pragma once

#include <QString>
#include <QVariant>
#include <QColor>

namespace ObjectViewLayers {

const QColor DEFAULT_GRIDCOLOR {QColor(15, 90, 180, 100)};
const QColor DEFAULT_CANVASCOLOR {QColor(220, 220, 220)};
const QColor DEFALT_CANVASBORDERCOLOR {QColor(70, 60, 60)};

enum ItemLayers : int {
    CanvasLayer = -1,


};

}
