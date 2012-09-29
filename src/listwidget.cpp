#include "listwidget.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <qmath.h>

ListWidget::ListWidget(QWidget *parent) :
    QListWidget(parent),
    _mousePressed(false)
{
    _scrollBar = verticalScrollBar();
}

void ListWidget::mousePressEvent(QMouseEvent *ev)
{
    _mousePressed = true;
    _lastHeight = ev->y();
    _startPoint = ev->pos();
    QListWidget::mousePressEvent(ev);
}

void ListWidget::mouseMoveEvent(QMouseEvent *ev)
{
    if (_mousePressed)
    {
        //scrollContentsBy(0, ev->y() - _lastHeight);
        _scrollBar->setValue(_scrollBar->value() - ev->y() + _lastHeight);
        _lastHeight = ev->y();
    }
    //QListWidget::mouseMoveEvent(ev);
    ev->accept();
}

void ListWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    _mousePressed = false;
    int distance = qSqrt(qPow(ev->x() - _startPoint.x(), 2) + qPow(ev->y() - _startPoint.y(), 2));
    if (distance <= 10.0)
        QListWidget::mouseReleaseEvent(ev);
    else ev->accept();
}

void ListWidget::leaveEvent(QEvent *)
{
    _mousePressed = false;
}
