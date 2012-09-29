#include "src/notelist.h"

NoteList::NoteList(QWidget *parent):
    ListWidget(parent),
    _menu(new QMenu(this)),
    _editAction(new QAction(tr("Edit"), this)),
    _deleteAction(new QAction(tr("Delete"), this)),
    _detailsAction(new QAction(tr("Details"), this))
{
    _menu->addAction(_editAction);
    _menu->addSeparator();
    _menu->addAction(_deleteAction);
    _menu->addSeparator();
    _menu->addAction(_detailsAction);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

    this->setSortingEnabled(true);
    this->setVerticalScrollMode(QListWidget::ScrollPerPixel);
}

NoteList::~NoteList()
{}

void NoteList::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = itemAt(pos);
    if (!item)
        return;

    QAction *action = _menu->exec(pos);
    if (action == _editAction)
        emit editTriggered(item);
    else if (action == _deleteAction)
        emit deleteTriggered(item);
    else if (action == _detailsAction)
        emit detailsTriggered(item);
}
