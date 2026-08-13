#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include <QWidget>
#include <QMouseEvent>
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
    Ui::chat_widget *ui;
private slots:

    void on_close_pushButton_clicked();

    void on_sendMessage_clicked();
signals:
    void on_sendbtn_clicked(QString msg);

private:

    QPoint m_dragOffset;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // CHAT_WIDGET_H
