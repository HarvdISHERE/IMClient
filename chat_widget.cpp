#include "chat_widget.h"
#include "ui_chat_widget.h"



chat_widget::chat_widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chat_widget)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    init_Database();
    setupFriendTree();

}

chat_widget::~chat_widget()
{   
    if(m_db.isOpen()){
        m_db.close();
    }
    delete ui;
}

void chat_widget::init_Database(){
    QString dataPath=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if(!dir.exists()){
        dir.mkpath(".");
    }

    QString dbPath =dataPath +"chat_history.db";
    m_db=QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if(!m_db.open()){
        QMessageBox::critical(this,"数据库错误","无法打开数据库"+m_db.lastError().text());
        return;
    }

    createTables();
}

void chat_widget::createTables(){
    QSqlQuery query(m_db);

    bool success=query.exec(
        "CREATE TABLE IF NOT EXISTS messages("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "friend_name TEXT NOT NULL,"
        "sender TEXT NOT NULL,"
        "content TEXT NOT NULL,"
        "timestamp TEXT NOT NULL"
        ")"
        );

    if(!success){
        QMessageBox::critical(this,"数据库错误","建表失败："+query.lastError().text());
    }

    query.exec("CREATE INDEX IF NOT EXISTS ind_friend ON messages(friend_name)");
    success=query.exec(
        "CREATE TABLE IF NOT EXISTS friends("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "owner TEXT NOT NULL,"
        "friend_name TEXT NOT NULL,"
        "UNIQUE(owner, friend_name)"
        ")"
        );

    if(!success){
        QMessageBox::critical(this,"数据库错误","建表失败："+query.lastError().text());
    }
}
void chat_widget::add_db_Message(const QString &friendName,const QString &sender,
                                 const QString &content,const QString &timestamp){
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO messages(friend_name,sender,content,timestamp) "
                  "VALUES (:friend,:sender,:content,:time)");
    query.bindValue(":friend",friendName);
    query.bindValue(":sender",sender);
    query.bindValue(":content",content);
    query.bindValue(":time",timestamp);

    if(!query.exec()){
        qWarning() << "插入消息失败:" << query.lastError().text();
    }
}

QStringList chat_widget::loadMessages(const QString &friendName){
    QStringList messages;

    QSqlQuery query(m_db);
    query.prepare("SELECT sender,content FROM messages WHERE friend_name=:friend "
                  "ORDER BY id ASC" );
    query.bindValue(":friend",friendName);

    if(query.exec()){
        while(query.next()){
            QString sender=query.value(0).toString();
            QString content=query.value(1).toString();
            messages.append(QString("%1: %2").arg(sender).arg(content));
        }
    }

    return messages;
}
    // ==================== 初始化好友树 ====================
void chat_widget::add_db_friends(const QString &owner, const QString &friendName){
    QSqlQuery query(m_db);
    query.prepare("INSERT OR IGNORE INTO friends(owner,friend_name)"
                  "VALUES (:owner,:friend)");
    query.bindValue(":owner",owner);
    query.bindValue(":friend",friendName);
    query.exec();
}

void chat_widget::delete_db_friends(const QString &owner,const QString friendName){
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM friends WHERE owner= :owner AND friend_name=:friend");
    query.bindValue(":owner",owner);
    query.bindValue(":friend",friendName);
    query.exec();
}

QStringList chat_widget::loadfriends(const QString &owner){
    QStringList friends;
    QSqlQuery query(m_db);
    query.prepare("SELECT friend_name FROM friends WHERE owner=:owner ORDER BY id ASC");
    query.bindValue(":owner",owner);

    if(query.exec()){
        while(query.next()){
            friends.append(query.value(0).toString());
        }
    }

    return friends;
}
void chat_widget::setupFriendTree()
{
    // 隐藏表头
    ui->friend_treeWidget->setHeaderHidden(true);

    // 创建"我的好友"根节点
    root = ui->friend_treeWidget->topLevelItem(0);

    if(!root){
        root = new QTreeWidgetItem(ui->friend_treeWidget);
        root->setText(0, "好友");
    }
    root->setExpanded(true);
    // 添加示例好友
    QStringList friends = loadfriends(m_currentUser);
    if(!friends.contains("服务器")){
        friends.append("服务器");
    }
    for (const auto &name : friends) {
        QTreeWidgetItem *friendItem = new QTreeWidgetItem(root);
        friendItem->setText(0, name);

        // 初始化该好友的聊天记录
        m_chatHistory[name] = loadMessages(name);
    }
}

