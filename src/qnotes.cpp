#include "src/qnotes.h"
#include "ui_qnotes.h"

QNotes::QNotes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QNotes)
{
    ui->setupUi(this);
}

QNotes::~QNotes()
{
    delete ui;
}
