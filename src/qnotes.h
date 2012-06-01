#ifndef QNOTES_H
#define QNOTES_H

#include <QWidget>

namespace Ui {
class QNotes;
}

class QNotes : public QWidget
{
    Q_OBJECT
    
public:
    explicit QNotes(QWidget *parent = 0);
    ~QNotes();
    
private:
    Ui::QNotes *ui;
};

#endif // QNOTES_H
