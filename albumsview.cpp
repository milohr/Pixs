#include "albumsview.h"



AlbumsView::AlbumsView(QWidget *parent) :
    QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto layout = new QGridLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    albumSize = PixsUtils::getWidgetSizeHint(PixsUtils::MEDIUM_ALBUM_FACTOR,PixsUtils::MEDIUM_ALBUM);
    this->setAcceptDrops(false);
    grid = new QListWidget(this);
    grid->setObjectName("grid");
    grid->setMinimumHeight(albumSize);
    grid->setViewMode(QListWidget::IconMode);
    grid->setResizeMode(QListWidget::Adjust);
    grid->setUniformItemSizes(true);
    grid->setWrapping(true);
    grid->setAcceptDrops(false);
    grid->setDragDropMode(QAbstractItemView::DragOnly);
    grid->setFrameShape(QFrame::NoFrame);
    grid->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );
    grid->setSizeAdjustPolicy(QListWidget::AdjustToContentsOnFirstShow);
    //grid->setStyleSheet("QListWidget {background:#2E2F30; border:1px solid black; border-radius: 2px; }");
    grid->setStyleSheet("QListWidget,QListWidget::item:selected,QListWidget::item:selected:active {background:transparent; padding-top:15px; padding-left:15px; color:transparent; }");
    grid->setGridSize(QSize(albumSize+25,albumSize+25));


    connect(this, &AlbumsView::createdAlbum,[this](Album *artwork)
    {
//        albumsList.push_back(artwork);

//        connect(artwork,&Album::albumCoverClicked,this,&AlbumsView::getAlbumInfo);


//        auto shadow = new QGraphicsDropShadowEffect();
//        shadow->setColor(QColor(0, 0, 0, 140));
//        shadow->setBlurRadius(9);
//        shadow->setOffset(3,5);

//        artwork->setGraphicsEffect(shadow);
//        artwork->borderColor=false;
//        artwork->setUpMenu();

//        connect(artwork,&Album::albumCoverDoubleClicked, [this] (QMap<int, QString> info)
//        {
//            emit albumDoubleClicked(info);
//        });

//        connect(artwork,&Album::playAlbum, [this] (QMap<int,QString> info) { emit this->playAlbum(info); });
//        connect(artwork,&Album::changedArt,this,&AlbumsView::changedArt_cover);
//        connect(artwork,&Album::babeAlbum_clicked,this,&AlbumsView::babeAlbum);
//        connect(artwork,&Album::albumDragStarted, this, &AlbumsView::hideAlbumFrame);
//        connect(artwork,&Album::albumCoverEnter,[artwork,shadow,this]()
//        {
//            if(hiddenLabels) artwork->showTitle(true);
//            shadow->setColor(QColor(0, 0, 0, 180));
//            shadow->setOffset(2,3);
//        });
//        connect(artwork, &Album::albumCoverLeave,[artwork,shadow,this]()
//        {
//            if(hiddenLabels) artwork->showTitle(false);
//            shadow->setColor(QColor(0, 0, 0, 140));
//            shadow->setOffset(3,5);
//        });


//        auto item = new QListWidgetItem();


//        itemsList.push_back(item);
//        item->setSizeHint(QSize(artwork->getSize(),artwork->getSize()));

//        if(artwork->getAlbum().isEmpty()) item->setText(artwork->getArtist());
//        else item->setText(artwork->getAlbum()+" "+artwork->getArtist());

//        item->setTextAlignment(Qt::AlignCenter);
//        grid->addItem(item);
//        grid->setItemWidget(item,artwork);

    });

    grid->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);



    order  = new QAction("Go Descending");
    connect(order, &QAction::triggered,this,&AlbumsView::orderChanged);
    grid->addAction(order);
    auto hideLabels = new QAction("Hide titles",this);
    grid->addAction(hideLabels);
    connect(hideLabels, &QAction::triggered,[hideLabels,this]()
    {
        if (hideLabels->text().contains("Hide titles"))
        {
            for(auto album: albumsList)
                album->showTitle(false);
            hideLabels->setText("Show titles");
            hiddenLabels = true;

        }
        else
        {
            for(auto album: albumsList)
                album->showTitle(true);
            hideLabels->setText("Hide titles");
            hiddenLabels = false;
        }

    });

    QAction *zoomIn = new QAction(this);
    zoomIn->setText("Zoom in");
    zoomIn->setShortcut(tr("CTRL++"));
    grid->addAction(zoomIn);
    connect(zoomIn, &QAction::triggered,[this]()
    {
        if(albumSize+5<=PixsUtils::MAX_MID_ALBUM_SIZE)
            this->setAlbumsSize(albumSize+5);

    });

    QAction *zoomOut = new QAction(this);
    zoomOut->setText("Zoom out");
    zoomOut->setShortcut(tr("CTRL+-"));
    grid->addAction(zoomOut);
    connect(zoomOut, &QAction::triggered,[this]()
    {
        if(albumSize-5>=PixsUtils::MAX_MIN_ALBUM_SIZE)
            this->setAlbumsSize(albumSize-5);
    });

    QSplitter *splitter = new QSplitter(parent);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Vertical);

    splitter->addWidget(grid);   
    layout->addWidget(splitter);   
    this->setLayout(layout);
}

