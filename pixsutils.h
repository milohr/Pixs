#ifndef PIXSUTILS_H
#define PIXSUTILS_H

#include "string"
#include <cmath>

#include <QString>
#include <QDebug>
#include <QStandardPaths>
#include <QFileInfo>
#include <QApplication>
#include <QDesktopWidget>
using namespace std;

namespace PixsUtils
{


enum ALbumSizeHint {BIG_ALBUM=200,MEDIUM_ALBUM=120,SMALL_ALBUM=80};

static const int MAX_BIG_ALBUM_SIZE=300;
static const int MAX_MID_ALBUM_SIZE=200;
static const int MAX_MIN_ALBUM_SIZE=80;

static const double BIG_ALBUM_FACTOR = 0.039;
static const double BIG_ALBUM_FACTOR_SUBWIDGET = 0.27;

static const double MEDIUM_ALBUM_FACTOR = 0.013;
static const double MEDIUM_ALBUM_FACTOR_SUBWIDGET = 0.4;

static const double SMALL_ALBUM_FACTOR = 0.006;
static const double SMALL_ALBUM_FACTOR_SUBWIDGET = 0.5;

inline int getWidgetSizeHint(const double &factor, const ALbumSizeHint &deafultValue)
{
    int ALBUM_SIZE = deafultValue;
    auto screenSize = QApplication::desktop()->availableGeometry().size();
    int albumSizeHint =  static_cast<int>(sqrt((screenSize.height()*screenSize.width())*factor));
    ALBUM_SIZE = albumSizeHint > ALBUM_SIZE? albumSizeHint : ALBUM_SIZE;


    switch(deafultValue)
    {

    case BIG_ALBUM:  return ALBUM_SIZE > MAX_BIG_ALBUM_SIZE? MAX_BIG_ALBUM_SIZE : ALBUM_SIZE;
    case MEDIUM_ALBUM:  return ALBUM_SIZE > MAX_MID_ALBUM_SIZE? MAX_MID_ALBUM_SIZE : ALBUM_SIZE;
    case SMALL_ALBUM:  return ALBUM_SIZE > MAX_MIN_ALBUM_SIZE ? MAX_MIN_ALBUM_SIZE :ALBUM_SIZE;

    }

    return MAX_MID_ALBUM_SIZE;
}

inline QString getNameFromLocation(QString str)
{
    QString ret;
    int index;

    for(int i = str.size() - 1; i >= 0; i--)
    {
        if(str[i] == '/')
        {
            index = i + 1;
            i = -1;
        }
    }

    for(; index < str.size(); index++)
    {
        ret.push_back(str[index]);
    }

    return ret;
}

const QString SettingPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+"/pixs/";
const QString CollectionDBPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/pixs/";
const QString CachePath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)+"/pixs/";
const QString ExtensionFetchingPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
const QString NotifyDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
const QStringList MoodColors = {"#F0FF01","#01FF5B","#3DAEFD","#B401FF","#E91E63"};

inline bool fileExists(const QString &url)
{
    QFileInfo path(url);
    if (path.exists()) return true;
    else return false;
}


}

#endif // PIXSUTILS_H
