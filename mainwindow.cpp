#include "mainwindow.h"
#include "ui_mainwindow.h"


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
            ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , frame(new QFrame)
  , layout(new QHBoxLayout)
  , collectionUtils(new QWidget(this))
  , collectionBtns(new QButtonGroup(this))
  , views(new QWidget(this))
  , viewerUtils(new QWidget(this))
  , utilsBar (new QWidget(this))
  , altSidebar(new QWidget(this))
  , imageLabel(new QLabel(this))
  , scrollArea(new QScrollArea(this)),
    stack(new QStackedWidget(this))
  , scaleFactor(1)
{
    ui->setupUi(this);


    this->setWindowTitle("Pixs");
    //*THE SIDEBAR*//
    setUpCollectionBar();

    //*THE TOOLBAR*// /*this needs to go before setting up the views because views contains the toolbar*/
    setUpViewerBar();

    //*THE VIEWS*//
    setUpViews();

    //*THE ALTSIDEBAR*//
    setUpAltSidebar();

    //*PIXS MAINWINDOW*//
    setUpPixs();

    if(PixsUtils::fileExists(PixsUtils::CollectionDBPath+"pixs.db"))
    {
        qDebug()<<"db exist trying to populate tables";
        galleryView->populate("select * from images");
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::setUpCollectionBar()
{
    qDebug()<<"setting up the sidebar";
    connect(collectionBtns,SIGNAL(buttonClicked(int)),this, SLOT(changeView(int)));
    collectionUtils->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto collectionUtils_layout = new QHBoxLayout();
    collectionUtils->setLayout(collectionUtils_layout);
    collectionUtils_layout->setContentsMargins(0,0,0,0);
    collectionUtils_layout->setMargin(0);

    collectionBtns->setExclusive(true);

    collectionUtils_layout->addStretch();

    for(int i=0; i<SCOUNT;i++)
    {
        auto btn = new QToolButton();
        btn->setObjectName(sidebarMap.value(i));
        btn->setAutoRaise(true);
        btn->setIconSize(QSize(16,16));
        btn->setCheckable(true);
        collectionBtns->addButton(btn,i);
        collectionUtils_layout->addWidget(btn);

    }
    collectionUtils_layout->addStretch();

    collectionBtns->button(VIEWER)->setIcon(QIcon::fromTheme("folder-image"));
    collectionBtns->button(VIEWER)->setToolTip(tr("Viewer"));
    collectionBtns->button(VIEWER)->setChecked(true);

    collectionBtns->button(GALLERY)->setIcon(QIcon::fromTheme("view-grid"));
    collectionBtns->button(GALLERY)->setToolTip(tr("Gallery"));

    collectionBtns->button(ALBUMS)->setIcon(QIcon::fromTheme("identity"));
    collectionBtns->button(ALBUMS)->setToolTip(tr("Albums"));

    collectionBtns->button(PEOPLE)->setIcon(QIcon::fromTheme("tag-people"));
    collectionBtns->button(PEOPLE)->setToolTip(tr("People"));

    collectionBtns->button(TAGS)->setIcon(QIcon::fromTheme("tag"));
    collectionBtns->button(TAGS)->setToolTip(tr("Tags"));

    collectionBtns->button(SETTINGS)->setIcon(QIcon::fromTheme("configure"));
    collectionBtns->button(SETTINGS)->setToolTip(tr("Settings"));



}




void MainWindow::setUpViews()
{
    qDebug()<<"setting up the views";
    views->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto viewsLayout=new QVBoxLayout();
    views->setLayout(viewsLayout);

    utilsBar = new QWidget(this);
    utilsBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    auto utilsBar_layout = new QHBoxLayout();
    utilsBar_layout->setContentsMargins(0,0,0,0);
    utilsBar_layout->setMargin(0);
    utilsBar->setLayout(utilsBar_layout);

    utilsBar_layout->addWidget(collectionUtils);
    utilsBar_layout->addWidget(viewerUtils);


    viewsLayout->setContentsMargins(0,0,0,0);
    viewsLayout->setMargin(0);
    viewsLayout->setSpacing(0);
    viewsLayout->addWidget(stack);
    viewsLayout->addWidget(toolbarBorder);
    viewsLayout->addWidget(utilsBar,Qt::AlignBottom);

    stack->setContentsMargins(0,0,0,0);
    //stack->layout()->setContentsMargins(0,0,0,0);
    scrollArea->setContentsMargins(0,0,0,0);
    //scrollArea->layout()->setContentsMargins(0,0,0,0);

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    //imageLabel->setContentsMargins(0,0,0,0);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setAlignment(Qt::AlignCenter);




    settingsView = new Settings(this);
    galleryView = new collectionView(this);
    connect(galleryView,&collectionView::imageDoubleClicked,[this](QList<QMap<int,QVariant>> map)
    {
        this->loadFile(map.first().value(dbactions::URL).toString());
        this->changeView(VIEWER);
    });
    albumsView = new AlbumsView(this);

    //*THE VIEWS MAIN STACK*//
    stack->addWidget(scrollArea);
    stack->addWidget(galleryView);
    stack->addWidget(new QPushButton("3"));
    stack->addWidget(new QPushButton("4"));
    stack->addWidget(new QPushButton("5"));
    stack->addWidget(settingsView);

    changeView(GALLERY);

}

void MainWindow::setUpViewerBar()
{
    qDebug()<<"setting up the toolbar";

    viewerUtils->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    toolbarBorder = new lineS(Qt::Horizontal);
    auto viewerUtils_layout = new QHBoxLayout();
    viewerUtils->setLayout(viewerUtils_layout);
    viewerUtils_layout->setContentsMargins(0,0,0,0);
    viewerUtils_layout->setMargin(0);
    viewerUtils_layout->setSpacing(0);
    for(int i=0; i<TCOUNT; i++)
    {
        auto btn = new QToolButton();
        btn->setAutoRaise(true);
        btn->setIconSize(QSize(16,16));
        viewerBtns.insert(i,btn);

    }

    viewerBtns[SIDEBAR]->setIcon(QIcon::fromTheme("view-file-columns"));
    viewerUtils_layout->addWidget(viewerBtns[SIDEBAR]);
    connect(viewerBtns[SIDEBAR],&QToolButton::clicked,this,&MainWindow::sidebarBtn_clicked);

    viewerBtns[FULLSCREEN]->setIcon(QIcon::fromTheme("view-fullscreen"));
    viewerUtils_layout->addWidget(viewerBtns[FULLSCREEN]);

    viewerUtils_layout->addStretch();

    viewerBtns[PREVIOUS]->setIcon(QIcon::fromTheme("go-previous"));
    viewerUtils_layout->addWidget(viewerBtns[PREVIOUS]);
    connect(viewerBtns[PREVIOUS],&QToolButton::clicked,this,&MainWindow::previousBtn_clicked);

    viewerBtns[FAV]->setIcon(QIcon::fromTheme("love"));
    viewerUtils_layout->addWidget( viewerBtns[FAV]);

    viewerBtns[NEXT]->setIcon(QIcon::fromTheme("go-next"));
    viewerUtils_layout->addWidget( viewerBtns[NEXT]);
    connect( viewerBtns[NEXT],&QToolButton::clicked,this,&MainWindow::nextBtn_clicked);

    viewerUtils_layout->addStretch();

    viewerBtns[FIT]->setIcon(QIcon::fromTheme("zoom-select-fit"));
    viewerUtils_layout->addWidget( viewerBtns[FIT]);
    viewerBtns[FIT]->setCheckable(true);
    connect(viewerBtns[FIT],&QToolButton::clicked,this,&MainWindow::fitBtn_clicked);


    viewerBtns[SHARE]->setIcon(QIcon::fromTheme("document-share"));
    viewerUtils_layout->addWidget(viewerBtns[SHARE]);

    viewerBtns[EDIT]->setIcon(QIcon::fromTheme("editor"));
    viewerUtils_layout->addWidget(viewerBtns[EDIT]);
    viewerBtns[EDIT]->setCheckable(true);

}

void MainWindow::setUpAltSidebar()
{
    qDebug()<<"setting up the altSidebar";

    altSidebar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    altSidebar->setVisible(false);
    altSidebarBorder = new lineS(Qt::Vertical);
    altSidebarBorder->setVisible(false);
    auto altSidebarLayout = new QVBoxLayout();
    altSidebar->setLayout(altSidebarLayout);
    altSidebarLayout->setContentsMargins(0,0,0,0);

    altSidebarLayout->addWidget(new QListWidget());

}


void MainWindow::setUpPixs()
{

    qDebug()<<"setting up Pixs";
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    frame->setLayout(layout);
    //frame->setContentsMargins(0,0,0,0);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    layout->addWidget(views,Qt::AlignCenter);
    layout->addWidget(altSidebarBorder);
    layout->addWidget(altSidebar,Qt::AlignRight);

    this->setCentralWidget(frame);

    this->setContentsMargins(6,6,6,6);
    //*THE MENUBAR*//
    createActions();

}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
    openAct->setShortcut(QKeySequence::Open);

    saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &MainWindow::saveAs);
    saveAsAct->setEnabled(false);

    printAct = fileMenu->addAction(tr("&Print..."), this, &MainWindow::print);
    printAct->setShortcut(QKeySequence::Print);
    printAct->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    copyAct = editMenu->addAction(tr("&Copy"), this, &MainWindow::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    QAction *pasteAct = editMenu->addAction(tr("&Paste"), this, &MainWindow::paste);
    pasteAct->setShortcut(QKeySequence::Paste);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindow::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindow::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &MainWindow::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &MainWindow::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    helpMenu->addAction(tr("About &Qt"), &QApplication::aboutQt);
}




void MainWindow::changeView(int id)
{
    switch(id)
    {

    case VIEWER:
        stack->setCurrentIndex(VIEWER);
        collectionUtils->setVisible(false);
        viewerUtils->setVisible(true);
        collectionBtns->button(VIEWER)->setChecked(true);
        break;
    case GALLERY:
        stack->setCurrentIndex(GALLERY);
        viewerUtils->setVisible(false);
        collectionUtils->setVisible(true);
        collectionBtns->button(GALLERY)->setChecked(true);

        break;
    case ALBUMS: stack->setCurrentIndex(ALBUMS); break;
    case PEOPLE: stack->setCurrentIndex(PEOPLE); break;
    case TAGS: stack->setCurrentIndex(TAGS); break;
    case SETTINGS: stack->setCurrentIndex(SETTINGS); break;


    }


    qDebug()<<collectionBtns->button(id)->objectName();
}

bool MainWindow::loadFolder(const QString &filePath)
{
    qDebug()<<"trying to get all images from folder: "<< filePath;
    if(PixsUtils::fileExists(filePath))
    {


        QDirIterator it(filePath, formats, QDir::Files);
        while (it.hasNext())
        {
            QString file = it.next();
            queueImages.push_back(file);
            qDebug()<< file;
        }

        return true;
    }else
    {
        return false;
    }
}

bool MainWindow::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }


    setImage(newImage);
    currentImage=fileName;
    this->setWindowTitle(QFileInfo(currentImage).fileName());


    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
            .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    return true;
}



