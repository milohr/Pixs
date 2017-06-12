#include "dbactions.h"



dbactions::dbactions(QObject *parent) : QObject(parent)
{
    createDB();
}

void dbactions::createDB()
{

    QDir qdir(databasePath);
    if (!qdir.exists())
        qdir.mkpath(databasePath);

    if (!PixsUtils::fileExists(databaseFile))
    {

        QFile file(":/db/script.sql");
        if (!file.exists())
        {
            QString log = QStringLiteral("Fatal error on build database. The file '");
            log.append(file.fileName() + QStringLiteral("' for database and tables creation query cannot be not found!"));
            qDebug()<<log;
            emit logMessage(log);
            return;
        }

        if (!file.open(QIODevice::ReadOnly))
        {
            emit logMessage(QStringLiteral("Fatal error on try to create database! The file with sql queries for database creation cannot be opened!"));
            return;
        }

        bool hasText;
        QString line;
        QByteArray readLine;
        QString cleanedLine;
        QStringList strings;

        while (!file.atEnd())
        {
            hasText     = false;
            line        = "";
            readLine    = "";
            cleanedLine = "";
            strings.clear();
            while (!hasText)
            {
                readLine    = file.readLine();
                cleanedLine = readLine.trimmed();
                strings     = cleanedLine.split("--");
                cleanedLine = strings.at(0);
                if (!cleanedLine.startsWith("--") && !cleanedLine.startsWith("DROP") && !cleanedLine.isEmpty())
                    line += cleanedLine;
                if (cleanedLine.endsWith(";"))
                    break;
                if (cleanedLine.startsWith("COMMIT"))
                    hasText = true;
            }
            if (!line.isEmpty())
                queryExec(line);
            if (!sqlQuery.isActive())
                emit logMessage(sqlQuery.lastError().text());
        }

        file.close();
        QFile::setPermissions(databaseFile, QFile::WriteOwner | QFile::ReadGroup | QFile::ReadUser | QFile::ReadOther);
        emit created();

    } else
    {
        qDebug()<<"Database file already exists! The build was ignored.";
        emit logMessage(QStringLiteral("Database file already exists! The build was ignored."));
    }
}

bool dbactions::queryExec(const QString &sqlQueryString)
{
    openConnection();

    if (!sqlDatabase.isOpen())
    {
        emit logMessage(QStringLiteral("Fatal error on execute query! The database connection cannot be opened!"));
        return false;

    } else if (sqlQuery.exec(sqlQueryString))
    {
        emit logMessage(QStringLiteral("Query success executed: ") + sqlQueryString);
        return true;
    }

    QString error(sqlQuery.lastError().text());

    if (!error.isEmpty())
    {
        error.prepend(QStringLiteral("Fatal error on execute query! The error is: "));
        error.append(QStringLiteral(" The query executed is: ") + sqlQueryString);
        emit logMessage(error);
    }

    return false;
}

int dbactions::insert(const QString &tableName, const QVariantMap &insertData)
{
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on insert! The table name is empty!"));
        return 0;
    } else if (insertData.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on insert! The insertData is empty!"));
        return 0;
    }

    QStringList strValues;
    QStringList fields = insertData.keys();
    QVariantList values = insertData.values();
    int totalFields = fields.size();
    for (int i = 0; i < totalFields; ++i)
        strValues.append("?");



    if (!sqlDatabase.isOpen()) {
        emit logMessage(QStringLiteral("Fatal error on insert! Database connection cannot be opened!"));
        return 0;
    }

    QString sqlQueryString;
    sqlQueryString = "INSERT INTO " + tableName + "(" + QString(fields.join(",")) + ") VALUES(" + QString(strValues.join(",")) + ")";
    sqlQuery.prepare(sqlQueryString);

    int k = 0;
    foreach (const QVariant &value, values)
        sqlQuery.bindValue(k++, value);

    return sqlQuery.exec() ? lastInsertId() : 0;
}

int dbactions::lastInsertId() const
{
    // WARNING!
    // lastInsertId() return value that have been auto-generated by the autoincremented column!
    QVariant idTemp = sqlQuery.lastInsertId();
    return idTemp.isValid() ? idTemp.toInt() : 0;
}

