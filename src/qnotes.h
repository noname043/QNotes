#ifndef QNOTES_H
#define QNOTES_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include "ui_qnotes.h"

class QNotes: public QWidget
{
    Q_OBJECT
public:
    QNotes(QWidget *parent = 0);
    ~QNotes();

private:
    Ui::QNotes *_ui;

    QMenu *_menu;
    QAction *_menuAction;
    QAction *_addNoteAction;
    QMenu *_passwordMenu;
    QAction *_enablePasswordAction;
    QAction *_disablePasswordAction;
    QAction *_changePasswordAction;
    QAction *_aboutAction;
    QAction *_exitAction;

    QSettings *_settings;
};

#endif // QNOTES_H
