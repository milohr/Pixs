#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    connect(&db,&dbactions::logMessage,[this](QString msg)
    {

        qDebug()<<msg;
    });
    connect(&db,&dbactions::finishedInsertingImages,[this]()
    {

        qDebug()<<"finished inserting images";
    });
    db.createDB();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_toolButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select folder...", QDir().homePath()+"/Music/");
    QStringList urlCollection;

    if (QFileInfo(path).isDir())
    {
        QDirIterator it(path, {"*.png", "*.xpm"," *.jpg", "*.jpeg", "*.gif", "*.bmp", "*.svg"}, QDir::Files, QDirIterator::Subdirectories);

        while (it.hasNext()) urlCollection << it.next();

    } else if (QFileInfo(path).isFile()) urlCollection << path;


    db.insertImages(urlCollection);

}
