#include <QApplication>
#include <QMessageBox>
//#include <QInputDialog> <- broken
#include <QTextCodec>
#include "qnotes.h"
#include "getpassworddialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("QNotes");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("Noname043");
    //app.setAttribute(Qt::AA_S60DisablePartialScreenInputMode, false);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QNotes notes;
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    notes.showMaximized();
#else
    notes.show();
#endif

    if (!notes.dbExists() && !notes.createDB())
    {
        QMessageBox::critical(0, QObject::tr("QNotes error"), QObject::tr("Unable to create QNotes database!"));
        return -1;
    }
    else if (!notes.openDB())
    {
        QMessageBox::critical(0, QObject::tr("QNotes error"), QObject::tr("Unable to open QNotes database!"));
        return -1;
    }

    if (notes.hasPassword())
    {
        QString password = GetPasswordDialog::getPassword(&notes);
        if (password.isNull())
            return 0;
        if (!notes.checkPassword(password))
        {
            QMessageBox::critical(0, QObject::tr("QNotes error"), QObject::tr("Wrong password!"));
            return -1;
        }
    }

    notes.loadNotes();

    return app.exec();
}
