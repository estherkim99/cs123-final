#include <QApplication>
#include "mainwindow.h"


int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
//    w.fileOpenPool();
    return app.exec();
}
