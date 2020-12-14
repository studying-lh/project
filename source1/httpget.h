#ifndef HTTPGET_H
#define HTTPGET_H

#include <QObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <stdio.h>
#include <QFile>
#include <QFileInfo>

class httpGet:public QObject
{
    Q_OBJECT
public:
    httpGet();
    void recv(QNetworkReply *reply);
    int GetWeather(QString times);
    void recvWeather(QNetworkReply *reply);
    void JsonWeatherDate();
signals:
    void getHttpTime(QString str);
    void getWeatherStr(QString str);
private:
    QNetworkAccessManager manage;

};

#endif // HTTPGET_H
