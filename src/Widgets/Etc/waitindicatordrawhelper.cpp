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
        pHelper->init();
        return pHelper;
    }
    return nullptr;
}

void WaitIndicatorDrawHelper::setConfiguration(const IndicatorConfigurationBasePtr &pConfig) {
    m_pConfig = pConfig;
}

void CircleDrawHelper::init()
{
    m_pRollingAnimation = new QVariantAnimation(this);
    connect(m_pRollingAnimation, &QVariantAnimation::valueChanged,
            this, &CircleDrawHelper::slot_processRollingAnimation);
    connect(m_pRollingAnimation, &QVariantAnimation::finished,
            this, [this](){
        m_pRollingAnimation->start();
    });

    m_pRollingAnimation->setStartValue(double(0));
    m_pRollingAnimation->setEndValue(double(360));
    m_pRollingAnimation->setDuration(720);
}

void CircleDrawHelper::paint(QPainter *pPainter,
                             const QRect& targetWidgetRect,
                             double currentPercent,
                             uint8_t indicatorStatus)
{
    // Move to desired draw location
    pPainter->save();

    m_isCircleHasSpaces = (currentPercent > 0 && currentPercent < 100);

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
    m_pRollingAnimation->start();
}

void CircleDrawHelper::pauseAnimation()
{
    m_pRollingAnimation->pause();
}

void CircleDrawHelper::continueAnimation()
{
    m_pRollingAnimation->resume();
}

void CircleDrawHelper::stopAnimation()
{
    m_pRollingAnimation->stop();
}

void CircleDrawHelper::pollAnimation()
{
    // Wait for animation to complete
    QEventLoop loop;
    connect(m_pRollingAnimation, &QVariantAnimation::finished,
            &loop, &QEventLoop::quit);
    loop.exec();
}

void CircleDrawHelper::slot_processRollingAnimation(const QVariant &animationValue)
{
    m_animationOffsetPercent = animationValue.toDouble();
    if (m_isCircleHasSpaces) {
        qobject_cast<QWidget*>(parent())->update(); // TODO: Add checks?
    }
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
                        utilityPieFromDegree(90 - m_animationOffsetPercent),
                        -utilityPieFromDegree(3.6f * currentPercent));
}

}