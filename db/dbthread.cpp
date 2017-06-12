#include "dbthread.h"

dbthread::dbthread(QObject *parent) : QThread(parent)
{
    qDebug() << "Create DatabaseThread class";

}

dbthread::~dbthread()
{
    delete accessor;
    qDebug() << "Destroy DatabaseThread class";
}


void dbthread::run()
{
    accessor = new dbactions;

    connect(accessor,&dbactions::imageAdded,[this](){emit this->imageAdded();});
    connect(accessor, &dbactions::finished,[this](){emit this->finished();});
    connect(accessor, &dbactions::pathSize,[this](int size){emit this->pathSize(size);});
    connect(accessor, &dbactions::selectionReady,[this](QList<QMap<int,QVariant>>  result){emit this->selectionReady(result);});

    connect(this,&dbthread::getSelection,accessor,&dbactions::select);
    connect(this,&dbthread::addImages,accessor,&dbactions::insertImages);
    connect(this,&dbthread::addPath,accessor, &dbactions::insertPath);


    exec();
}


void dbthread::insertImages(const QStringList &urls)
{
    emit addImages(urls);
}

void dbthread::insertPath(const QString &path)
{
    emit addPath(path);
}

void dbthread::select(const QString &query)
{
    qDebug()<<"calling the select thread";
    emit getSelection(query);
}


