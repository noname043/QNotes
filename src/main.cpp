#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include "qnotes.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("QNotes");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("Noname043");

    QNotes notes;
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
        QString password = QInputDialog::getText(0, QObject::tr("Enter password"), QObject::tr("Password:"), QLineEdit::Password);
        if (password.isNull())
            return 0;
        if (!notes.checkPassword(password))
        {
            QMessageBox::critical(0, QObject::tr("QNotes error"), QObject::tr("Wrong password!"));
            return -1;
        }
    }
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    notes.showMaximized();
#else
    notes.show();
#endif

    return app.exec();
}
