#include "collectionview.h"

collectionView::collectionView(QWidget *parent) : QListWidget(parent)
{
    db = new dbthread;
    db->start();

    this->setViewMode(QListWidget::IconMode);
    this->setResizeMode(QListWidget::Adjust);
    //        this->setUniformItemSizes(true);
    this->setWrapping(true);
    //    this->setAcceptDrops(false);
    this->setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );
    this->setSizeAdjustPolicy(QListWidget::AdjustToContentsOnFirstShow);
    this->setIconSize(QSize(140,100));
    this->setGridSize(QSize(180,180));

    connect(this,&collectionView::itemsReady,[this](QList<QListWidgetItem*> items)
    {

        for(auto item : items) this->addItem(item);
    });

    connect (this,&collectionView::itemDoubleClicked,this,&collectionView::imageClicked);
}



void collectionView::populate(const QString &query)
{
    qDebug()<<"trying to populate main galley";
    connect(db,&dbthread::selectionReady,[this](QList<QMap<int,QVariant>> results)
    {
        this->addItems(results);
    });

    db->select(query);
}


void collectionView::addItems(const QList<QMap<int,QVariant>> & results)
{
    //    QList<QListWidgetItem*> items;
    for(auto result : results)
    {
        auto item = new QListWidgetItem();

        QIcon icon(QPixmap(result[dbactions::URL].toString()).scaledToHeight(100));

//        item->setIcon(QIcon::fromTheme("image-x-generic"));
        item->setIcon(icon);
        item->setText(result[dbactions::TITLE].toString());
        item->setTextAlignment(Qt::AlignCenter);
        //        items<<item;
        this->addItem(item);
        qDebug()<<result[dbactions::TITLE].toString();
    }

    //    emit itemsReady(items);

}

void collectionView::imageClicked(QListWidgetItem *item)
{
    QString img = item->text();

    auto map = s_db.selectImage(img);

    emit imageDoubleClicked(map);
    qDebug()<<map;
}
