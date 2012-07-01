#include "getpassworddialog.h"
#include <QS60Style>

GetPasswordDialog::GetPasswordDialog(const QString &caption, const QString &label, QWidget *parent):
    QDialog(parent),
    _ui(new Ui::GetPasswordDialog),
    _cancelAction(new QAction(this)),
    _okAction(new QAction(this))
{
    _ui->setupUi(this);
    this->setWindowTitle(caption);
    _ui->label->setText(label);

    _cancelAction->setText(tr("Cancel"));
    _okAction->setText(tr("Ok"));
    _cancelAction->setIcon(qApp->style()->standardIcon(static_cast<QStyle::StandardPixmap>(SP_CustomToolBarBack)));
    _okAction->setIcon(qApp->style()->standardIcon(static_cast<QStyle::StandardPixmap>(SP_CustomToolBarDone)));
    addAction(_cancelAction);
    addAction(_okAction);
    _cancelAction->setSoftKeyRole(QAction::NegativeSoftKey);
    _okAction->setSoftKeyRole(QAction::PositiveSoftKey);
    connect(_cancelAction, SIGNAL(triggered()), this, SLOT(reject()));
    connect(_okAction, SIGNAL(triggered()), this, SLOT(accept()));
}

GetPasswordDialog::~GetPasswordDialog()
{}

QString GetPasswordDialog::getPassword(QWidget *parent, const QString &caption, const QString &label)
{
    GetPasswordDialog *dialog = new GetPasswordDialog(caption, label, parent);
    QString tmp;
    if (dialog->exec() == QDialog::Accepted)
        tmp = dialog->password();
    delete dialog;
    return tmp;
}
