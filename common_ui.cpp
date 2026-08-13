#include "common_ui.h"

    myui::myui(){
        cw=new chat_widget;
        lw=new Widget;
        ps=new proxy_setting;
}


myui::~myui() {
    delete cw;
    delete lw;
    delete ps;
}

