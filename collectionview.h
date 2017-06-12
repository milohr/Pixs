#ifndef COLLECTIONVIEW_H
#define COLLECTIONVIEW_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include "db/dbthread.h"
#include "db/dbactions.h"

class collectionView : public QListWidget
{
    Q_OBJECT

public:
    collectionView(QWidget *parent = 0);

    void populate(const QString &query);
    void addItems(const QList<QMap<int,QVariant>> & results);

private:

    dbthread *db;
    dbactions s_db;

public slots:
    void imageClicked(QListWidgetItem *item);

signals:
    void itemsReady(QList<QListWidgetItem*> items);
    void imageDoubleClicked( QList<QMap<int,QVariant>> &map);

};

#endif // COLLECTIONVIEW_H
