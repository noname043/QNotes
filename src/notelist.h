#ifndef NOTELIST_H
#define NOTELIST_H

#include <QListWidget>
#include <QMenu>
#include <QAction>

class NoteList: public QListWidget
{
    Q_OBJECT
public:
    NoteList(QWidget *parent = 0);
    ~NoteList();
    
private slots:
    void showContextMenu(const QPoint &pos);

signals:
    void editTriggered(QListWidgetItem *item);
    void deleteTriggered(QListWidgetItem *item);
    void detailsTriggered(QListWidgetItem *item);

private:
    QMenu *_menu;
    QAction *_editAction;
    QAction *_deleteAction;
    QAction *_detailsAction;
};

#endif // NOTELIST_H
