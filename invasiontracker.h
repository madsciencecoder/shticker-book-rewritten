#ifndef INVASIONTRACKER_H
#define INVASIONTRACKER_H

#include "jsonworker.h"
#include <QWidget>

class InvasionTracker : public QWidget
{
    Q_OBJECT
public:
    explicit InvasionTracker(QWidget *parent = 0);

signals:
    void clear();

public slots:
    void jsonDocumentReceived(QJsonDocument);

private slots:
    void refresh();

private:
    QTimer *timer;
    JsonWorker *jsonWorker;
    QLayout *layout;
};

class InvasionsView : public QWidget
{
    Q_OBJECT
public:
    explicit InvasionsView(QWidget *parent = 0);
};

class InvasionBlock : public QWidget
{
    Q_OBJECT
public:
    explicit InvasionBlock(QWidget *parent = 0, QString = "", QString = "", QString = "");
};

#endif // INVASIONTRACKER_H
