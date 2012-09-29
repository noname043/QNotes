#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>

//! Replacement for QListWidget.
class ListWidget: public QListWidget
{
    Q_OBJECT
public:
    ListWidget(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void leaveEvent(QEvent *);

private:
    bool _mousePressed;
    int _lastHeight;
    QScrollBar *_scrollBar;
    QPoint _startPoint;
};

#endif // LISTWIDGET_H
