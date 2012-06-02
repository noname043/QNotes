#ifndef NOTEEDITOR_H
#define NOTEEDITOR_H

#include <QDialog>
#include <QAction>
#include "ui_noteeditor.h"
#include "note.h"

class NoteEditor: public QDialog
{
    Q_OBJECT
public:
    NoteEditor(Note *note, QWidget *parent = 0);
    ~NoteEditor();

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    void show() { showMaximized(); }
#endif

private slots:
    void updateNote();

private:
    Ui::NoteEditor *_ui;
    Note *_note;
    QAction *_cancelAction;
    QAction *_saveAction;
};

#endif // NOTEEDITOR_H
