#ifndef QNOTES_H
#define QNOTES_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QSqlDatabase>
#include <QFile>
#include <QSettings>
#include "note.h"
#include "notelist.h"
#include "ui_qnotes.h"

#define DBFILE "QNotes.db"
#define DBPATH "DBPath"

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

private slots:
    void addNote();
    void editNote(QListWidgetItem *item);
    void enablePassword();
    void changePassword();
    void disablePassword();

private:
    void encrypt(Note *note);
    void decrypt(Note *note);
    void updateNotes();
    static void encrypt(QByteArray &data, const char *key);
    static void decrypt(QByteArray &data, const char *key) { encrypt(data, key); }

private:
    Ui::QNotes *_ui;
    NoteList *_noteList;

    QMenu *_menu;
    QAction *_menuAction;
    QAction *_addNoteAction;
    QMenu *_passwordMenu;
    QAction *_enablePasswordAction;
    QAction *_changePasswordAction;
    QAction *_disablePasswordAction;
    QAction *_aboutAction;
    QAction *_exitAction;

    QSettings _settings;
    QSqlDatabase _db;
    bool _hasPassword;
    QString _password;
    QString _hash;
};

#endif // QNOTES_H
