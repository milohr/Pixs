#ifndef ALBUMSVIEW_H
#define ALBUMSVIEW_H

#include <QWidget>
#include <QSqlQuery>
#include <QGridLayout>
#include <QFrame>
#include <QListWidgetItem>
#include <QListWidget>
#include <QComboBox>
#include <QHeaderView>
#include <QToolTip>
#include <QDebug>
#include <QShortcut>
#include <QSplitter>
#include <QGraphicsDropShadowEffect>
#include <QAction>
//#include <QGraphicsBlurEffect>
//#include <QGraphicsScene>
//#include <QGraphicsPixmapItem>

#include "scrolltext.h"
#include "album.h"
#include "db/dbthread.h"
#include "pixsutils.h"

class AlbumsView : public QWidget
{
    Q_OBJECT

public:

    explicit AlbumsView(QWidget *parent = 0);
    ~AlbumsView();
    void populate(QSqlQuery query);
    void flushGrid();
    int getAlbumSize() { return this->albumSize; }
    void hide_all(bool state);
    void filter(const QList<QMap<int, QString> > &filter, const int &type);


    enum ALBUMSVIEW_H{ TITLE, DATE };

private:

    dbthread db;

    QAction *order;
    QListWidget *grid;

    int gridSize;
    int albumSize;

    bool ascending=true;
    bool hiddenLabels=false;

    QList<Album*> albumsList;
    QList<QListWidgetItem*> itemsList;

    QStringList albums;

public slots:

    void babeAlbum(QMap<int,QString> info);
    void setAlbumsSize(int value);
    void getAlbumInfo(QMap<int,QString> info);

private slots:

    void albumHover();
    void orderChanged();

signals:

    void albumDoubleClicked(QMap<int,QString> info);
    void albumOrderChanged(QString order);
    void playAlbum(QMap<int,QString> info);
    void babeAlbum_clicked( QMap<int,QString> info);
    void populateCoversFinished();
    void createdAlbum(Album *album);

};

#endif // ALBUMSVIEW_H
