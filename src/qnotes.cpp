#include "qnotes.h"
#include <QStyle>
#include <QSqlQuery>
#include <QSqlResult>
#include <QCryptographicHash>
#include <QMessageBox>
#ifdef Q_WS_SIMULATOR
#include <QPushButton>
#endif
#include "noteeditor.h"
#include "passworddialog.h"

QNotes::QNotes(QWidget *parent):
    QWidget(parent),
    _ui(new Ui::QNotes),
    _menu(new QMenu(tr("Menu"), this)),
    _menuAction(new QAction(tr("Menu"), this)),
    _addNoteAction(new QAction(tr("Add note"), this)),
    _passwordMenu(new QMenu(tr("Password"), this)),
    _enablePasswordAction(new QAction(tr("Enable"), this)),
    _changePasswordAction(new QAction(tr("Change"), this)),
    _disablePasswordAction(new QAction(tr("Disable"), this)),
    _aboutAction(new QAction(tr("About"), this)),
    _exitAction(new QAction(tr("Exit"), this)),
    _db(QSqlDatabase::addDatabase("QSQLITE")),
    _hasPassword(false)
{
    _ui->setupUi(this);

    _disablePasswordAction->setDisabled(true);
    _changePasswordAction->setDisabled(true);

    _passwordMenu->addAction(_enablePasswordAction);
    _passwordMenu->addAction(_changePasswordAction);
    _passwordMenu->addAction(_disablePasswordAction);
    _menu->addAction(_addNoteAction);
    _menu->addMenu(_passwordMenu);
    _menu->addAction(_aboutAction);
    _menu->addAction(_exitAction);
    addAction(_menuAction);
    _menuAction->setMenu(_menu);
    _menuAction->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    _menuAction->setSoftKeyRole(QAction::PositiveSoftKey);

    // Workaround for QtSimulator
#ifdef Q_WS_SIMULATOR
    QPushButton *button = new QPushButton(tr("Menu"), this);
    button->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    button->setMenu(_menu);
#endif

    connect(_addNoteAction, SIGNAL(triggered()), this, SLOT(addNote()));
    connect(_enablePasswordAction, SIGNAL(triggered()), this, SLOT(enablePassword()));
    connect(_changePasswordAction, SIGNAL(triggered()), this, SLOT(changePassword()));
    connect(_disablePasswordAction, SIGNAL(triggered()), this, SLOT(disablePassword()));
    connect(_exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(_ui->notesList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(editNote(QListWidgetItem*)));

    _db.setDatabaseName(DBFILE);
}

QNotes::~QNotes()
{
    if (_db.isOpen())
        _db.close();
}

bool QNotes::createDB()
{
    if (!_db.open())
        return false;

    QSqlQuery q;
    q.exec("CREATE TABLE DBInfo (appName varchar(20), appVersion float, isPasswordEnabled boolean, password varchar(40));");
    q.exec(QString("INSERT INTO DBInfo VALUES('%1', %2, 'false', '');").arg(qApp->applicationName(), qApp->applicationVersion()));
    q.exec("CREATE TABLE Notes (id int primary key, title varchar(60), content text, created int, modified int);");

    _db.close();
    return true;
}

bool QNotes::openDB()
{
    if (!_db.open())
        return false;

    QSqlQuery q;
    if (!q.exec("SELECT appName, isPasswordEnabled, password FROM DBInfo") ||
        !q.next() || q.value(0).toString() != qApp->applicationName())
    {
        return false;
    }

    _hasPassword = q.value(1).toBool();
    _hash = q.value(2).toString();

    _enablePasswordAction->setDisabled(_hasPassword);
    _changePasswordAction->setEnabled(_hasPassword);
    _disablePasswordAction->setEnabled(_hasPassword);

    return true;
}

bool QNotes::checkPassword(const QString &password)
{
    if (_hasPassword)
        return _hash == QCryptographicHash::hash(QByteArray(password.toStdString().c_str()), QCryptographicHash::Sha1).toHex();
    return true;
}

void QNotes::loadNotes()
{
    QSqlQuery q;
    Note *note;
    q.exec("SELECT id, title, content, created, modified FROM Notes ORDER BY title");
    while (q.next())
    {
        note = new Note;
        note->setId(q.value(0).toInt());
        note->setTitle(q.value(1).toString());
        note->setContent(q.value(2).toString());
        note->setCreated(q.value(3).toInt());
        note->setModified(q.value(4).toInt());
        // TODO: encryption, hex->strings
        _ui->notesList->addItem(note);
    }
}

void QNotes::editNote(QListWidgetItem *item)
{
    Note *note = dynamic_cast<Note*>(item);
    NoteEditor *editor = new NoteEditor(note, this);
    editor->exec();
    if (editor->result() == QDialog::Accepted)
    {
        QSqlQuery q;
        // TODO: encryption, strings->hex
        q.exec(QString("UPDATE Notes SET title='%1', content='%2', modified=%3 WHERE id=%4")
               .arg(note->title(), note->content(), QString::number(note->modifiedTime()), QString::number(note->id())));
    }
    delete editor;
}

void QNotes::addNote()
{
    Note *note = new Note;
    note->setCreated(QDateTime::currentDateTime().toTime_t());
    NoteEditor *editor = new NoteEditor(note, this);
    editor->exec();
    if (editor->result() == QDialog::Accepted)
    {
        _ui->notesList->addItem(note);
        QSqlQuery q;
        // TODO: encryption, strings->hex  v workaround for auto_increment, lol
        q.exec("SELECT Count(1)+1 FROM Notes");
        q.next();
        note->setId(q.value(0).toInt());
        q.exec(QString("INSERT INTO Notes VALUES (%1, '%2', '%3', %4, %5)")
               .arg(QString::number(note->id()), note->title(), note->content(),
                    QString::number(note->createdTime()), QString::number(note->modifiedTime())));
    }
    else delete note;
    delete editor;
}

void QNotes::enablePassword()
{
    PasswordDialog *dialog = new PasswordDialog(this);
    dialog->exec();
    if (dialog->result() == QDialog::Accepted)
    {
        _password = dialog->password();
        _hash = QCryptographicHash::hash(_password.toStdString().c_str(), QCryptographicHash::Sha1).toHex();
        _hasPassword = true;

        QSqlQuery q;
        q.exec(QString("UPDATE DBInfo SET isPasswordEnabled='true', password='%1'").arg(_hash));

        // TODO: encrypt notes

        _enablePasswordAction->setDisabled(true);
        _changePasswordAction->setEnabled(true);
        _disablePasswordAction->setEnabled(true);
    }
    delete dialog;
}

void QNotes::changePassword()
{
    PasswordDialog *dialog = new PasswordDialog(this);
    dialog->exec();
    if (dialog->result() == QDialog::Accepted)
    {
        QString newPassword = dialog->password();
        _hash = QCryptographicHash::hash(newPassword.toStdString().c_str(), QCryptographicHash::Sha1).toHex();

        // TODO: "recrypt" notes

        _password = newPassword;
        QSqlQuery q;
        q.exec(QString("UPDATE DBInfo SET password='%1'").arg(_hash));
    }
    delete dialog;
}

void QNotes::disablePassword()
{
    if (QMessageBox::question(this, tr("QNotes"), tr("Do you really want to disable your password?"), QMessageBox::Yes, QMessageBox::No) ==
            QMessageBox::Yes)
    {
        // TODO: decrypt notes
        _hasPassword = false;
        QSqlQuery q;
        q.exec("UPDATE DBInfo SET isPasswordEnabled='false'");
        _enablePasswordAction->setEnabled(true);
        _changePasswordAction->setDisabled(true);
        _disablePasswordAction->setDisabled(true);
    }
}
