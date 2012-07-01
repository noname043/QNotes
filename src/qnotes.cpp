#include "qnotes.h"
#include <QStyle>
#include <QSqlQuery>
#include <QSqlResult>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QFileDialog>
#include "noteeditor.h"
#include "passworddialog.h"

QNotes::QNotes(QWidget *parent):
    QWidget(parent),
    _ui(new Ui::QNotes),
    _noteList(new NoteList(this)),
    _menu(new QMenu(tr("Menu"), this)),
    _menuAction(new QAction(tr("Menu"), this)),
    _addNoteAction(new QAction(tr("Add note"), this)),
    _passwordMenu(new QMenu(tr("Password"), this)),
    _enablePasswordAction(new QAction(tr("Enable"), this)),
    _changePasswordAction(new QAction(tr("Change"), this)),
    _disablePasswordAction(new QAction(tr("Disable"), this)),
    _aboutAction(new QAction(tr("About"), this)),
    _exitAction(new QAction(tr("Exit"), this)),
    _settings(this),
    _db(QSqlDatabase::addDatabase("QSQLITE")),
    _hasPassword(false)
{
    _ui->setupUi(this);
    _ui->gridLayout->addWidget(_noteList);

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

    connect(_addNoteAction, SIGNAL(triggered()), this, SLOT(addNote()));
    connect(_enablePasswordAction, SIGNAL(triggered()), this, SLOT(enablePassword()));
    connect(_changePasswordAction, SIGNAL(triggered()), this, SLOT(changePassword()));
    connect(_disablePasswordAction, SIGNAL(triggered()), this, SLOT(disablePassword()));
    connect(_exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(_noteList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(editNote(QListWidgetItem*)));
    connect(_noteList, SIGNAL(editTriggered(QListWidgetItem*)), this, SLOT(editNote(QListWidgetItem*)));
    connect(_noteList, SIGNAL(deleteTriggered(QListWidgetItem*)), this, SLOT(deleteNote(QListWidgetItem*)));
    connect(_noteList, SIGNAL(detailsTriggered(QListWidgetItem*)), this, SLOT(showDetails(QListWidgetItem*)));

    QString tmp;
    if (_settings.contains(DBPATH))
        tmp = _settings.value(DBPATH).toString();
    else
    {
        QMessageBox::information(this, "QNotes", tr("It's the first time you use QNotes. After this message you will be asked to choose directory where you want to keep your notes."));
        tmp = QFileDialog::getExistingDirectory(this, tr("Choose directory where you want to keep your notes."));
        if (tmp.isEmpty())
            tmp = QDir::homePath();
        _settings.setValue(DBPATH, tmp);
    }
    if (!tmp.endsWith('/'))
        tmp += '/';
    tmp += DBFILE;
    _db.setDatabaseName(QDir::toNativeSeparators(tmp));
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
    q.exec("CREATE TABLE Notes (id int primary key, title varchar(240), content text, created int, modified int);");

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
    {
        if (_hash == QCryptographicHash::hash(QByteArray(password.toStdString().c_str()), QCryptographicHash::Sha1).toHex())
        {
            _password = password;
            return true;
        }
        else return false;
    }
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
        decrypt(note);
        _noteList->addItem(note);
    }
}

void QNotes::editNote(QListWidgetItem *item)
{
    Note *note = dynamic_cast<Note*>(item);
    NoteEditor *editor = new NoteEditor(note, this);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    editor->showMaximized();
#endif
    editor->exec();
    if (editor->result() == QDialog::Accepted)
    {
        if (note->title().isEmpty())
            note->setTitle(tr("Note #%1").arg(QString::number(note->id())));
        QSqlQuery q;
        encrypt(note);
        q.exec(QString("UPDATE Notes SET title='%1', content='%2', modified=%3 WHERE id=%4")
               .arg(note->title(), note->content(), QString::number(note->modifiedTime()), QString::number(note->id())));
        decrypt(note);
    }
    delete editor;
}

void QNotes::addNote()
{
    Note *note = new Note;
    note->setCreated(QDateTime::currentDateTime().toTime_t());
    NoteEditor *editor = new NoteEditor(note, this);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    editor->showMaximized();
#endif
    editor->exec();
    if (editor->result() == QDialog::Accepted)
    {
        _noteList->addItem(note);
        QSqlQuery q;
        //               v--- workaround for auto_increment, lol
        q.exec("SELECT Count(1)+1 FROM Notes");
        q.next();
        note->setId(q.value(0).toInt());
        if (note->title().isEmpty())
            note->setTitle(tr("Note #%1").arg(QString::number(note->id())));
        encrypt(note);
        q.exec(QString("INSERT INTO Notes VALUES (%1, '%2', '%3', %4, %5)")
               .arg(QString::number(note->id()), note->title(), note->content(),
                    QString::number(note->createdTime()), QString::number(note->modifiedTime())));
        decrypt(note);
    }
    else delete note;
    delete editor;
}

