#include "waitindicatorwidget.hpp"

#include <QEventLoop>
#include <QVariantAnimation>

#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QPen>

#include <math.h>

#include <Components/Logger/Logger.h>

namespace QtCustom::Widgets {

constexpr auto INTERNAL_RECT_OFFSET {5};

struct WaitIndicatorWidget::Impl
{
    QWidget* m_pTargetWidget {nullptr};

    // Main logic
    bool m_isDisablingParent {true};

    // Dynamic processing
    Status      m_status            {Status::Ready};
    uint64_t    m_currentPercent    {0};                    // For example, 99.123 is 99123. Used to workaround double store errors
    QVariantAnimation* m_pPrimaryAnimation      {nullptr};  // Main action (for example, percent change)
    QVariantAnimation* m_pSecondaryAnimation    {nullptr};  // Passive actions (for example, particles behind widget)
    QPainter*          m_pPainter {nullptr};

    // Display configuration
    bool    m_isPercentVisible  {true};
    bool    m_isAnimationEnabled {false};
    Shape   m_shape             {Shape::Circle};
    uint    m_titleTextSymbolLimit  {25};
    QString m_titleText             {"Please, be patient..."};
    uint    m_descriptionTextSymbolLimit    {150};
    QString m_descriptionText               {"Operation is in process."};

