#include "chat_widget.h"
#include "ui_chat_widget.h"

chat_widget::chat_widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chat_widget)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
}

chat_widget::~chat_widget()
{
    delete ui;
}


void chat_widget::on_close_pushButton_clicked()
{
    this->close();
}


void chat_widget::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        m_dragOffset = event->globalPosition().toPoint()-(frameGeometry().topLeft());
        event->accept();
    }
}

void chat_widget::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons()&Qt::LeftButton){
        move(event->globalPosition().toPoint()-m_dragOffset);
        event->accept();
    }
}


void chat_widget::on_sendMessage_clicked()
{
    QString msg=ui->message->toPlainText();
    emit on_sendbtn_clicked(msg);
    ui->history->append("客户端Say: " + msg);
    ui->message->clear();
}

