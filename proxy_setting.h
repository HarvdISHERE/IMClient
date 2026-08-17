#ifndef PROXY_SETTING_H
#define PROXY_SETTING_H

#include <QWidget>
#include <QMouseEvent>
#include "ui_proxy_setting.h"
namespace Ui {
class proxy_setting;
}

class proxy_setting : public QWidget
{
    Q_OBJECT

public:
    Ui::proxy_setting *ui;
    explicit proxy_setting(QWidget *parent = nullptr);
    ~proxy_setting();
    QString returnIP();
    QString returnPORT();
signals:
    void back();

private slots:

    void on_back_pushButton_clicked();

private:
    QPoint m_dragOffset;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // PROXY_SETTING_H
