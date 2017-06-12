#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QFrame>
#include <QButtonGroup>
#include <QToolButton>
#include <QDebug>
#include <QObject>
#include <QMap>
#include <QPushButton>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QScrollArea>
#include <QLabel>
#include <QAction>
#include <QImageWriter>
#include <QMessageBox>
#include <QFileDialog>
#include <QGuiApplication>
#include <QPainter>
#include <QClipboard>
#include <QMimeData>
#include <QMenu>
#include <QMenuBar>
#include <QScrollBar>
#include <QApplication>
#include <QImageReader>
#include <QListWidget>
#include <QPrintDialog>


#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif


#include "utils.h"
#include "pixsutils.h"
#include "db/dbthread.h"
#include "settings.h"
#include "albumsview.h"
#include "collectionview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadFolder(const QString &);
    bool loadFile(const QString &);
    enum sidebarBG { VIEWER, GALLERY, ALBUMS, PEOPLE, TAGS, SETTINGS, SCOUNT};
    enum toolbarBG { SIDEBAR, FULLSCREEN, PREVIOUS, FAV, NEXT, FIT, SHARE, EDIT, TCOUNT };

private:

    Ui::MainWindow *ui;

    Settings *settingsView;
    collectionView *galleryView;
    AlbumsView *albumsView;

    QFrame *frame;
    QHBoxLayout *layout;
    QWidget *sidebar;
    lineS *sidebarBorder;
    QButtonGroup *sidebarBtns;
    QWidget *views;
    QWidget *toolbar;
    QButtonGroup *toolbarBtns;
    lineS *toolbarBorder;
    QWidget *altSidebar;
    lineS *altSidebarBorder;
    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QStackedWidget *stack;


    QAction *saveAsAct;
    QAction *printAct;
    QAction *copyAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;

    bool sidebarVisible=true;
    bool altSidebarVisible = false;

    double scaleFactor;

    const QMap <int, QString> sidebarMap {{VIEWER,"viewerBtn"},{GALLERY,"galleryBtn"},{ALBUMS,"albumsBtn"},{PEOPLE,"peopleBtn"},{TAGS,"tagsBtn"},{SETTINGS,"settingsBtn"}};
    QStringList formats { "*.jpg" , "*.jpeg" , "*.png" , "*.bmp" , "*.svg" , "*.gif"};
    QStringList queueImages;
    QString currentImage;


    void setUpSidebar();
    void setUpViews();
    void setUpAltSidebar();
    void setUpPixs();
    void setUpToolbar();

    void setImage(const QImage &newImage);
    void updateActions();
    void createActions();

    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    bool saveFile(const QString &fileName);


    void hideToolbar();
    void showToolbar();


private slots:

    void open();
    void saveAs();
    void print();
    void copy();
    void paste();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();

    void changeView(int id);
    void sidebarBtn_clicked();
    void editBtn_clicked();
    void fitBtn_clicked();
    void nextBtn_clicked();
    void previousBtn_clicked();

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

};


#endif // MAINWINDOW_H
