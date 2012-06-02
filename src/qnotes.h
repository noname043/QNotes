#ifndef QNOTES_H
#define QNOTES_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QSqlDatabase>
#include <QFile>
#include "ui_qnotes.h"

#define DBFILE "QNotes.db"

class QNotes: public QWidget
{
    Q_OBJECT
public:
    QNotes(QWidget *parent = 0);
    ~QNotes();

    bool hasPassword() const { return _hasPassword; }
    bool checkPassword(const QString &password);

    bool dbExists() const { return QFile::exists(DBFILE); }
    bool createDB();
    bool openDB();

    void loadNotes();

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

    QSqlDatabase _db;
    bool _hasPassword;
    QString _password;
    QString _hash;
};

#endif // QNOTES_H
