#include "login_widget.h"
#include "ui_login_widget.h"
#include "chat_widget.h"
#include "proxy_setting.h"
#include <QMouseEvent>
LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::loginWidget)
    ,m_proxyWidget(nullptr)
    ,m_chatWidget(nullptr)
{
    ui->setupUi(this);
    m_t=new QTcpSocket;
    m_proxyWidget = new proxy_setting;
    m_proxyWidget->hide();

    m_chatWidget = new chat_widget;
    m_chatWidget->hide();
    this->setWindowFlag(Qt::FramelessWindowHint);
    connect(this->m_proxyWidget,&proxy_setting::back,[=](){
        this->m_proxyWidget->hide();
        this->show();
    });

    connect(m_t,&QTcpSocket::readyRead,this,[=](){
        QByteArray data =m_t->readAll();
        m_chatWidget->ui->history->append("服务器say:"+data);
    });

    connect(m_t,&QTcpSocket::disconnected,this,[=](){
        m_chatWidget->ui->history->append("链接已断开，可能是网络问题或服务器未打开...");
    });
    connect(m_t,&QTcpSocket::connected,this,[=](){
        m_chatWidget->ui->history->append("已经成功连接到了服务器...");

        //这里写一个发送信号的东西到登录界面login里决定this要不要隐藏
    });
    connect(m_chatWidget,&chat_widget::on_sendbtn_clicked,this,[this](QString msg){
        m_t->write(msg.toUtf8());
    });
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_login_pushButton_clicked()
{
    QString ip=m_proxyWidget->returnIP();
    unsigned short port=m_proxyWidget->returnPORT().toUShort();
    m_t->connectToHost(QHostAddress(ip),port);
    this->hide();
    m_chatWidget->show();
}


void LoginWidget::on_menu_pushButton_clicked()
{
    this->hide();
    m_proxyWidget->show();
}


void LoginWidget::on_close_pushButton_clicked()
{
    this->close();
}

void LoginWidget::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        m_dragOffset = event->globalPosition().toPoint()-(frameGeometry().topLeft());
        event->accept();
    }
}

void LoginWidget::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons()&Qt::LeftButton){
        move(event->globalPosition().toPoint()-m_dragOffset);
        event->accept();
    }
}