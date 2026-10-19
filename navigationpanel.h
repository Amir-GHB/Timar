#ifndef NAVIGATIONPANEL_H
#define NAVIGATIONPANEL_H

#include <QFrame>
#include <qevent.h>

class QVBoxLayout;
class QPropertyAnimation;

class NavigationPanel : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(int panelWidth READ panelWidth WRITE setPanelWidth)

public:
    explicit NavigationPanel(QWidget* parent = nullptr);

    void addNavigationButton(QWidget* button);

    int panelWidth() const;
    void setPanelWidth(int width);

signals:
    void expandedChanged(bool expanded);

protected:
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void expand();
    void collapse();

private:
    QVBoxLayout* m_layout = nullptr;
    QPropertyAnimation* m_animation = nullptr;

    int m_collapsedWidth = 60;
    int m_expandedWidth = 155;

    bool m_isExpanded = false;
};

#endif