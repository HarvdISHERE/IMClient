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

    ok=false;
    m_t=new QTcpSocket;
    m_proxyWidget = new proxy_setting;
    m_proxyWidget->hide();
    init_login();
    m_chatWidget = new chat_widget;
    m_chatWidget->hide();
    this->setWindowFlag(Qt::FramelessWindowHint);

    connect(m_proxyWidget,&proxy_setting::back,[=](){
        m_proxyWidget->hide();
        this->show();
    });

    connect(m_t,&QTcpSocket::connected,this,[=](){
        m_chatWidget->add_UI_Message("服务器","已经成功连接到了服务器...");
        QString login_info="LOGIN:\t"+account+'\t'+password+'\n';
        m_t->write(login_info.toUtf8());
    });

    connect(m_t,&QTcpSocket::readyRead,this,[=](){
        m_recvBuf.append(m_t->readAll());

        while(m_recvBuf.contains('\n'))
        {
            int pos = m_recvBuf.indexOf('\n');
            QByteArray onePkg = m_recvBuf.left(pos);
            m_recvBuf.remove(0, pos+1);

            QString response = QString::fromUtf8(onePkg).trimmed();
            if(response.isEmpty()) continue;

            QStringList parts = response.split('\t');
            if(parts.isEmpty()) continue;

            if (parts[0] == "LOGIN_OK") {
                // 登录成功
                ok = true;
                m_loggedIn = true;
                m_chatWidget->ui->username_label->setText(account);
                m_chatWidget->setCurrentUser(account);
                this->hide();
                m_chatWidget->show();
                m_chatWidget->add_UI_Message("服务器","✅ 登录成功");
            }
            else if (parts[0] == "SEND_OK") {
                // 消息发送成功回执，不需要显示
            }
            else if (parts[0] == "MSG"&& parts.size() >= 3) {
                // 收到别人发来的消息: MSG\t发送者\t内容
                QString sender = parts[1];
                QString content = parts[2];
                m_chatWidget->onReceiveMessage(sender,content);
            }
            else if (parts[0] == "ERROR") {
                // 错误信息
                m_chatWidget->onReceiveMessage("服务器","❌ " + response);
            }
            else if (ok) {
                // 其他消息（仅登录后显示）
                m_chatWidget->onReceiveMessage("服务器",response);
            }
        }
    });

    connect(m_t,&QTcpSocket::disconnected,this,[=](){
        m_recvBuf.clear();
        m_chatWidget->add_UI_Message("服务器","链接已断开，可能是网络问题或服务器未打开..."  );
        ok=false;
        m_loggedIn = false;
    });

    connect(m_t, &QTcpSocket::errorOccurred, this, [=](QAbstractSocket::SocketError err){
        QString errMsg = m_t->errorString();
        QMessageBox::warning(this,"连接失败",
                             QString("无法连接服务器！\n错误:%1\nIP:%2 端口:%3")
                                 .arg(errMsg)
                                 .arg(m_proxyWidget->returnIP())
                                 .arg(m_proxyWidget->returnPORT()));
        //连接失败重置状态
        ok = false;
        m_loggedIn = false;
    });

    connect(m_chatWidget, &chat_widget::sendbtn_clicked, this, [this](const QString &msg){
        if (m_t->state() != QAbstractSocket::ConnectedState) {
            m_chatWidget->add_UI_Message("服务器","❌ 未连接到服务器");
            return;
        }

        // 获取当前选中的聊天对象
        QTreeWidgetItem *currentItem = m_chatWidget->ui->friend_treeWidget->currentItem();
        if (!currentItem || currentItem->parent() == nullptr) {
            m_chatWidget->add_UI_Message("服务器","❌ 请先选择一个聊天对象");
            return;
        }

        QString target = currentItem->text(0);

        // ✅ 发给好友，包装成 SEND: 协议
        QString sendMsg = QString("SEND:\t%1\t%2\n").arg(target).arg(msg);
        m_t->write(sendMsg.toUtf8());
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

    account=ui->account_lineEdit->text();
    password=ui->password_lineEdit->text();

    if (account.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "❌ 账号或密码不能为空");
        return;
    }


    //===== 关键：勾选自动登录 →强制开启记住密码 =====
    if(ui->auto_login_checkBox->checkState() == Qt::Checked)
    {
        ui->remeber_password_checkBox->setCheckState(Qt::Checked);
    }

    //===== 登录那一刻统一保存配置到json =====
    if(ui->remeber_password_checkBox->checkState() == Qt::Checked)
    {
        saveConfig();
    }else
    {
        clearSavedPassword();
    }

    auto state = m_t->state();
    if(state == QAbstractSocket::UnconnectedState)
    {
        m_t->connectToHost(QHostAddress(ip),port);
    }
    else if(state == QAbstractSocket::ConnectingState)
    {
        QMessageBox::information(this,"提示","⏳ 正在连接服务器，请等待超时后重试");
    }
    else if(state == QAbstractSocket::ConnectedState)
    {
        QString login_info = QString("LOGIN:\t%1\t%2\n").arg(account).arg(password);
        m_t->write(login_info.toUtf8());
    }
    else
    {
        //强制重置卡死的socket
        m_t->abort();
        m_t->connectToHost(QHostAddress(ip),port);
    }
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

void LoginWidget::init_login(){
    QString filePath = QDir(QCoreApplication::applicationDirPath()).filePath("login.json");
    QFile file;
    file.setFileName(filePath);
    if (!file.exists()) {
        return;  // 首次运行，没有配置文件，直接返回
    }
    if(!file.open(QIODevice::ReadOnly)){
        return;
    }
    QByteArray jsondata=file.readAll();
    file.close();
    if(jsondata.isEmpty()){
        return;
    }
    QJsonDocument doc=QJsonDocument::fromJson(jsondata);
    QJsonObject json=doc.object();

    account=json["account"].toString();
    password=json["password"].toString();
    QString ip=json["ip"].toString();
    QString port=json["port"].toString();
    bool autoLogin = json["auto_login"].toBool(false);
    ui->account_lineEdit->setText(account);
    ui->password_lineEdit->setText(password);
    m_proxyWidget->ui->ip->setText(ip);
    m_proxyWidget->ui->port->setText(port);

    // 记住密码复选框
    if(!password.isEmpty())
    {
        ui->remeber_password_checkBox->blockSignals(true);
        ui->remeber_password_checkBox->setCheckState(Qt::Checked);
        ui->remeber_password_checkBox->blockSignals(false);
    }

    //自动登录复选框赋值（阻断信号，防止初始化触发槽函数）
    ui->auto_login_checkBox->blockSignals(true);
    if(autoLogin)
    {
        ui->auto_login_checkBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui->auto_login_checkBox->setCheckState(Qt::Unchecked);
    }
    ui->auto_login_checkBox->blockSignals(false);


    // ============核心：自动登录触发============
    if(autoLogin && !account.isEmpty() && !password.isEmpty())
    {
        //延时一小会再执行登录，等所有控件初始化完毕
        QTimer::singleShot(2000,this,[this](){
            on_login_pushButton_clicked();
        });
    }
}


void LoginWidget::saveConfig()
{
    QJsonObject json;
    json["account"]  = ui->account_lineEdit->text();
    json["password"]= ui->password_lineEdit->text();
    json["ip"]      = m_proxyWidget->returnIP();
    json["port"]    = m_proxyWidget->returnPORT();
    json["auto_login"] = (ui->auto_login_checkBox->checkState() == Qt::Checked);
    json["remember_pwd"] = (ui->remeber_password_checkBox->checkState() == Qt::Checked);

    QJsonDocument doc(json);
    QString filePath = QCoreApplication::applicationDirPath()+"/login.json";
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void LoginWidget::clearSavedPassword()
{
    QJsonObject json;
    json["account"]  = ui->account_lineEdit->text();
    json["password"] = "";         //清空密码
    json["ip"]      = m_proxyWidget->returnIP();
    json["port"]    = m_proxyWidget->returnPORT();
    json["auto_login"] = false;    //关闭自动登录
    json["remember_pwd"] = false;  //关闭记住密码标记

    QJsonDocument doc(json);
    QString filePath = QCoreApplication::applicationDirPath()+"/login.json";
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}