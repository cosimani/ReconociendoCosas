#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QElapsedTimer>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QCloseEvent>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <QTextToSpeech>
#include <QKeyEvent>
#include <QVector>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

    QTextToSpeech *speech;

    QString multObj;
    QString unObj;
    bool detecting;
    QChar endch;

    QVector<QStringList> v;
    QVector<QString> objetosRepr;

    int d, maxApariciones,jk;
    float contar,umbralPorcentaje;
    int tiempoProcesamiento;


private slots:
    bool detectingObjects();

    void mensajeRecibido(const QString &mensaje);

private:
    Ui::MainWindow *ui;

    cv::dnn::Net tfNetwork;

    QGraphicsScene scene;

    QGraphicsPixmapItem pixmap;

    void on_startBtn_pressed();

    void on_reproduceBtn_pressed();

    void toSay(QString object, int c);

    void maximoApariciones();

    void chekearApariciones(QString object, int c);

    bool videoStopped;

    void speechConfig(int pitch,int rate, int language);

protected:
    void keyPressEvent( QKeyEvent *event );


};

#endif // MAINWINDOW_H
