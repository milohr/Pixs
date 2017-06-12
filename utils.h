#ifndef UTILS_H
#define UTILS_H

#include <QFrame>
#include <QObject>

class lineS :public QFrame
{
    Q_OBJECT

public:
    lineS(Qt::Orientation orientation)
    {
        if(orientation==Qt::Vertical)
        {
            this->setFrameShape(QFrame::VLine);
            this->setFrameShadow(QFrame::Plain);
            this->setMaximumWidth(1);
        }
        else if(orientation==Qt::Horizontal)
        {

            this->setFrameShape(QFrame::HLine);
            this->setFrameShadow(QFrame::Plain);
            this->setMaximumHeight(1);
        }
        this->setContentsMargins(0,0,0,0);
    }



};

class spacer :public QWidget
{
    Q_OBJECT

public:
    spacer()
    {
       this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

};
#endif // UTILS_H
