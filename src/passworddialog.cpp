#include "passworddialog.h"
#include <QStyle>

PasswordDialog::PasswordDialog(QWidget *parent):
    QDialog(parent),
    _ui(new Ui::PasswordDialog),
    _cancelAction(new QAction(qApp->style()->standardIcon(QStyle::SP_ArrowBack), tr("Cancel"), this)),
    _okAction(new QAction(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Ok"), this))
{
    _ui->setupUi(this);

    addAction(_cancelAction);
    addAction(_okAction);
    _cancelAction->setSoftKeyRole(QAction::NegativeSoftKey);
    _okAction->setSoftKeyRole(QAction::PositiveSoftKey);
    _okAction->setDisabled(true);

    connect(_cancelAction, SIGNAL(triggered()), this, SLOT(reject()));
    connect(_okAction, SIGNAL(triggered()), this, SLOT(accept()));
    connect(_ui->password, SIGNAL(textEdited(QString)), this, SLOT(passwordChanged(QString)));
    connect(_ui->retype, SIGNAL(textEdited(QString)), this, SLOT(retypeChanged(QString)));

    // Workaround for QtSimulator.
#ifdef Q_WS_SIMULATOR
    _button = new QPushButton(tr("Cancel"), this);
    _ui->verticalLayout_3->addWidget(_button);
    connect(_button, SIGNAL(clicked()), this, SLOT(reject()));
    _button = new QPushButton(tr("Ok"), this);
    _ui->verticalLayout_3->addWidget(_button);
    connect(_button, SIGNAL(clicked()), this, SLOT(accept()));
    _button->setEnabled(false);
#endif
}

PasswordDialog::~PasswordDialog()
{}

bool PasswordDialog::passwordChanged(const QString &text)
{
    if (text.length() < 5)
    {
        _ui->status->setText(tr("Password too short."));
        return false;
    }
    _ui->status->setText("");
    return true;
}

bool PasswordDialog::retypeChanged(const QString &text)
{
    bool result;
    if (!passwordChanged(_ui->password->text()))
        result = false;
    else if (text != _ui->password->text())
    {
        _ui->status->setText(tr("Passwords do not match."));
        result = false;
    }
    else result = true;
    _okAction->setEnabled(result);
#ifdef Q_WS_SIMULATOR
    _button->setEnabled(result);
#endif
    return result;
}
