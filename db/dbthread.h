#ifndef DBTHREAD_H
#define DBTHREAD_H

#include <QObject>
#include <QThread>
#include <QtDebug>

#include <db/dbactions.h>


class dbthread : public QThread
{
    Q_OBJECT
public:
    dbthread(QObject *parent = 0);
    ~dbthread();

    void insertImages(const QStringList &urls);
    void insertPath(const QString &path);
    void select(const QString &query);

signals:

    void addImages(const QStringList &urls);
    void addPath(const QString &path);
    void getSelection(const QString &query);

    void imageAdded();
    void finished();
    void selectionReady(const  QList<QMap<int,QVariant>> &results);

    void pathSize(const int &size);

protected:
    void run();

private:
    dbactions *accessor;
};

#endif // DBTHREAD_H
