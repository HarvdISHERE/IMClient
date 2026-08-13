#ifndef COMMON_UI_H
#define COMMON_UI_H

#include "chat_widget.h"
#include "login_widget.h"
#include "proxy_setting.h"

class myui{
public:
    chat_widget *cw;
    Widget *lw;
    proxy_setting *ps;
    myui();
    ~myui();
};

#endif // COMMON_UI_H
