#include <QApplication>
#include "qnotes.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("QNotes");
    app.setApplicationVersion("v0.1");
    app.setOrganizationName("Noname043");

    QNotes notes;
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    notes.showMaximized();
#else
    notes.show();
#endif

    return app.exec();
}
