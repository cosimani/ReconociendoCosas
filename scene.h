#ifndef SCENE_H
#define SCENE_H

#define RESOLUTION_WIDTH  640
#define RESOLUTION_HEIGHT 480

#include <stdio.h>
#include <stdlib.h>

#include <QDir>
#include <QFile>
#include <QTimer>
#include <QVector>
#include <QKeyEvent>
#include <QMouseEvent>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QLabel>

using namespace cv;
using namespace std;

class Scene : public QLabel  {
    Q_OBJECT

private:
    VideoCapture *videoCapture;
    QTimer *sceneTimer;
    cv::Mat frame;

    cv::dnn::Net * tfNetwork;

public:
    Scene( QWidget *parent = 0 );
    ~Scene();

protected:
    void keyPressEvent( QKeyEvent *event );

private slots:
    void slot_procesar();
};

#endif // SCENE_H
