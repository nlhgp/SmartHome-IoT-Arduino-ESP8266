#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <qtcpsocket.h>
// #include <QtSql/QSqlDatabase>//数据库头文件
// #include <qsqlquery.h>//操作数据库对象
#include <QMessageBox>
#include <my_thread.h>//线程
#include <QList>//容器
#define fstl(act) for(auto it=act.begin();it!=act.end();it++)
#define PORT 8000

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void add_item(QString &s);
    // void login_text(QStringList &sl);
    // void enroll_text(QStringList &sl);
    // void alter_text(QStringList &sl);
    void public_message(QString &m,QStringList &sl);
    void init_ui();
    // void private_message(QString &m,QStringList &sl);

private slots:
    void new_client_hander();
    void threadslot(QString ba);
    void send_massage();

    void on_clearButton_clicked();

private:
    Ui::Widget *ui;
    QTcpServer *sever;
    QList<my_thread*> mts;
    // QSqlDatabase db;
};
#endif // WIDGET_H
