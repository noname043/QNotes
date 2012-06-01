#include "qnotes.h"
#include <QStyle>
#ifdef Q_WS_SIMULATOR
#include <QPushButton>
#endif

QNotes::QNotes(QWidget *parent):
    QWidget(parent),
    _ui(new Ui::QNotes),
    _menu(new QMenu(tr("Menu"), this)),
    _menuAction(new QAction(tr("Menu"), this)),
    _addNoteAction(new QAction(tr("Add note"), this)),
    _passwordMenu(new QMenu(tr("Password"), this)),
    _enablePasswordAction(new QAction(tr("Enable"), this)),
    _disablePasswordAction(new QAction(tr("Disable"), this)),
    _changePasswordAction(new QAction(tr("Change"), this)),
    _aboutAction(new QAction(tr("About"), this)),
    _exitAction(new QAction(tr("Exit"), this))
{
    _ui->setupUi(this);

    _passwordMenu->addAction(_enablePasswordAction);
    _passwordMenu->addAction(_disablePasswordAction);
    _passwordMenu->addAction(_changePasswordAction);
    _menu->addAction(_addNoteAction);
    _menu->addMenu(_passwordMenu);
    _menu->addAction(_aboutAction);
    _menu->addAction(_exitAction);
    addAction(_menuAction);
    _menuAction->setMenu(_menu);
    // TODO: options icon
    _menuAction->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowUp));
    _menuAction->setSoftKeyRole(QAction::PositiveSoftKey);

    // Workaround for QtSimulator
#ifdef Q_WS_SIMULATOR
    QPushButton *button = new QPushButton(tr("Menu"), this);
    button->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowUp));
    button->setMenu(_menu);
#endif
}

QNotes::~QNotes()
{}
