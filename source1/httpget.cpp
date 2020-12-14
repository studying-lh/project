#include "httpget.h"

httpGet::httpGet()
{
    /*接受信号关联*/
    connect(&this->manage,&QNetworkAccessManager::finished,this,&httpGet::recv);

    /*编辑发送的信息 获取时间  此为苏宁的api接口*/
    QString buf = QString("http://quan.suning.com/getSysTime.do");

    QUrl url(buf);
    /*定义一个请求对象*/
    QNetworkRequest req(url);
    /*发送时间获取请求*/
    manage.get(req);

}

/*信息接收*/
void httpGet::recv(QNetworkReply *reply)
{
    /*读取接收的信息*/
    QByteArray buf = reply->readAll();

    /*获取的样式
        {"sysTime2":"2020-12-12 10:10:40",
         "sysTime1":"20201212101040"}
    */
    /*解析json数据*/
    /*定义json的错误对象*/
    QJsonParseError err;

    /*定义Doucument对象*/
    QJsonDocument doc = QJsonDocument::fromJson(buf,&err);

    //得到最外层的对象{}
    QJsonObject obj=doc.object();

    /*获取值*/
    QString object = obj.value("sysTime2").toString();

    emit this->getHttpTime(object);
}
/*获取天气*/
int  httpGet::GetWeather(QString times)
{

    /*确认文本是否存在*/
    QFileInfo fileinfo("weather.txt");
    if(!fileinfo.exists())
    {
        QFile file("weather.txt");
        file.open(QIODevice::Truncate);
        file.close();
    }

    if( fileinfo.created().toString("yyyy-MM-dd") == times)
    {
       /*调用解析文本的函数*/
        this->JsonWeatherDate();
       /*结束 不发送请求*/
        return 0 ;
    }
    qDebug()<<"start get weather";
    /*断开时间获取*/
    disconnect(&this->manage,&QNetworkAccessManager::finished,this,&httpGet::recv);

    /*连接天气接受函数*/
    connect(&this->manage,&QNetworkAccessManager::finished,this,&httpGet::recvWeather);
    /*编辑发送的信息 获取天气  此为青云客的api接口*/
    QString buf = QString("http://api.qingyunke.com/api.php?key=free&appid=0&msg=%E5%A4%A9%E6%B0%94%E5%B9%BF%E5%B7%9E");

    QUrl url(buf);
    /*定义一个请求对象*/
    QNetworkRequest req(url);
    /*发送时间获取请求*/
    manage.get(req);

    return 1;
}

void httpGet::recvWeather(QNetworkReply *reply)
{
    /*读取接收的信息*/
    QByteArray buf = reply->readAll();



    /*打开文本*/
    QFile file("weather.txt");
    /*重新创建文本*/
    if(!file.open(QIODevice::Truncate|QIODevice::Text|QIODevice::WriteOnly))
    {
        qDebug()<<"open weather file error";
    }

    /*写入文本*/
    file.write(buf);

    /*关闭文本*/
    file.close();

    /*调用解析函数*/
    this->JsonWeatherDate();
}

/*数据解析*/
void httpGet::JsonWeatherDate()
{
    /*打开文本读取内容*/
    /*打开文本*/
    QFile file("weather.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"open weather file error";
    }

    /*读取全部内容*/
    QByteArray buf = file.readAll();


    /*获取的样式
        {"result":0,"content":"广州天气 ：当前温度20℃，感冒低发期，天气舒适，请注意多吃蔬菜水果，多喝水哦。
        {br}
        [12月12日] ：多云，低温 16℃，高温 24℃，风力2级
        {br}
        [12月13日] ：多云，低温 13℃，高温 24℃，风力2级
        {br}
        [12月14日] ：多云，低温 10℃，高温 14℃，风力3级
        {br}
        [12月15日] ：多云，低温 13℃，高温 18℃，风力3级
        {br}
        [12月16日] ：多云，低温 15℃，高温 20℃，风力3级"}
    */
    /*解析json数据*/
    /*定义json的错误对象*/
    QJsonParseError err;

    /*定义Doucument对象*/
    QJsonDocument doc = QJsonDocument::fromJson(buf,&err);

    //得到最外层的对象{}
    QJsonObject obj=doc.object();

    /*获取值*/
    QString object = obj.value("content").toString();

    /*字符串处理*/
    //获取第一段
    QString first = object.mid(object.indexOf(":\"广州天气"),object.indexOf("{br}")-object.indexOf(":\"广州天气"));
    //第一段拆分
    QString firstOne = first.mid(0,first.indexOf("，"));

    /*获取第二段*/
    QString second = object.mid(object.indexOf("{br}"),object.indexOf("风力")-object.indexOf("{br}")-1);
    //第二段拆分
    QString secondOne = second.mid(second.indexOf("：")+1);

    /*合并*/
    QString objectStr = QString("%1，%2").arg(firstOne).arg(secondOne);
    emit this->getWeatherStr(objectStr);
}






