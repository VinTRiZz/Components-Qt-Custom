#include "waitindicatordrawhelper.hpp"

#include <QPainter>
#include <QVariantAnimation>

#include <QEventLoop>

#include "waitindicatorwidget.hpp"
#include "waitindicatorutility.hpp"

namespace QtCustom::Widgets {

WaitIndicatorDrawHelper *WaitIndicatorDrawHelper::create(const IndicatorConfigurationBasePtr &cfg, QObject *parent) {
    if (dynamic_cast<IndicatorCircleConfiguration*>(cfg.get())) {
        auto pHelper = new CircleDrawHelper(parent);
        pHelper->setConfiguration(cfg);
        return pHelper;
    }
    return nullptr;
}

void WaitIndicatorDrawHelper::setConfiguration(const IndicatorConfigurationBasePtr &pConfig) {
    m_pConfig = pConfig;
}

void CircleDrawHelper::init()
{
    m_pPrimaryAnimation = new QVariantAnimation(this);
    // connect(m_pPrimaryAnimation, &QVariantAnimation::valueChanged,
    //         this, &WaitIndicatorWidget::slot_switchState);
    // connect(m_pPrimaryAnimation, &QVariantAnimation::finished,
    //         this, &WaitIndicatorWidget::slot_finishSwitchChange);

    m_pSecondaryAnimation = new QVariantAnimation(this);
    // connect(m_pSecondaryAnimation, &QVariantAnimation::valueChanged,
    //         this, &WaitIndicatorWidget::slot_updateSecondary);
    // connect(m_pSecondaryAnimation, &QVariantAnimation::finished,
    //         this, &WaitIndicatorWidget::slot_finishSecondary);
}

void CircleDrawHelper::paint(QPainter *pPainter,
                             const QRect& targetWidgetRect,
                             double currentPercent,
                             uint8_t indicatorStatus)
{
    // Move to desired draw location
    pPainter->save();

    auto convertedRect = getCircleArea(targetWidgetRect);

    pPainter->setClipRect(convertedRect);
    pPainter->translate(convertedRect.topLeft());

    paintIndicatorCircle(pPainter, currentPercent);
    paintPercent(pPainter, convertedRect, indicatorStatus, currentPercent);

    pPainter->restore();

    paintTitle(pPainter, convertedRect);
    paintDescription(pPainter, convertedRect);

    pPainter->end();
}

void CircleDrawHelper::startAnimation()
{
    m_pPrimaryAnimation->start();
}

void CircleDrawHelper::pauseAnimation()
{
    m_pPrimaryAnimation->pause();
}

void CircleDrawHelper::continueAnimation()
{
    m_pPrimaryAnimation->resume();
}

void CircleDrawHelper::stopAnimation()
{
    m_pPrimaryAnimation->stop();
    m_pSecondaryAnimation->stop();
}

void CircleDrawHelper::pollAnimation()
{
    // Wait for animation to complete
    QEventLoop loop;
    connect(m_pPrimaryAnimation, &QVariantAnimation::finished,
            &loop, &QEventLoop::quit);
    loop.exec();
}

QRect CircleDrawHelper::createCircleRect() const
{
    return QRect(CIRCLE_RECT_OFFSET, CIRCLE_RECT_OFFSET, getConfig()->m_size.width(), getConfig()->m_size.height());
}

QRect CircleDrawHelper::getCircleArea(QRect targetWidgetRect) const
{
    auto rectCenter = targetWidgetRect.center();
    auto workAreaSize = getConfig()->m_size;
    targetWidgetRect.setWidth(workAreaSize.width() + CIRCLE_RECT_OFFSET * 2);
    targetWidgetRect.setHeight(workAreaSize.height() + CIRCLE_RECT_OFFSET * 2);
    targetWidgetRect.moveCenter(rectCenter);
    return targetWidgetRect;
}

void CircleDrawHelper::paintTitle(QPainter* pPainter, const QRect &targetWidgetRect) const
{
    pPainter->save();

    pPainter->setPen(getConfig()->m_textPen);
    auto fnt = pPainter->font();
    fnt.setBold(true);
    fnt.setPixelSize(14);
    pPainter->setFont(fnt);

    auto textRect = getCircleArea(targetWidgetRect);
    textRect.moveTo(textRect.x() - textRect.width() * 1.5, textRect.y() + textRect.height() * 0.8);
    textRect.setWidth(textRect.width() + textRect.width() * 3);
    pPainter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, getConfig()->getTitle());

    pPainter->restore();
}

void CircleDrawHelper::paintDescription(QPainter* pPainter, const QRect &targetWidgetRect) const
{
    pPainter->save();

    pPainter->setPen(getConfig()->m_textPen);
    auto fnt = pPainter->font();
    fnt.setItalic(true);
    fnt.setPixelSize(10);
    pPainter->setFont(fnt);

    auto textRect = getCircleArea(targetWidgetRect);
    textRect.moveTo(textRect.x() - textRect.width() * 1.5, textRect.y() + textRect.height() * 1.4);
    textRect.setWidth(textRect.width() + textRect.width() * 3);
    pPainter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, getConfig()->getDescription());

    pPainter->restore();
}

void CircleDrawHelper::paintPercent(QPainter* pPainter, const QRect &targetWidgetRect, uint8_t indicatorStatus, double currentPercent) const
{
    if (indicatorStatus & WaitIndicatorWidget::Status::Paused) {
        return;
    }
    pPainter->save();

    auto pConfig = getConfig<IndicatorCircleConfiguration>();
    pPainter->setPen(pConfig->m_primaryPen);
    pPainter->drawText(createCircleRect(), Qt::AlignCenter, QString("%1%").arg(currentPercent));

    pPainter->restore();
}

void CircleDrawHelper::paintIndicatorCircle(QPainter* pPainter, double currentPercent) const
{
    pPainter->setBrush(Qt::transparent);

    auto pConfig = getConfig<IndicatorCircleConfiguration>();

    pPainter->setPen(pConfig->m_secondaryPen);
    pPainter->drawEllipse(createCircleRect());

    pPainter->setPen(pConfig->m_primaryPen);
    pPainter->drawArc(createCircleRect(),
                        utilityPieFromDegree(90),
                        -utilityPieFromDegree(3.6f * currentPercent));
}

}