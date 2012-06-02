#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QAction>
#include "ui_passworddialog.h"

class PasswordDialog: public QDialog
{
    Q_OBJECT
public:
    PasswordDialog(QWidget *parent = 0);
    ~PasswordDialog();

    QString password() const { return _ui->password->text(); }
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    void show() { showMaximized(); }
#endif

private slots:
    bool passwordChanged(const QString &text);
    bool retypeChanged(const QString &text);

private:
    Ui::PasswordDialog *_ui;
    QAction *_cancelAction;
    QAction *_okAction;
};

#endif // PASSWORDDIALOG_H
