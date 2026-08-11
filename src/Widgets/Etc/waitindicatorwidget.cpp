#include "waitindicatorwidget.hpp"

#include <QEventLoop>
#include <QVariantAnimation>

#include <QPen>
#include <QPainter>
#include <QPaintEvent>

#include <math.h>

namespace QtCustom::Widgets {

struct WaitIndicatorWidget::Impl
{
    QWidget* m_pTargetWidget {nullptr};

    // Main logic
    bool m_isDisablingParent {true};

    // Dynamic processing
    Status      m_status            {Status::Ready};
    uint64_t    m_currentPercent    {0};                // For example, 99.123 is 99123. Used to workaround double store errors
    QVariantAnimation* m_pPrimaryAnimation      {nullptr};  // Main action (for example, percent change)
    QVariantAnimation* m_pBackgroundAnimation   {nullptr};  // Passive actions (for example, particles behind widget)

    // Display configuration
    bool    m_isPercentVisible  {true};
    Shape   m_shape             {Shape::Circle};
    QString m_titleText         {"Please, be patient..."};
    QString m_descriptionText   {"Operation is in process."};

    // Draw information
    QSize   m_size {60, 60};
    QPen    m_primaryPen    {QPen(QColor(45, 210, 170), 2, Qt::SolidLine, Qt::RoundCap)};
    QPen    m_secondaryPen  {QPen(QColor(65, 120, 110), 2, Qt::SolidLine, Qt::RoundCap)};
};

WaitIndicatorWidget::WaitIndicatorWidget(QWidget *parent) :
    QWidget{parent},
    d {new Impl}
{
    d->m_pPrimaryAnimation = new QVariantAnimation(this);
    d->m_pBackgroundAnimation = new QVariantAnimation(this);

    // Work with source transparency for better displaying
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

WaitIndicatorWidget::~WaitIndicatorWidget()
{
    stop();
}

void WaitIndicatorWidget::setTarget(QWidget *pWidget)
{
    d->m_pTargetWidget = pWidget;
    updateVisualState();
}

QWidget *WaitIndicatorWidget::getTarget() const
{
    return d->m_pTargetWidget;
}

void WaitIndicatorWidget::setIndicatorRect(const QSize &indSize)
{
    d->m_size = indSize;
}

QSize WaitIndicatorWidget::getIndicatorRect() const
{
    return d->m_size;
}

void WaitIndicatorWidget::setDisablingParent(bool isDisabling)
{
    d->m_isDisablingParent = isDisabling;
    updateVisualState();
}

bool WaitIndicatorWidget::isDisablingParent() const
{
    return d->m_isDisablingParent;
}

void WaitIndicatorWidget::start()
{
    if (d->m_status & Status::Working) {
        return;
    }
    d->m_status = Status::Working;

    if (d->m_isDisablingParent) {
        d->m_pTargetWidget->setEnabled(false);
    }
}

void WaitIndicatorWidget::pause()
{
    if (d->m_status & (Status::Paused | Status::Ready)) {
        return;
    }
    d->m_status = Status::Pausing;
}

void WaitIndicatorWidget::stop()
{
    if (d->m_status & Status::Ready) {
        return;
    }
    d->m_status = Status::Stopping;

    pollAnimation();
    if (d->m_pBackgroundAnimation->state() == QVariantAnimation::Running) {
        d->m_pBackgroundAnimation->stop();
    }

    if (d->m_isDisablingParent) {
        d->m_pTargetWidget->setEnabled(true);
    }
}

void WaitIndicatorWidget::pollAnimation() const
{
    // No animation pending
    if (d->m_status ^ ~Status::InProcess) {
        return;
    }

    // Wait for animation to complete
    QEventLoop loop;
    connect(d->m_pPrimaryAnimation, &QVariantAnimation::finished,
            &loop, &QEventLoop::quit);
    loop.exec();
}

WaitIndicatorWidget::Status WaitIndicatorWidget::getStatus() const
{
    return d->m_status;
}

void WaitIndicatorWidget::setPercent(double perc)
{
    d->m_currentPercent = perc * 100;
    updateVisualState();
}

double WaitIndicatorWidget::getCurrentPercent() const
{
    double res {};
    res += d->m_currentPercent / 100;
    res += 0.001f * (d->m_currentPercent % 100);
    return res;
}

void WaitIndicatorWidget::setPercentVisible(bool isPercentVisible)
{
    d->m_isPercentVisible = isPercentVisible;
    updateVisualState();
}

bool WaitIndicatorWidget::isPercentVisible() const
{
    return d->m_isPercentVisible;
}

void WaitIndicatorWidget::setTitle(const QString &text)
{
    d->m_titleText = text;
    updateVisualState();
}

QString WaitIndicatorWidget::getTitle() const
{
    return d->m_titleText;
}

void WaitIndicatorWidget::setDescription(const QString &text)
{
    d->m_descriptionText = text;
    updateVisualState();
}

QString WaitIndicatorWidget::getDescription() const
{
    return d->m_descriptionText;
}

void WaitIndicatorWidget::setShape(Shape itype)
{
    d->m_shape = itype;
    updateVisualState();
}

WaitIndicatorWidget::Shape WaitIndicatorWidget::getShape() const
{
    return d->m_shape;
}

void WaitIndicatorWidget::setShapePenPrimary(const QPen &primaryPen)
{
    d->m_primaryPen = primaryPen;
    updateVisualState();
}

QPen WaitIndicatorWidget::getShapePenPrimary() const
{
    return d->m_primaryPen;
}

void WaitIndicatorWidget::setShapePenSecondary(const QPen &secondaryPen)
{
    d->m_secondaryPen = secondaryPen;
    updateVisualState();
}

QPen WaitIndicatorWidget::getShapePenSecondary() const
{
    return d->m_secondaryPen;
}

void WaitIndicatorWidget::updateVisualState()
{
    // TODO: Start animation if needed

    update();
}

void WaitIndicatorWidget::switchState()
{
    // TODO: Process state changing
}

void WaitIndicatorWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event); // Draw base
}

}