void chat_widget::setCurrentUser(const QString &user){
    m_currentUser = user;
    ui->username_label->setText(user);
    // 重新加载好友列表
    root = ui->friend_treeWidget->topLevelItem(0);
    if (root) {
        root->takeChildren();  // 清空现有好友
    }
    setupFriendTree();

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
    QString msg = ui->message->toPlainText();
    if (msg.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入消息内容");
        return;
    }

    // 检查是否选中了好友
    QTreeWidgetItem *currentItem = ui->friend_treeWidget->currentItem();
    if (!currentItem || currentItem->parent() == nullptr) {
        QMessageBox::information(this, "提示", "请先选择一个好友");
        return;
    }

    // 显示自己发的消息
    QString sendername=ui->username_label->text();
    QString friendName=currentItem->text(0);
    QString timestamp=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    m_chatHistory[friendName].append(QString("%1: %2").arg(sendername).arg(msg));
    add_UI_Message(sendername, msg);
    add_db_Message(friendName,sendername,msg,timestamp);

    // 发送信号给外部去实际发送
    emit sendbtn_clicked(msg);

    // 清空输入框
    ui->message->clear();
}

void chat_widget::on_friend_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(previous&&previous->parent()!=nullptr){
        saveCurrentChat(previous->text(0));
    }
    // 加载当前好友的聊天记录
    if (current && current->parent() != nullptr) {
        loadFriendChat(current->text(0));
    }
}

void chat_widget::saveCurrentChat(const QString &friendName){
    if(friendName.isEmpty()) return;

    QString content=ui->history->toPlainText();

    m_chatHistory[friendName]=content.split('\n',Qt::SkipEmptyParts);
}
// ==================== 加载聊天记录 ====================
void chat_widget::loadFriendChat(const QString &friendName)
{
    if (friendName.isEmpty()) return;

    // 清空聊天框
    ui->history->clear();

    // 加载该好友的历史记录
    if (m_chatHistory.contains(friendName)) {
        for (const auto &line : m_chatHistory[friendName]) {
            ui->history->append(line);
        }
    }
}

// ==================== 添加消息 ====================
void chat_widget::add_UI_Message(const QString &sender, const QString &content)
{
    // 显示到聊天框
    QString message = QString("%1: %2").arg(sender).arg(content);
    ui->history->append(message);

}

// ==================== 外部调用：收到消息 ====================
void chat_widget::onReceiveMessage(const QString &sender, const QString &content,const QString &timestamp)
{
    qDebug() << "收到消息:" << sender << content << timestamp;
    // 检查当前正在和谁聊天
    if(!m_chatHistory.contains(sender)){
        m_chatHistory[sender]=QStringList();

        QTreeWidgetItem *root=ui->friend_treeWidget->topLevelItem(0);
        if(root){
            QTreeWidgetItem *newFriend= new QTreeWidgetItem(root);
            newFriend->setText(0,sender);
        }
    }

    QTreeWidgetItem *currentItem = ui->friend_treeWidget->currentItem();
    QString message = QString("%1: %2").arg(sender).arg(content);
    if (currentItem && currentItem->parent() != nullptr) {
        QString currentFriend = currentItem->text(0);

        if (sender == currentFriend) {
            // 正是当前聊天对象，直接显示
            add_UI_Message(sender, content);
        }
    }
    m_chatHistory[sender].append(message);
    add_db_Message(sender,sender,content,timestamp);
}
void chat_widget::on_addfriend_pushButton_clicked()
{
    QString searchname=ui->search_friend->text();
    if(searchname.isEmpty()) return;

    for(int i=0;i<root->childCount();++i){
        QTreeWidgetItem *existing =root->child(i);
        if(existing->text(0)==searchname){
            QMessageBox::information(this,"提示","已经存在该好友");
            return;
        }
    }
    // 初始化该好友的聊天记录
    QTreeWidgetItem *friendItem = new QTreeWidgetItem(root);
    friendItem->setText(0, searchname);
    m_chatHistory[searchname] = loadMessages(searchname);
    add_db_friends(m_currentUser,searchname);
    ui->search_friend->clear();
    root->setExpanded(true);
}





void chat_widget::on_delete_Button_clicked()
{
    QTreeWidgetItem *delete_friend=ui->friend_treeWidget->currentItem();
    if(!delete_friend||delete_friend->parent()==nullptr||delete_friend->text(0)=="服务器") return;
    QString delete_friend_name=delete_friend->text(0);
    delete_db_friends(m_currentUser,delete_friend_name);
    m_chatHistory.remove(delete_friend_name);
    delete delete_friend;
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM messages WHERE friend_name = :friend");
    query.bindValue(":friend", delete_friend_name);
    query.exec();
    ui->history->clear();
}


void chat_widget::on_min_pushButton_clicked()
{
    this->showMinimized();
}