void MainWindow::open()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select Image File"), tr("Images (*.png *.xpm *.jpg *.jpeg *.gif *.bmp *.svg)"));
    if(!file.isEmpty())
    {
        loadFile(file);
    }

    queueImages.clear();
    loadFolder(QFileInfo(file).dir().path());
}
//! [1]

void MainWindow::saveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

//! [5]
void MainWindow::print()
//! [5] //! [6]
{
    Q_ASSERT(imageLabel->pixmap());
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    //! [6] //! [7]
    QPrintDialog dialog(&printer, this);
    //! [7] //! [8]
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabel->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabel->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
#endif
}
//! [8]

void MainWindow::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}




void MainWindow::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        // statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
                .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
    }
#endif // !QT_NO_CLIPBOARD
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void MainWindow::zoomIn()
{
    scaleImage(1.25);
}

void MainWindow::zoomOut()
{
    scaleImage(0.8);
}

void MainWindow::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void MainWindow::fitBtn_clicked()
{
    if(viewerBtns[FIT]->isChecked())
    {
        qDebug()<<"the button is checked";
        fitToWindow();
        viewerBtns[FIT]->setChecked(true);
    }else
    {
        qDebug()<<"the button is unchecked";
        normalSize();
        viewerBtns[FIT]->setChecked(false);
    }
}

