#include "ovinformationlayer.hpp"

#include <QMouseEvent>

#include "ovinternalscene.hpp"
#include "ovconstants.hpp"

namespace ObjectViewLayers {

OVInformationLayer::OVInformationLayer(QWidget *parent) :
    OVContextMenuLayer(parent)
{
    m_pCursorLabel = new ObjectViewItems::LabelItem;
    m_pCursorLabel->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    getScene()->addItem(m_pCursorLabel);
    m_pCursorLabel->setZValue(ItemLayers::CursorLabel);
    m_pCursorLabel->setBackgroundColor(Qt::white);

    m_pInformationLabel = new QLabel(this);
    m_pInformationLabel->setWordWrap(true);
    m_pInformationLabel->setFixedSize(500, 50);

    // Настройка для отображения
    m_pInformationLabel->setStyleSheet(
        R"(
    color: black;
    font-size: 14px;
    background-color: rgba(200, 240, 210, 80);
    border: 1px solid black;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
)");
    updateInformationLabel();

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

QLabel *OVInformationLayer::getInformationLabel() const
{
    return m_pInformationLabel;
}

void OVInformationLayer::setCurrentToolname(const QString &toolName)
{
    m_currentToolName = toolName;
    updateInformationLabel();
}

void OVInformationLayer::setInformationFormat(const QString &infoFormat)
{
    m_currentInfoFormat = infoFormat;
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

    // TODO: Use
//    auto hoverItem = itemAt(currentPos);
//    if ((nullptr != hoverItem) &&
//        (dynamic_cast<ObjectViewItems::SceneMarkerItem*>(hoverItem) ==
//         nullptr)) {
//        auto pHoverItemParent = getParentOfComplex(hoverItem);
//        if (nullptr != pHoverItemParent) {
//            hoverItemName = pHoverItemParent->getSystemName();
//        } else {
//            hoverItemName =
//                hoverItem->data(ObjectViewItems::OBJECTFIELD_NAME_SYSTEM)
//                    .toString();
//        }
//    }
}

void OVInformationLayer::updateInformationLabel() {
    auto isGridEnabled = getScene()->getIsGridEnabled();
    auto gridSize = getScene()->getGridSize();
    auto infoText = m_currentInfoFormat.arg(
                QString::number(getCurrentScale(), 'f', 3),
                 isGridEnabled ? QString("%0px").arg(gridSize)
                               : QString("Выкл."),
                 m_currentToolName.isEmpty() ? QString("Нет")
                                             : m_currentToolName);

    m_pInformationLabel->setText(infoText);
}

void OVInformationLayer::wheelEvent(QWheelEvent *e)
{
    OVContextMenuLayer::wheelEvent(e);
}

void OVInformationLayer::mousePressEvent(QMouseEvent *e)
{
    OVContextMenuLayer::mousePressEvent(e);
}

void OVInformationLayer::mouseMoveEvent(QMouseEvent *e)
{
    OVContextMenuLayer::mouseMoveEvent(e);
    updateCursorLabel();
}

void OVInformationLayer::mouseReleaseEvent(QMouseEvent *e)
{
    OVContextMenuLayer::mouseReleaseEvent(e);
}

void OVInformationLayer::enterEvent(QEvent *e)
{
    OVContextMenuLayer::enterEvent(e);
    m_pCursorLabel->show();
}

void OVInformationLayer::leaveEvent(QEvent *e)
{
    OVContextMenuLayer::leaveEvent(e);
    m_pCursorLabel->hide();
}

void OVInformationLayer::resizeEvent(QResizeEvent *e) {
    m_pInformationLabel->move(10, height() - m_pInformationLabel->height());
    OVContextMenuLayer::resizeEvent(e);
}

} // namespace ObjectViewLayers
