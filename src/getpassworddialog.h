#ifndef GETPASSWORDDIALOG_H
#define GETPASSWORDDIALOG_H

#include <QAction>
#include "ui_getpassworddialog.h"

class GetPasswordDialog: public QDialog
{
    Q_OBJECT
public:
    GetPasswordDialog(const QString &caption, const QString &label, QWidget *parent = 0);
    ~GetPasswordDialog();

    static QString getPassword(QWidget *parent = 0, const QString &caption = "QNotes", const QString &label = "Password:");

    QString password() const { return _ui->password->text(); }

private:
    Ui::GetPasswordDialog *_ui;
    QAction *_cancelAction;
    QAction *_okAction;
};

#endif // GETPASSWORDDIALOG_H
