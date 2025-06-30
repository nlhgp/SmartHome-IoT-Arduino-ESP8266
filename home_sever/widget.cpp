#include "widget.h"
#include "ui_widget.h"
#include "qdatetime.h"
#include "QGraphicsDropShadowEffect"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    sever = new QTcpServer;//服务器对象
    //sever->listen(QHostAddress("192.168.126.56"),8000);//监听
    sever->listen(QHostAddress::AnyIPv4,PORT);
    setWindowTitle("服务端");
    //信号与槽
    connect(sever,&QTcpServer::newConnection,this,&Widget::new_client_hander);//连接客户端
    //发送信息
    connect(ui->messageline,&QLineEdit::returnPressed,this,&Widget::send_massage);
    connect(ui->sendButton,&QPushButton::clicked,this,&Widget::send_massage);
    init_ui();
}

Widget::~Widget()
{
    delete ui;
}

//添加信息记录
void Widget::add_item(QString &m)
{
    ui->messagelist->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));//添加消息时间
    ui->messagelist->addItem(m.toUtf8());
    ui->messagelist->scrollToBottom(); // 添加后滚动到底部
    //ui->messagelist->addItem("");//空一行，不然挤着难看
}

//从服务端发送消息
void Widget::send_massage()
{
    QString message=ui->messageline->text();
    add_item(message);
    if(message.length()==0) return;
    if(message=="打开窗户"){
        fstl(this->mts)
        {
            message = "open#window";
            ((*it)->socket->write(message.toUtf8()));
        }
    }
    else if(message=="关闭窗户"){
        fstl(this->mts)
        {
            message = "close#window";
            ((*it)->socket->write(message.toUtf8()));
        }
    }
    else if(message=="开门"){
        fstl(this->mts)
        {
            message = "open#door";
            ((*it)->socket->write(message.toUtf8()));
        }
    }
    else if(message=="关门"){
        fstl(this->mts)
        {
            message = "close#door";
            ((*it)->socket->write(message.toUtf8()));
        }
    }
    else
    {
        fstl(this->mts)
        {
            ((*it)->socket->write(message.toUtf8()));
        }
    }
    ui->messageline->clear();
}

void Widget::public_message(QString &m,QStringList &sl)
{
    int i;
    m=sl[2]+=" 说：";
    for(i=3;i<sl.size();++i)
    {
        m+=sl[i];
    }
    fstl(this->mts)
    {
        if((*it)->socket->peerAddress().toString()!=sl[0])
            (*it)->socket->write(m.toUtf8());
    }
}

//建立新线程
void Widget::new_client_hander()
{
    //建立tcp连接，连接也是对象，类型为socket
    QTcpSocket *socket = sever->nextPendingConnection();
    socket->peerAddress();//获取客户端地址
    socket->peerPort();//获取客户端端口号
    ui->iplineEdit->setText(socket->peerAddress().toString());
    ui->portlineEdit->setText(QString::number(socket->peerPort()));
    // //收到消息socket会发送readysocket信号
    // connect(socket,&QTcpSocket::readyRead,this,&Widget::message);

    //启动线程
    my_thread *t=new my_thread;
    t->get_socket(socket);
    t->start();
    mts.push_back(t);
    connect(t,&my_thread::message,this,&Widget::threadslot);
}

//接收线程传来的信号
void Widget::threadslot(QString m)
{
    qDebug() << m;
    QStringList sl=m.split('#');//将字符串以#分割
    if(sl[1]=="public")
    {
        this->public_message(m,sl);
    }
    else if(sl[1]=="worning"||(sl.length()>=5 && sl[4]=="worning"))
    {
        if(sl[2]=="thing_move"||(sl.length()>=6 && sl[5]=="thing_move"))
        {
            QString s="有物体经过";
            add_item(s);
        }
        else if(sl[2]=="people_move")
        {
            QString s="有人经过";
            add_item(s);
        }
        else if(sl[2]=="fire")
        {
            QString s="高温，有火灾风险";
            add_item(s);
        }
    }
    else if(sl[1]=="c"||sl[1]=="oc")
    {
        if(sl.length()>=3)
            ui->clabel->setText("当前温度：" + sl[2]);
        if(sl.length()>=4)
            ui->rhlabel->setText("当前湿度：" + sl[3]);
    }

    else if(sl[1]=="deconned")//发送下线信息
    {
        fstl(this->mts)
        {
            if((*it)->get_ip()==sl[0])
            {
                delete(*it);
                mts.erase(it);
            }
            else (*it)->socket->write((sl[2]+"已下线").toUtf8());
        }
    }
    if(sl.length()>2 && sl[2]=="yan")
    {
        QString s = "火灾警告！！！";
        add_item(s);
    }
    else if(sl.length()>4 && sl[4]=="fire")
    {
        QString s = "明火警告！！！";
        add_item(s);
    }
}

