#include "my_thread.h"
my_thread::my_thread(QObject *parent)
    : QThread{parent}
{
}

void my_thread::run()
{
    connect(socket,&QTcpSocket::readyRead,this,&my_thread::client_info_slot);
    connect(socket,&QTcpSocket::disconnected,this,&my_thread::deconnected);
}

void my_thread::get_socket(QTcpSocket *s)//获取socket对象，顺便初始化ip
{
    this->socket=s;
    this->IP=s->peerAddress().toString();
}

//线程接受信号
void my_thread::client_info_slot()
{
    QString m=socket->readAll();
    QStringList sl=m.split('#');
    //处理心跳包和空语句
    if(m == "ping") return;
    if(sl.length()>1&&sl[0].length()>0)
        emit message(IP+"#"+m.toUtf8());
}

void my_thread::deconnected()
{
    QString m=IP+"#deconned#";
    m += username;
    emit message(m);
}

QString my_thread::get_ip()
{
    return this->IP;
}

void my_thread::set_name(QString &s)
{
    this->username=s;
}

QString my_thread::get_name()
{
    return this->username;
}
