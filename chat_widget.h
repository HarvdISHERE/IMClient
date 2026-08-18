#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QMessageBox>
#include <QStringList>
#include <QMap>
#include <QTreeWidgetItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include "ui_chat_widget.h"
namespace Ui {
class chat_widget;
}

class chat_widget : public QWidget
{
    Q_OBJECT

public:
    explicit chat_widget(QWidget *parent = nullptr);
    ~chat_widget();
    void onReceiveMessage(const QString &sender, const QString &content,const QString &timestamp="");  // 新增
    void add_UI_Message(const QString &sender, const QString &content);
    void setCurrentUser(const QString &user);
    QString m_currentUser;
    Ui::chat_widget *ui;

signals:
    void sendbtn_clicked(QString msg);

private slots:

    void on_close_pushButton_clicked();

    void on_sendMessage_clicked();

    void on_friend_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_addfriend_pushButton_clicked();

    void on_delete_Button_clicked();

    void on_min_pushButton_clicked();

private:
    QPoint m_dragOffset;
    QTreeWidgetItem *root;

    void add_db_friends(const QString &owner,const QString &friendName);
    void delete_db_friends(const QString &owner,const QString friendName);
    QStringList loadfriends(const QString &owner);


    QSqlDatabase m_db;
    void init_Database();
    void createTables();
    void add_db_Message(const QString &friendName, const QString &sender,
                       const QString &content, const QString &timestamp);
    QStringList loadMessages(const QString &friendName);

    QMap<QString, QStringList> m_chatHistory;
    void saveCurrentChat(const QString &friendName);
    void loadFriendChat(const QString &friendName);
    void setupFriendTree();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // CHAT_WIDGET_H
