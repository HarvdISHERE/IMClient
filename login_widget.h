#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#define PORT 8848
#define IP 127.0.0.1
#include <QWidget>
#include <QTcpSocket>
class proxy_setting;
class chat_widget;
QT_BEGIN_NAMESPACE
namespace Ui {
class loginWidget;
}
QT_END_NAMESPACE

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget() override;

private slots:
    void on_login_pushButton_clicked();

    void on_menu_pushButton_clicked();

    void on_close_pushButton_clicked();

private:
    Ui::loginWidget *ui;
    proxy_setting *m_proxyWidget;
    chat_widget   *m_chatWidget;
    QPoint m_dragOffset;
    QTcpSocket *m_t;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};
#endif // LOGIN_WIDGET_H
