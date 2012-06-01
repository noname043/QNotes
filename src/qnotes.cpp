#include "qnotes.h"
#include <QStyle>
#include <QSqlQuery>
#include <QSqlResult>
#include <QCryptographicHash>
#ifdef Q_WS_SIMULATOR
#include <QPushButton>
#endif

QNotes::QNotes(QWidget *parent):
    QWidget(parent),
    _ui(new Ui::QNotes),
    _menu(new QMenu(tr("Menu"), this)),
    _menuAction(new QAction(tr("Menu"), this)),
    _addNoteAction(new QAction(tr("Add note"), this)),
    _passwordMenu(new QMenu(tr("Password"), this)),
    _enablePasswordAction(new QAction(tr("Enable"), this)),
    _disablePasswordAction(new QAction(tr("Disable"), this)),
    _changePasswordAction(new QAction(tr("Change"), this)),
    _aboutAction(new QAction(tr("About"), this)),
    _exitAction(new QAction(tr("Exit"), this)),
    _db(QSqlDatabase::addDatabase("QSQLITE"))
{
    _ui->setupUi(this);

    _passwordMenu->addAction(_enablePasswordAction);
    _passwordMenu->addAction(_disablePasswordAction);
    _passwordMenu->addAction(_changePasswordAction);
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
    q.exec("CREATE TABLE Notes (id int primary key, title varchar(60), content text, created varchar(16), modified varchar(16));");

    return true;
}

bool QNotes::openDB()
{
    if (!_db.open())
        return false;

    QSqlQuery q;
    if (!q.exec("SELECT appName, isPasswordEnabled, password FROM DBInfo") ||
            q.value(1).toString() != qApp->applicationName())
    {
        return false;
    }

    _hasPassword = q.value(2).toBool();
    _hash = q.value(3).toString();

    return true;
}

bool QNotes::checkPassword(const QString &password)
{
    if (_hasPassword)
        return _hash == QCryptographicHash::hash(QByteArray(password.toStdString().c_str()), QCryptographicHash::Sha1);
    return true;
}
