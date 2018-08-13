#include <QApplication>
#include "mainwindow.h"
#include "scene.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.setWindowTitle("Detector de objetos");
//    w.showMaximized();

    Scene scene;
    scene.show();
    scene.move( 100, 100 );
    scene.resize( 640, 480 );


    return a.exec();
}



