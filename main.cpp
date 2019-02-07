#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow* window = MainWindow::instance();
    window->show();

    return app.exec();
}
