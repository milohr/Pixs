#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    ui->progressBar->setVisible(false);
    db = new dbthread;
    db->start();

}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_toolButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select folder...", QDir().homePath()+"/Music/");

    ui->listWidget->addItem(path);

    ui->progressBar->setVisible(true);

    connect(db,&dbthread::pathSize,ui->progressBar,&QProgressBar::setMaximum);

    connect(db,&dbthread::imageAdded,[this]()
    {
        ui->progressBar->setValue(ui->progressBar->value()+1);

    });

    connect(db,&dbthread::finished,[this]()
    {
        ui->progressBar->setValue(0);
        ui->progressBar->setVisible(false);

    });

    db->insertPath(path);

}
