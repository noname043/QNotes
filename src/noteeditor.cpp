#include "noteeditor.h"
#include <QS60Style>

NoteEditor::NoteEditor(Note *note, QWidget *parent):
    QDialog(parent),
    _ui(new Ui::NoteEditor),
    _note(note),
    _cancelAction(new QAction(qApp->style()->standardIcon(static_cast<QStyle::StandardPixmap>(SP_CustomToolBarBack)), tr("Cancel"), this)),
    _saveAction(new QAction(qApp->style()->standardIcon(static_cast<QStyle::StandardPixmap>(SP_CustomToolBarDone)), tr("Save"), this))
{
    _ui->setupUi(this);
    _ui->title->setText(_note->title());
    _ui->content->setPlainText(_note->content());

    addAction(_cancelAction);
    addAction(_saveAction);
    _cancelAction->setSoftKeyRole(QAction::NegativeSoftKey);
    _saveAction->setSoftKeyRole(QAction::PositiveSoftKey);
    connect(_cancelAction, SIGNAL(triggered()), this, SLOT(reject()));
    connect(_saveAction, SIGNAL(triggered()), this, SLOT(accept()));

    connect(this, SIGNAL(accepted()), this, SLOT(updateNote()));
}

NoteEditor::~NoteEditor()
{}

void NoteEditor::updateNote()
{
    _note->setTitle(_ui->title->text());
    _note->setContent(_ui->content->toPlainText());
    _note->setModified(QDateTime::currentDateTime().toTime_t());
}