    // Draw information
    QSize   m_size {60, 60};
    QPen    m_textPen       {QPen(QColor(85, 220, 190), 1, Qt::SolidLine, Qt::RoundCap)};
    QPen    m_primaryPen    {QPen(QColor(45, 210, 170), 4, Qt::SolidLine, Qt::RoundCap)};
    QPen    m_secondaryPen  {QPen(QColor(65, 120, 110), 2, Qt::SolidLine, Qt::RoundCap)};
};

WaitIndicatorWidget::WaitIndicatorWidget(QWidget *parent) :
    QWidget{parent},
    d {new Impl}
{
    d->m_pPrimaryAnimation = new QVariantAnimation(this);
    connect(d->m_pPrimaryAnimation, &QVariantAnimation::valueChanged,
            this, &WaitIndicatorWidget::slot_switchState);
    connect(d->m_pPrimaryAnimation, &QVariantAnimation::finished,
            this, &WaitIndicatorWidget::slot_finishSwitchChange);

    d->m_pSecondaryAnimation = new QVariantAnimation(this);
    connect(d->m_pSecondaryAnimation, &QVariantAnimation::valueChanged,
            this, &WaitIndicatorWidget::slot_updateSecondary);
    connect(d->m_pSecondaryAnimation, &QVariantAnimation::finished,
            this, &WaitIndicatorWidget::slot_finishSecondary);

    // Work with source transparency for better displaying
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
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
    if (d->m_status != Status::Ready) {
        return;
    }
    d->m_status = Status::Working;

    if (d->m_isDisablingParent) {
        d->m_pTargetWidget->setEnabled(false);
    }

    show();
    pollAnimation();
}

void WaitIndicatorWidget::pauseIndicator()
{
    if (d->m_status & (Status::Paused | Status::Ready)) {
        return;
    }
    d->m_status = Status::Pausing;
    updateVisualState();
    pollAnimation();

    d->m_status = Status::Paused;
    updateVisualState();
}

void WaitIndicatorWidget::continueIndicator()
{
    if (d->m_status != Status::Paused) {
        return;
    }
    d->m_status = Status::Working;
    updateVisualState();
    pollAnimation();
}

void WaitIndicatorWidget::stop()
{
    if (d->m_status & Status::Ready) {
        return;
    }
    d->m_status = Status::Stopping;

    pollAnimation();
    if (d->m_pSecondaryAnimation->state() == QVariantAnimation::Running) {
        d->m_pSecondaryAnimation->stop();
    }

    if (d->m_isDisablingParent) {
        d->m_pTargetWidget->setEnabled(true);
    }
    d->m_status = Status::Ready;
    updateVisualState();
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

void WaitIndicatorWidget::setAnimationEnabled(bool isEn)
{
    d->m_isAnimationEnabled = isEn;
    updateVisualState();
}

bool WaitIndicatorWidget::isAnimationEnabled() const
{
    return d->m_isAnimationEnabled;
}

WaitIndicatorWidget::Status WaitIndicatorWidget::getStatus() const
{
    return d->m_status;
}

void WaitIndicatorWidget::setPercent(double perc)
{
    d->m_currentPercent = perc * 1000;
    updateVisualState();
}

double WaitIndicatorWidget::getCurrentPercent() const
{
    double res {};
    res += d->m_currentPercent / 1000;
    res += 0.001f * (d->m_currentPercent % 1000);
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
    if (text.size() > d->m_titleTextSymbolLimit) {
        d->m_titleText = text.left(d->m_titleTextSymbolLimit);
    } else {
        d->m_titleText = text;
    }
    updateVisualState();
}

void WaitIndicatorWidget::setTitleSymbolLimit(int maxSymbols)
{
    d->m_titleTextSymbolLimit = maxSymbols;
}

QString WaitIndicatorWidget::getTitle() const
{
    return d->m_titleText;
}

void WaitIndicatorWidget::setDescription(const QString &text)
{
    if (text.size() > d->m_descriptionTextSymbolLimit) {
        d->m_descriptionText = text.left(d->m_descriptionTextSymbolLimit);
    } else {
        d->m_descriptionText = text;
    }
    updateVisualState();
}

void WaitIndicatorWidget::setDescriptionSymbolLimit(int maxSymbols)
{
    d->m_descriptionTextSymbolLimit = maxSymbols;
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

void WaitIndicatorWidget::setTextPen(const QPen &textPen)
{
    d->m_textPen = textPen;
    updateVisualState();
}

QPen WaitIndicatorWidget::getTextPen() const
{
    return d->m_textPen;
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
    if (d->m_isAnimationEnabled) {
        if (d->m_pPrimaryAnimation->state() != QVariantAnimation::State::Running) {
            d->m_pPrimaryAnimation->start();
        }

        if (d->m_pSecondaryAnimation->state() != QVariantAnimation::State::Running) {
            d->m_pSecondaryAnimation->start();
        }
    } else {
        d->m_pPrimaryAnimation->stop();
        d->m_pSecondaryAnimation->stop();
    }

    if (d->m_status == Status::Ready) {
        hide();
    } else {
        update();
    }
}

QRect WaitIndicatorWidget::createWorkingRect() const
{
    if (!d->m_pTargetWidget) {
        COMPLOG_WARNING("WaitIndicatorWidget: Called without target");
        return {};
    }

    auto targetRect = d->m_pTargetWidget->rect();
    auto rectCenter = targetRect.center();

    targetRect.setWidth(d->m_size.width() + INTERNAL_RECT_OFFSET * 2);
    targetRect.setHeight(d->m_size.height() + INTERNAL_RECT_OFFSET * 2);
    targetRect.moveCenter(rectCenter);

    return targetRect;
}

QRect WaitIndicatorWidget::getWorkingRect() const
{
    return QRect(INTERNAL_RECT_OFFSET, INTERNAL_RECT_OFFSET, d->m_size.width(), d->m_size.height());
}

void WaitIndicatorWidget::paintTitle() const
{
    d->m_pPainter->save();

    d->m_pPainter->setPen(d->m_textPen);
    auto fnt = d->m_pPainter->font();
    fnt.setBold(true);
    fnt.setPixelSize(14);
    d->m_pPainter->setFont(fnt);

    auto textRect = createWorkingRect();
    textRect.moveTo(textRect.x() - textRect.width() * 1.5, textRect.y() + textRect.height() * 0.8);
    textRect.setWidth(textRect.width() + textRect.width() * 3);
    d->m_pPainter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, d->m_titleText);

    d->m_pPainter->restore();
}

void WaitIndicatorWidget::paintDescription() const
{
    d->m_pPainter->save();

    d->m_pPainter->setPen(d->m_textPen);
    auto fnt = d->m_pPainter->font();
    fnt.setItalic(true);
    fnt.setPixelSize(10);
    d->m_pPainter->setFont(fnt);

    auto textRect = createWorkingRect();
    textRect.moveTo(textRect.x() - textRect.width() * 1.5, textRect.y() + textRect.height() * 1.4);
    textRect.setWidth(textRect.width() + textRect.width() * 3);
    d->m_pPainter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, d->m_descriptionText);

    d->m_pPainter->restore();
}

void WaitIndicatorWidget::paintPercent() const
{
    if (d->m_status & Status::Paused) {
        return;
    }
    d->m_pPainter->save();

    d->m_pPainter->setPen(d->m_primaryPen);
    d->m_pPainter->drawText(getWorkingRect(), Qt::AlignCenter, QString("%1%").arg(getCurrentPercent()));

    d->m_pPainter->restore();
}

void WaitIndicatorWidget::paintCircleIndicator() const
{
    d->m_pPainter->setBrush(Qt::transparent);

    d->m_pPainter->setPen(d->m_secondaryPen);
    d->m_pPainter->drawEllipse(getWorkingRect());

    d->m_pPainter->setPen(d->m_primaryPen);
    d->m_pPainter->drawArc(getWorkingRect(),
                           utilityPieFromDegree(90),
                           -utilityPieFromDegree(3.6f * getCurrentPercent()));
}

double WaitIndicatorWidget::utilityPieFromDegree(const double degree) const
{
    return degree * 16.0f; // See Qt QPainter::drawPie documentation
}

void WaitIndicatorWidget::slot_switchState(const QVariant &animationValue)
{
    // TODO: Process state changing
}

void WaitIndicatorWidget::slot_finishSwitchChange()
{
    // TODO: End up everything about change state
}

void WaitIndicatorWidget::slot_updateSecondary(const QVariant &animationValue)
{
    // TODO: Change secondary animations
}

void WaitIndicatorWidget::slot_finishSecondary()
{
    // TODO: Finish background things
}

void WaitIndicatorWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event); // Draw base
    if (!d->m_pTargetWidget) { return; }

    if (!d->m_pPainter) {
        d->m_pPainter = new QPainter(this);
    }
    if (!d->m_pPainter->isActive()) {
        d->m_pPainter->begin(this);
    }

    // Move to desired draw location
    d->m_pPainter->save();
    auto targetRect = createWorkingRect();
    d->m_pPainter->setClipRect(targetRect);
    d->m_pPainter->translate(targetRect.topLeft());

    // Draw indicator base
    switch (d->m_shape)
    {
    case Shape::Circle:
        paintCircleIndicator();
        break;

    default:
        COMPLOG_WARNING("Unsupported shape type");
    }
    paintPercent();
    d->m_pPainter->restore();

    paintTitle();
    paintDescription();

    // TODO: Draw indicator side-effects

    d->m_pPainter->end();
}

void WaitIndicatorWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (d->m_pTargetWidget) {
        setGeometry(d->m_pTargetWidget->geometry());
    }
}

}