void dbactions::remove(const QString &path)
{

}

void dbactions::update(const QString &path)
{

}



void dbactions::select(const QString &query)
{
    this->prepareQuick();

    qDebug()<<"tryin to run a slection"<<query;
    QList<QMap<int,QVariant>> resultSet;

    sqlQuery.prepare(query);


    if(sqlQuery.exec())
    {

        while(sqlQuery.next())
        {

            auto url = sqlQuery.value(URL);
            auto title = sqlQuery.value(TITLE);
            auto rate = sqlQuery.value(RATE);
            auto fav = sqlQuery.value(FAV);
            auto color = sqlQuery.value(COLOR);
            auto note = sqlQuery.value(NOTE);
            auto adddate = sqlQuery.value(ADDDATE);
            auto source = sqlQuery.value(SOURCE);

            QMap<int,QVariant> map = {{URL,url},{TITLE,title},{RATE,rate},{FAV,fav},{COLOR,color},{NOTE,note},{ADDDATE,adddate},{SOURCE,source}};
            resultSet<<map;
        }
    }

    emit selectionReady(resultSet);

}

void dbactions::openConnection()
{
    if (!sqlDatabase.isOpen())
    {
        if (!QSqlDatabase::contains(applicationName))
        {
            sqlDatabase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), applicationName);
            sqlDatabase.setDatabaseName(databaseFile);
        }

        sqlDatabase = QSqlDatabase::database(applicationName);
        sqlQuery = QSqlQuery(sqlDatabase);
    }

    if (!sqlDatabase.isOpen())
        emit logMessage(QStringLiteral("Fatal error on insert! Database connection cannot be opened!"));
}



void dbactions::insertImage(const QString &url, const int &rate, const int &fav)
{

    QVariantMap map;

    auto source = QFileInfo(url).dir().path();
    this->insert("sources",{{"url",source}});

    map.insert("url",url);
    map.insert("title",QFileInfo(url).fileName());
    map.insert("rate",rate);
    map.insert("fav",fav);
    map.insert("color","");
    map.insert("note","");
    map.insert("addDate",QDate::currentDate());
    map.insert("source",source);
    this->insert("images",map);

    emit imageAdded();

}

void dbactions::insertImages(const QStringList &urls)
{
    this->prepareQuick();
    foreach(const auto &url, urls)
        this->insertImage(url);

    emit finished();
}

void dbactions::insertPath(const QString &path)
{
    QStringList paths;

    if (QFileInfo(path).isDir())
    {
        QDirIterator it(path, {"*.png", "*.xpm"," *.jpg", "*.jpeg", "*.gif", "*.bmp", "*.svg"}, QDir::Files, QDirIterator::Subdirectories);

        while (it.hasNext()) paths << it.next();

    } else if (QFileInfo(path).isFile()) paths << path;


    emit pathSize(paths.size());

    this->insertImages(paths);
}

void dbactions::prepareQuick()
{
    openConnection();
    if(!sqlQuery.exec("PRAGMA synchronous=OFF"))
        qDebug()<<"synchronous failed";

}

QList<QMap<int, QVariant> > dbactions::selectImage(const QString &img)
{
    QList<QMap<int, QVariant>> result;
    prepareQuick();

    sqlQuery.prepare("select * from images where title = '"+img+"';");

    if(sqlQuery.exec())
    {

        while(sqlQuery.next())
        {

            auto url = sqlQuery.value(URL);
            auto title = sqlQuery.value(TITLE);
            auto rate = sqlQuery.value(RATE);
            auto fav = sqlQuery.value(FAV);
            auto color = sqlQuery.value(COLOR);
            auto note = sqlQuery.value(NOTE);
            auto adddate = sqlQuery.value(ADDDATE);
            auto source = sqlQuery.value(SOURCE);

            QMap<int,QVariant> map = {{URL,url},{TITLE,title},{RATE,rate},{FAV,fav},{COLOR,color},{NOTE,note},{ADDDATE,adddate},{SOURCE,source}};

            result<<map;
        }
    }
    return result;
}

void dbactions::insertAlbum(const QString &title)
{

}

