#include "ovcanvaslayer.hpp"

#include "ovconstants.hpp"
#include "ovinternalscene.hpp"

#include <QScrollBar>

namespace ObjectViewLayers {

OVCanvasLayer::OVCanvasLayer(QWidget *parent) :
    QGraphicsView(parent)
{
    m_pInternalScene = new OVInternalScene(this);
    setScene(m_pInternalScene);

    m_pCanvasItem = new ObjectViewItems::SceneFieldItem;
    m_pInternalScene->addItem(m_pCanvasItem);
    m_pCanvasItem->setBrush(DEFAULT_CANVASCOLOR);
    m_pCanvasItem->setPen(QPen(DEFALT_CANVASBORDERCOLOR, 2));
    m_pCanvasItem->setZValue(ItemLayers::CanvasLayer);
    m_pCanvasItem->show();

    m_pCenterItem = new ObjectViewItems::CenterItem(m_pCenterItem);
    m_pInternalScene->addItem(m_pCenterItem);
    m_pCenterItem->setZValue(ItemLayers::CanvasLayer + 1);
}

void OVCanvasLayer::setCanvasRect(const QRectF &iRect) {
    m_pCanvasItem->setFieldRect(iRect);
    m_pCenterItem->setPos(getCanvasRect().center() - m_pCenterItem->boundingRect().center());
    resetTransform();

    QRectF viewRect = mapToScene(viewport()->rect()).boundingRect();
    auto viewCenter = viewRect.center();

    auto deltaPos = iRect.center() - viewCenter;
    horizontalScrollBar()->setSliderPosition(
                horizontalScrollBar()->sliderPosition() + deltaPos.x());
    verticalScrollBar()->setSliderPosition(
                verticalScrollBar()->sliderPosition() + deltaPos.y());

    auto rectCopy = iRect;
    rectCopy.moveTo(-10, -10);
    rectCopy.setSize(QSize(rectCopy.width() + 10, rectCopy.height() + 10));
    setSceneRect(rectCopy);
}

QRectF OVCanvasLayer::getCanvasRect() const
{
    return m_pCanvasItem->getFieldRect();
}

OVInternalScene *OVCanvasLayer::getScene() const
{
    return m_pInternalScene;
}

ObjectViewItems::SceneFieldItem *OVCanvasLayer::getCanvas() const
{
    return m_pCanvasItem;
}

ObjectViewItems::CenterItem *OVCanvasLayer::getCenterItem() const
{
    return m_pCenterItem;
}

QGraphicsItem *OVCanvasLayer::getTopItem(const QPoint &viewportPos) const
{
    auto posItems = items(viewportPos);
    std::sort(posItems.begin(), posItems.end(), [](auto* pItemL, auto* pItemR){
        return (pItemL->zValue() < pItemR->zValue());
    });
    auto resItem = std::find_if(posItems.begin(), posItems.end(), [this](auto* pItem){
        return  (pItem->zValue() > ItemLayers::CanvasLayer) &&
                (pItem->zValue() < ItemLayers::SystemComponentsLayerBegin) &&
                !isSystemItem(pItem);
    });
    if (resItem != posItems.end()) {
        return *resItem;
    }
    return nullptr;
}

QList<QGraphicsItem *> OVCanvasLayer::getItems(const QPoint &viewportPos, bool sorted) const
{
    auto posItems = items(viewportPos);
    std::sort(posItems.begin(), posItems.end(), [](auto* pItemL, auto* pItemR){
        return (pItemL->zValue() < pItemR->zValue());
    });
    auto removedBeg = std::remove_if(posItems.begin(), posItems.end(), [this](auto* pItem){
        return  (pItem->zValue() <= ItemLayers::CanvasLayer) &&
                (pItem->zValue() >= ItemLayers::SystemComponentsLayerBegin) &&
                isSystemItem(pItem);
    });
    posItems.erase(removedBeg, posItems.end());
    return posItems;
}

bool OVCanvasLayer::isSystemItem(QGraphicsItem *pItem) const
{
    return false;
}

}
