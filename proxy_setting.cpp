#include "proxy_setting.h"
#include "ui_proxy_setting.h"
#include "login_widget.h"
proxy_setting::proxy_setting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::proxy_setting)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
}

proxy_setting::~proxy_setting()
{
    delete ui;
}


void proxy_setting::on_back_pushButton_clicked()
{
    this->hide();
    emit back();
}

void proxy_setting::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        m_dragOffset = event->globalPosition().toPoint()-(frameGeometry().topLeft());
        event->accept();
    }
}

void proxy_setting::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons()&Qt::LeftButton){
        move(event->globalPosition().toPoint()-m_dragOffset);
        event->accept();
    }
}

QString proxy_setting::returnIP(){
    return ui->ip->text();
}

QString proxy_setting::returnPORT(){
    return ui->port->text();
}





