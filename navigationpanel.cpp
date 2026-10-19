#include "navigationpanel.h"

#include <QEnterEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QVBoxLayout>
#include <QSizePolicy>

NavigationPanel::NavigationPanel(QWidget* parent)
    : QFrame(parent)
{
    setFrameShape(QFrame::NoFrame);

    setFixedWidth(m_collapsedWidth);

    setSizePolicy(
        QSizePolicy::Fixed,
        QSizePolicy::Expanding
        );

    setStyleSheet(
        "NavigationPanel {"
        "    background-color: #e3e3e3;"
        "}"
        );

    m_layout = new QVBoxLayout(this);

    m_layout->setContentsMargins(10, 20, 10, 20);
    m_layout->setSpacing(6);

    m_animation = new QPropertyAnimation(
        this,
        "panelWidth",
        this
        );

    m_animation->setDuration(160);
    m_animation->setEasingCurve(
        QEasingCurve::InOutCubic
        );
}

void NavigationPanel::addNavigationButton(QWidget* button)
{
    if (!button)
        return;

    m_layout->addWidget(button);
}

int NavigationPanel::panelWidth() const
{
    return width();
}

void NavigationPanel::setPanelWidth(int width)
{
    setFixedWidth(width);
}

void NavigationPanel::enterEvent(QEvent* event)
{
    QFrame::enterEvent(event);
    expand();
}

void NavigationPanel::leaveEvent(QEvent* event)
{
    QFrame::leaveEvent(event);

    collapse();
}

void NavigationPanel::expand()
{
    if (m_isExpanded)
        return;

    m_isExpanded = true;

    m_animation->stop();

    m_animation->setStartValue(width());
    m_animation->setEndValue(m_expandedWidth);

    m_animation->start();

    emit expandedChanged(true);
}

void NavigationPanel::collapse()
{
    if (!m_isExpanded)
        return;

    m_isExpanded = false;

    m_animation->stop();

    m_animation->setStartValue(width());
    m_animation->setEndValue(m_collapsedWidth);

    m_animation->start();

    emit expandedChanged(false);
}