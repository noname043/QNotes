#include "noteeditor.h"
#include <QStyle>
#ifdef Q_WS_SIMULATOR
#include <QPushButton>
#endif

NoteEditor::NoteEditor(Note *note, QWidget *parent):
    QDialog(parent),
    _ui(new Ui::NoteEditor),
    _note(note),
    _cancelAction(new QAction(qApp->style()->standardIcon(QStyle::SP_ArrowBack), tr("Cancel"), this)),
    _saveAction(new QAction(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Save"), this))
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

    // Workaround for QtSimulator
#ifdef Q_WS_SIMULATOR
    QPushButton *button = new QPushButton(tr("Cancel"), this);
    _ui->verticalLayout->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(reject()));
    button = new QPushButton(tr("Save"), this);
    _ui->verticalLayout->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));
#endif
}

NoteEditor::~NoteEditor()
{}

// TODO: _note->_modified on accepted()
void NoteEditor::updateNote()
{
    _note->setTitle(_ui->title->text());
    _note->setContent(_ui->content->toPlainText());
    _note->setModified(QDateTime::currentDateTime().toTime_t());
}