AlbumsView::~AlbumsView(){}



void AlbumsView::setAlbumsSize(int value)
{
    albumSize=value;
    /*slider->setToolTip(QString::number(value));
                QToolTip::showText( slider->mapToGlobal( QPoint( 0, 0 ) ), QString::number(value) );*/
    for(auto album : albumsList)
    {
        album->setSize(albumSize);
        grid->setGridSize(QSize(albumSize+25,albumSize+25));
        grid->update();

    }

    for(auto item : itemsList) item->setSizeHint(QSize(albumSize, albumSize));

}

void AlbumsView::albumHover() { }

void  AlbumsView::flushGrid()
{
    albumsList.clear();
    albums.clear();
    grid->clear();
}

void AlbumsView::orderChanged()
{
    if(ascending)
    {
        grid->sortItems(Qt::DescendingOrder);
        order->setText("Go Ascending");
        ascending=!ascending;
    }else
    {
        grid->sortItems(Qt::AscendingOrder);
        order->setText("Go Descending");
        ascending=!ascending;
    }


}

void AlbumsView::populate(QSqlQuery query)
{
//    qDebug()<<"ON POPULATE ALBUM VIEW:";
//    int i =0;
//    while (query.next())
//    {
//        QString artist = query.value(ARTIST).toString();
//        QString album = query.value(TITLE).toString();
//        QString art=":Data/data/cover.svg";

//        if(!albums.contains(album+" "+artist))
//        {
////            albums<<album+" "+artist;
////            //qDebug()<<"creating a new album[cover] for<<"<<album+" "+artist;
////            if(!query.value(ART).toString().isEmpty()&&query.value(ART).toString()!="NULL")
////                art = query.value(ART).toString();
////            else art = connection.getArtistArt(artist);

//            auto artwork= new Album(this);
//            connect(artwork,&Album::albumCreated,[this](Album *album){emit createdAlbum(album);});
//            artwork->createAlbum(artist,album,art,PixsUtils::MEDIUM_ALBUM,4,true);

//        }

//        qDebug()<<"oi"<<i++;
//    }

//    //grid->adjustSize();
//    //    qDebug()<<grid->width()<<grid->size().height();
//    grid->sortItems(Qt::AscendingOrder);

}


void AlbumsView::filter(const QList<QMap<int,QString>> &filter, const int &type)
{

//    hide_all(true);

//    QList<QListWidgetItem*> matches;

//    for(auto result : filter)
//    {

//        switch(type)

//        {
//        case BabeTable::ALBUM:
//            matches<<grid->findItems(result[BabeTable::ALBUM]+" "+result[BabeTable::ARTIST], Qt::MatchFlag::MatchContains);
//            break;
//        case BabeTable::ARTIST:
//            matches<<grid->findItems(result[BabeTable::ARTIST], Qt::MatchFlag::MatchContains);
//            break;
//        }


//    }
//    for(QListWidgetItem* item : matches)
//        item->setHidden(false);
}

void AlbumsView::hide_all(bool state)
{
    for(int row(0); row < grid->count(); row++ )
        grid->item(row)->setHidden(state);
}


void AlbumsView::babeAlbum(QMap<int,QString> info)
{
    emit babeAlbum_clicked(info);
}


void AlbumsView::getAlbumInfo(QMap<int,QString> info)
{
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];



}