//清除消息
void Widget::on_clearButton_clicked()
{
    ui->messagelist->clear();
}

//美化页面
void Widget::init_ui()
{
    // ==================== 全局样式设置 ====================
    this->setStyleSheet("background-color: #f0f2f5;");  // 设置主窗口背景

    // ==================== 标题美化 ====================
    ui->title->setStyleSheet(R"(
        QLabel {
            color: #2c3e50;
            font-weight: bold;
            padding: 15px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #3498db, stop:1 #2980b9);
            border-radius: 8px;
            margin: 10px;
        }
    )");
    ui->title->setAlignment(Qt::AlignCenter);

    // ==================== 消息列表美化 ====================
    ui->messagelist->setStyleSheet(R"(
        QListWidget {
            background: white;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 5px;
            font-family: "Segoe UI";
        }
        QListWidget::item {
            border-bottom: 1px solid #ecf0f1;
            padding: 8px;
        }
        QListWidget::item:alternate {
            background: #f8f9fa;
        }
        QScrollBar:vertical {
            width: 12px;
            background: #ecf0f1;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background: #bdc3c7;
            min-height: 20px;
            border-radius: 6px;
        }
    )");

    // ==================== 输入区域美化 ====================
    ui->inputlabel->setStyleSheet("color: #34495e; font-weight: 500;");

    ui->messageline->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #bdc3c7;
            border-radius: 6px;
            padding: 8px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #3498db;
        }
    )");

    // ==================== 按钮统一风格 ====================
    QString buttonStyle = R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3498db, stop:1 #2980b9);
            border: none;
            color: white;
            padding: 8px 16px;
            border-radius: 6px;
            font-weight: bold;
            min-width: 80px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2980b9, stop:1 #3498db);
        }
        QPushButton:pressed {
            background: #2c3e50;
        }
    )";
    ui->sendButton->setStyleSheet(buttonStyle);
    ui->clearButton->setStyleSheet(buttonStyle);

    // ==================== 客户端信息区域美化 ====================
    QString infoLabelStyle = "color: #34495e; font-weight: 500;";
    ui->iplabel->setStyleSheet(infoLabelStyle);
    ui->portlabel->setStyleSheet(infoLabelStyle);

    QString lineEditStyle = R"(
        QLineEdit {
            border: 2px solid #bdc3c7;
            border-radius: 4px;
            padding: 6px;
            background: white;
        }
        QLineEdit:focus {
            border-color: #3498db;
        }
    )";
    ui->iplineEdit->setStyleSheet(lineEditStyle);
    ui->portlineEdit->setStyleSheet(lineEditStyle);

    // ==================== 环境数据显示区域美化 ====================
    QString envStyle = R"(
        QLabel {
            background: #ecf0f1;
            border-radius: 6px;
            padding: 10px;
            color: #2c3e50;
            font-weight: bold;
            min-width: 120px;
        }
    )";
    ui->clabel->setStyleSheet(envStyle);
    ui->rhlabel->setStyleSheet(envStyle);

    // ==================== 布局微调 ====================
    // 调整控件间距
    this->layout()->setSpacing(10);
    this->layout()->setContentsMargins(15, 15, 15, 15);

    // 消息列表区域添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(3, 3);
    ui->messagelist->setGraphicsEffect(shadow);

    // ==================== 字体优化 ====================
    QFont appFont("Segoe UI", 10);
    this->setFont(appFont);
}
