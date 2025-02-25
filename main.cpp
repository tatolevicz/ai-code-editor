#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}

//
//#include <sol/sol.hpp>
//#include <cassert>
//
//int main() {
//
//    sol::state lua;
//    int x = 0;
//
//    lua.set_function("beep", [&x]{
//        ++x;
//        std::cout << "beep: " << x << std::endl;
//    });
//
//    lua.script("beep()");
//    lua.script("beep()");
//    lua.script("beep()");
//
//    assert(x == 3);
//}