#ifndef NOTE_H
#define NOTE_H

#include <QListWidgetItem>
#include <QDateTime>

#define DATETIMEFMT "h:mm d-MM-yyyy"

class Note: public QListWidgetItem
{
    Q_OBJECT
public:
    Note();
    ~Note();

    int id() const { return _id; }
    void setId(int id) { _id = id; }

    QString title() const { return text(); }
    void setTitle(const QString &title) { setText(title); }

    const QString &content() const { return _content; }
    void setContent(const QString &content) { _content = content; }

    int created() const { return _created.toTime_t(); }
    const QString &created() const { return _created.toString(DATETIMEFMT); }
    void setCreated(int created) { _created.setTime_t(created); }

    int modified() const { return _modified.toTime_t(); }
    const QString &modified() const { return _modified.toString(DATETIMEFMT); }
    void setModified(int modified) { _modified.setTime_t(modified); }

private:
    int _id;
    //QString _title; <- QListWidgetItem::text()
    QString _content;
    QDateTime _created;
    QDateTime _modified;
};

#endif // NOTE_H
