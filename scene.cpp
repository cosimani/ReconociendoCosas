#include "scene.h"
#include <QApplication>

#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>

Scene::Scene( QWidget *parent ) : QLabel( parent ),
                                  videoCapture ( new cv::VideoCapture( 0 ) ),
                                  sceneTimer ( new QTimer( this ) ),
                                  tfNetwork( new cv::dnn::Net )
{

    sceneTimer->start( 50 );

    connect( sceneTimer, SIGNAL( timeout() ), SLOT( slot_procesar() ) );

    *tfNetwork = cv::dnn::readNetFromTensorflow( "/home/cosimani/Proyecto/2018/ssd_mobilenet_v1_coco_2017_11_17/frozen_inference_graph.pb",
                                                "/home/cosimani/Proyecto/2018/ssd_mobilenet_v1_coco_2017_11_17/frozen_inference_graph.pbtxt" );


}

Scene::~Scene()
{
    videoCapture->release();
    delete tfNetwork;
}

void Scene::slot_procesar()  {
    videoCapture->operator >>( frame );

    cvtColor(frame, frame, CV_BGR2RGB);

    // see the following for more on this parameters
    // https://www.tensorflow.org/tutorials/image_retraining
    const int inWidth = 300;
    const int inHeight = 300;
    const float meanVal = 127.5; // 255 divided by 2
    const float inScaleFactor = 1.0f / meanVal;
    const float confidenceThreshold = 0.8f;

    QMap< int, QString > classNames;
    QFile labelsFile( "/home/cosimani/Proyecto/2018/ssd_mobilenet_v1_coco_2017_11_17/labels.txt" );
    if( labelsFile.open( QFile::ReadOnly | QFile::Text) )
    {
        while( ! labelsFile.atEnd() )
        {
            QString line = labelsFile.readLine();
            classNames[ line.split( ',' )[ 0 ].trimmed().toInt() ] = line.split( ',' )[ 1 ].trimmed();
        }
        labelsFile.close();
    }

    Mat inputBlob = cv::dnn::blobFromImage( frame,
                                            inScaleFactor,
                                            Size(inWidth, inHeight),
                                            Scalar(meanVal, meanVal, meanVal),
                                            true,
                                            false );
    tfNetwork->setInput( inputBlob );
    Mat result = tfNetwork->forward();
    Mat detections( result.size[ 2 ], result.size[ 3 ], CV_32F, result.ptr< float >() );

    // Entra a este for una vez por cada objeto que haya en labels.txt. O sea, entra tantas veces como
    // lineas en labels.txt.
    for( int i = 0; i < detections.rows; i++ )
    {
        float confidence = detections.at< float >( i, 2 );  // En la columna 2 esta la confianza

        // De todos los objetos que haya en labels.txt, va a entrar a este if solos aquellos
        // que superen el umbral de confianza o de probabilidad de que sea el objeto.
        if( confidence > confidenceThreshold )
        {
            using namespace cv;

            int objectClass = ( int )( detections.at< float >( i, 1 ) );  // En la columna 1 esta el id del objeto

            int left = static_cast<int>(detections.at<float>(i, 3) * frame.cols);
            int top = static_cast<int>(detections.at<float>(i, 4) * frame.rows);
            int right = static_cast<int>(detections.at<float>(i, 5) * frame.cols);
            int bottom = static_cast<int>(detections.at<float>(i, 6) * frame.rows);

            rectangle( frame, Point( left, top ), Point( right, bottom ), Scalar( 0, 255, 0 ) );
            String label = classNames[ objectClass ].toStdString();
            int baseLine = 0;
            Size labelSize = getTextSize( label, FONT_HERSHEY_SIMPLEX, 0.5, 2, &baseLine );
            top = max( top, labelSize.height );
            rectangle( frame, Point( left, top - labelSize.height ),
                       Point( left + labelSize.width, top + baseLine ),
                       Scalar( 255, 255, 255 ), FILLED );
            putText( frame, label, Point( left, top ),
                     FONT_HERSHEY_SIMPLEX, 0.5, Scalar( 0, 0, 0 ) );

        }
    }










    QPixmap pixmap = QPixmap::fromImage( QImage( frame.data,
                                                 frame.cols,
                                                 frame.rows,
                                                 frame.step,
                                                 QImage::Format_RGB888
                                               ).scaled( this->width(), this->height() )
                                       );
    this->setPixmap( pixmap );
}

void Scene::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() )  {
    case Qt::Key_Escape:
        videoCapture->release();
        qApp->quit();
        break;

    default:;
    }
}








