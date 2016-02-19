#include "invasiontracker.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QLabel>

InvasionTracker::InvasionTracker(QWidget *parent) : QWidget(parent)
{
    layout = new QGridLayout(this);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    this->setLayout(layout);

    jsonWorker = new JsonWorker(this);
    connect(jsonWorker, SIGNAL(documentReady(QJsonDocument)), this, SLOT(jsonDocumentReceived(QJsonDocument)));

    //setup a timer to refresh invasions every 30 seconds
    timer = new QTimer(this);
    timer->setTimerType(Qt::VeryCoarseTimer);   //accuracy doesnt matter
    connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
    timer->start(30000);

    refresh();
}

void InvasionTracker::refresh()
{
    jsonWorker->startRequest(QUrl("https://www.toontownrewritten.com/api/invasions"));
}

void InvasionTracker::jsonDocumentReceived(QJsonDocument invasionsDocument)
{
    //clear previous invasions
    emit clear();

    QJsonObject apiObject = invasionsDocument.object();

    //check for an error with the API
    if(apiObject["error"].toString() == "")
    {
        QJsonObject invasionsObject = apiObject["invasions"].toObject();
        QStringList districtsList = invasionsObject.keys();
        QString districtName;
        int count = 0;

        foreach(districtName, districtsList)
        {
            QJsonObject districtInvasion = invasionsObject[districtName].toObject();
            InvasionBlock *invasionBlock = new InvasionBlock(this, districtName, districtInvasion["type"].toString(), districtInvasion["progress"].toString());
            layout->addWidget(invasionBlock, count / 3, count % 3, Qt::AlignCenter);
            invasionBlock->show();

            connect(this, SIGNAL(clear()), invasionBlock, SLOT(deleteLater()));
            count++;
        }
    }
    else
    {
        //error with API
    }
}

InvasionBlock::InvasionBlock(QWidget *parent, QString district, QString cog, QString progress) : QWidget(parent)
{
    //fix for glitch with cog names containing "\u0003"
    if(cog == "Micro\u0003manager")
    {
        cog = "Micromanager";
    }
    else if(cog == "Blood\u0003sucker")
    {
        cog = "Bloodsucker";
    }
    else if(cog == "Tele\u0003marketer")
    {
        cog = "Telemarketer";
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->setAlignment(Qt::AlignLeft);
    this->setLayout(layout);

    QLabel *districtLabel = new QLabel(this);
    QLabel *cogLabel = new QLabel(this);
    QLabel *progressLabel = new QLabel(this);
    QLabel *icon = new QLabel(this);
    QPixmap *iconPixmap = new QPixmap(QString(":/resources/invasion-icons/") + cog);

    layout->addWidget(icon, 1, 1, 3, 1, Qt::AlignCenter);
    layout->addWidget(districtLabel, 1, 2, Qt::AlignCenter);
    layout->addWidget(cogLabel, 2, 2, Qt::AlignCenter);
    layout->addWidget(progressLabel, 3, 2, Qt::AlignCenter);

    districtLabel->setText(district);
    cogLabel->setText(cog);
    progressLabel->setText(progress + QString(" cogs"));
    icon->setPixmap(*iconPixmap);
    icon->setFixedSize(iconPixmap->size());
}
