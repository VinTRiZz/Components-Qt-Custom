#include "ovinformationlayer.hpp"

#include <QMouseEvent>

#include "ovinternalscene.hpp"
#include "ovconstants.hpp"

namespace ObjectViewLayers {

OVInformationLayer::OVInformationLayer(QWidget *parent) :
    OVItemGeometryLayer(parent)
{
    m_pCursorLabel = new ObjectViewItems::LabelItem;
    m_pCursorLabel->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    getScene()->addItem(m_pCursorLabel);
    m_pCursorLabel->setZValue(ItemLayers::CursorLabel);
    m_pCursorLabel->setBackgroundColor(Qt::white);

    setCursorValuesPresenter([](const QPointF& curPoint) -> QString {
        return QString("X: %0\nY: %1").arg(QString::number(curPoint.x()), QString::number(curPoint.y()));
    });

    // Апдейт для скейлов
    connect(this, &OVNavigationLayer::scaleChanged,
            this, &OVInformationLayer::updateCursorLabel);
}

ObjectViewItems::LabelItem *OVInformationLayer::getCursorLabel() const
{
    return m_pCursorLabel;
}

void OVInformationLayer::setCursorValuesPresenter(const std::function<QString (const QPointF &)> &pres)
{
    m_cursorValuesPresenter = pres;
}

void OVInformationLayer::updateCursorLabel()
{
    auto cursorPos = mapToScene(mapFromGlobal(QCursor::pos()));
    m_pCursorLabel->setPos(cursorPos + QPointF(10, 10) / getCurrentScale());
    m_pCursorLabel->setDisplayName(m_cursorValuesPresenter(cursorPos));
}

void OVInformationLayer::wheelEvent(QWheelEvent *e)
{
    OVItemGeometryLayer::wheelEvent(e);
}

void OVInformationLayer::mousePressEvent(QMouseEvent *e)
{
    OVItemGeometryLayer::mousePressEvent(e);
}

void OVInformationLayer::mouseMoveEvent(QMouseEvent *e)
{
    OVItemGeometryLayer::mouseMoveEvent(e);
    updateCursorLabel();
}

void OVInformationLayer::mouseReleaseEvent(QMouseEvent *e)
{
    OVItemGeometryLayer::mouseReleaseEvent(e);
}

void OVInformationLayer::enterEvent(QEvent *e)
{
    OVItemGeometryLayer::enterEvent(e);
    m_pCursorLabel->show();
}

void OVInformationLayer::leaveEvent(QEvent *e)
{
    OVItemGeometryLayer::leaveEvent(e);
    m_pCursorLabel->hide();
}

} // namespace ObjectViewLayers
