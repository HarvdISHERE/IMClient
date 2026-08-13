#include "login_widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWidget login;
    login.show();
    return QApplication::exec();
}