void QNotes::enablePassword()
{
    PasswordDialog *dialog = new PasswordDialog(this);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    dialog->showMaximized();
#endif
    dialog->exec();
    if (dialog->result() == QDialog::Accepted)
    {
        _password = dialog->password();
        _hash = QCryptographicHash::hash(_password.toStdString().c_str(), QCryptographicHash::Sha1).toHex();
        _hasPassword = true;

        QSqlQuery q;
        q.exec(QString("UPDATE DBInfo SET isPasswordEnabled='true', password='%1'").arg(_hash));

        updateNotes();

        _enablePasswordAction->setDisabled(true);
        _changePasswordAction->setEnabled(true);
        _disablePasswordAction->setEnabled(true);
    }
    delete dialog;
}

void QNotes::changePassword()
{
    PasswordDialog *dialog = new PasswordDialog(this);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    dialog->showMaximized();
#endif
    dialog->exec();
    if (dialog->result() == QDialog::Accepted)
    {
        _password = dialog->password();
        _hash = QCryptographicHash::hash(_password.toStdString().c_str(), QCryptographicHash::Sha1).toHex();

        updateNotes();

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
        _hasPassword = false;
        QSqlQuery q;
        q.exec("UPDATE DBInfo SET isPasswordEnabled='false'");

        updateNotes();

        _enablePasswordAction->setEnabled(true);
        _changePasswordAction->setDisabled(true);
        _disablePasswordAction->setDisabled(true);
    }
}

void QNotes::encrypt(QByteArray &data, const char *key)
{
    const char *k1 = key;
    int k1len = (strlen(key)%2 ? strlen(key)/2 : strlen(key)/2+1);
    const char *k2 = &key[k1len];
    int k2len = strlen(key) - k1len;

    for (int i = 0, j = 0, k = 0; i < data.size(); ++i, ++j, ++k)
    {
        if (j == k1len)
            j = 0;
        if (k == k2len)
            k = 0;
        data[i] = data[i] ^ k1[j];
        data[i] = data[i] ^ k2[k];
    }
}

void QNotes::decrypt(Note *note)
{
    QByteArray tmp;

    tmp = QByteArray::fromHex(note->title().toStdString().c_str());
    if (_hasPassword)
        decrypt(tmp, _password.toStdString().c_str());
    note->setTitle(tmp);

    tmp = QByteArray::fromHex(note->content().toStdString().c_str());
    if (_hasPassword)
        decrypt(tmp, _password.toStdString().c_str());
    note->setContent(tmp);
}

void QNotes::encrypt(Note *note)
{
    QByteArray tmp;

    tmp = note->text().toStdString().c_str();
    if (_hasPassword)
        encrypt(tmp, _password.toStdString().c_str());
    note->setTitle(tmp.toHex());

    tmp = note->content().toStdString().c_str();
    if (_hasPassword)
        encrypt(tmp, _password.toStdString().c_str());
    note->setContent(tmp.toHex());
}

void QNotes::updateNotes()
{
    QSqlQuery q;
    Note *note;
    for (int i = 0; i < _noteList->count(); ++i)
    {
        // TODO: does it count from 1 or 0?
        note = dynamic_cast<Note*>(_noteList->item(i));
        encrypt(note);
        q.exec(QString("UPDATE Notes SET title='%1', content='%2' WHERE id=%3")
               .arg(note->title(), note->content(), QString::number(note->id())));
        decrypt(note);
    }
}

void QNotes::deleteNote(QListWidgetItem *item)
{
    if (QMessageBox::question(this, "QNotes", tr("Do you really want to delete this note?"), QMessageBox::Yes, QMessageBox::No)
            == QMessageBox::Yes)
    {
        Note *note = dynamic_cast<Note*>(item);
        _noteList->removeItemWidget(item);
        QSqlQuery q;
        q.exec(QString("DELETE FROM Notes WHERE id=%1").arg(QString::number(note->id())));
        delete note;
    }
}

void QNotes::showDetails(QListWidgetItem *item)
{
    Note *note = dynamic_cast<Note*>(item);
    QMessageBox::information(this, "QNotes", QString("Created: %1<br>Modified: %2").arg(note->createdString(), note->modifiedString()));
}
