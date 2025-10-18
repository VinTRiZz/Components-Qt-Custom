#ifndef OBJECTSCENE_H
#define OBJECTSCENE_H

#include "ovnavigationlayer.hpp"

namespace Ui {
class ObjectScene;
}

class ObjectsInternalScene;
class QMenu;

/**
 * @brief The ObjectScene class Посредник для отображения объектов на
 * QGraphicScene с доп функционалом
 */
class ObjectView : public ObjectViewLayers::OVNavigationLayer {
    Q_OBJECT
public:
    explicit ObjectView(QWidget* parent = nullptr);
    ~ObjectView();
};

#endif  // OBJECTSCENE_H
