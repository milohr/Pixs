#ifndef DBACTIONS_H
#define DBACTIONS_H

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QDirIterator>

#include "pixsutils.h"

class dbactions : public QObject
{
    Q_OBJECT
public:
    enum ImagesTable
    {
        URL, TITLE, RATE, FAV, COLOR, NOTE, ADDDATE, SOURCE
    };

    explicit dbactions(QObject *parent = nullptr);
    void createDB();

    void insertImage(const QString &url, const int &rate=0, const int &fav=0);
    void insertImages(const QStringList &urls);

    void insertPath(const QString &path);

    void insertAlbum(const QString &title);

    void select(const QString &query);

    void prepareQuick();

    QList<QMap<int,QVariant>> selectImage(const QString &img);

private:



    const QString applicationName = "pixs";
    const QString databaseFile = PixsUtils::CollectionDBPath+"pixs.db";
    const QString databasePath = PixsUtils::CollectionDBPath;

    QSqlQuery sqlQuery;
    QSqlDatabase sqlDatabase;

    void openConnection();
    bool queryExec(const QString &sqlQueryString);

    int insert(const QString &tableName, const QVariantMap &insertData);
    void update(const QString &path);
    void remove(const QString &path);

    int lastInsertId() const;

signals:
    void created();
    void imageAdded();
    void imageRemoved();
    void logMessage(QString msg);
    void finished();
    void selectionReady(const   QList<QMap<int,QVariant>>  &result);

    void pathSize(const int &size);


public slots:
};

#endif // DBACTIONS_H