void MainWindow::fitToWindow()
{
    auto imgW = imageLabel->pixmap()->toImage().width();
    auto imgH = imageLabel->pixmap()->toImage().height();

    auto percent = (imgW*100)/imgH;

    qDebug()<<"width:"<<imgW<<"height:"<<imgH<<"percent:"<<percent<<"scroll width:"<<scrollArea->size().width()
           <<"new size:"<<scrollArea->size().width()<<scrollArea->size().width()*(percent/100);
    imageLabel->resize((scrollArea->size().height()*percent)/100,scrollArea->size().height());

}

bool MainWindow::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    return true;
}

void MainWindow::setImage(const QImage &newImage)
{
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;
    scrollArea->setVisible(true);
    printAct->setEnabled(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!viewerBtns[FIT]->isChecked()) normalSize();
    else fitToWindow();
}

void MainWindow::updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void MainWindow::about()
//! [15] //! [16]
{
    QMessageBox::about(this, tr("About Image Viewer"),
                       tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
                          "and QScrollArea to display an image. QLabel is typically used "
                          "for displaying a text, but it can also display an image. "
                          "QScrollArea provides a scrolling view around another widget. "
                          "If the child widget exceeds the size of the frame, QScrollArea "
                          "automatically provides scroll bars. </p><p>The example "
                          "demonstrates how QLabel's ability to scale its contents "
                          "(QLabel::scaledContents), and QScrollArea's ability to "
                          "automatically resize its contents "
                          "(QScrollArea::widgetResizable), can be used to implement "
                          "zooming and scaling features. </p><p>In addition the example "
                          "shows how to use QPainter to print an image.</p>"));
}

