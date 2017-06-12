#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QFileDialog>
#include <QDirIterator>

#include "db/dbthread.h"

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);

    ~Settings();

private slots:
    void on_toolButton_clicked();

private:
    Ui::Settings *ui;
    dbthread *db;
};

#endif // SETTINGS_H