void MainWindow::enterEvent(QEvent *event)
{

    event->accept();
}

void MainWindow::hideToolbar()
{
    utilsBar->setVisible(false);
    toolbarBorder->setVisible(false);
}

void MainWindow::showToolbar()
{
    utilsBar->setVisible(true);
    toolbarBorder->setVisible(true);
}

void MainWindow::leaveEvent(QEvent *event)
{
    //  toolbar->setVisible(false);
    //  toolbarBorder->setVisible(false);
    event->accept();
}

void MainWindow::editBtn_clicked()
{
    if(altSidebarVisible)
    {
        altSidebar->setVisible(false);
        altSidebarBorder->setVisible(false);
        viewerBtns[EDIT]->setChecked(false);
        altSidebarVisible=!altSidebarVisible;


    }else
    {
        altSidebar->setVisible(true);
        altSidebarBorder->setVisible(true);
        viewerBtns[EDIT]->setChecked(true);
        altSidebarVisible=!altSidebarVisible;

    }
}

void MainWindow::sidebarBtn_clicked()
{
    this->changeView(GALLERY);
    viewerBtns[SIDEBAR]->setChecked(true);
    sidebarVisible=!sidebarVisible;

}

void MainWindow::nextBtn_clicked()
{
    if(!queueImages.isEmpty())
    {
        int position = queueImages.indexOf(currentImage);
        qDebug()<<"on next clicked: images on queue:"<<  queueImages.size()<< "currentImage position: " << position;


        if(position+1 < queueImages.size())
        {
            qDebug()<<queueImages.at(position+1);
            loadFile(queueImages.at(position+1));
        }else
        {
            qDebug()<<queueImages.at(0);
            loadFile(queueImages.at(0));
        }
    }
}

void MainWindow::previousBtn_clicked()
{
    if(!queueImages.isEmpty())
    {
        int position = queueImages.indexOf(currentImage);
        qDebug()<<"on next clicked: images on queue:"<<  queueImages.size()<< "currentImage position: " << position;


        if(position-1 > 0)
        {
            qDebug()<<queueImages.at(position-1);
            loadFile(queueImages.at(position-1));
        }else
        {
            qDebug()<<queueImages.at(queueImages.size()-1);
            loadFile(queueImages.at(queueImages.size()-1));
        }
    }
}
