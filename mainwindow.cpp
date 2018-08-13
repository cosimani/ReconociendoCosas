#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    unObj("se ha detectado un"),
    multObj("se han detectado"),
    detecting(false),
    d(0),
    tiempoProcesamiento(1000),
    contar(0),
    umbralPorcentaje(0.5)
{
    ui->setupUi(this);
    videoStopped = true;
    ui->videoView->setScene(&scene);
    scene.addItem(&pixmap);
    // Creamos el objeto speech para reproducir el texto que detectemos
    speech = new QTextToSpeech();
    speech->setLocale(QLocale(QLocale::Spanish, QLocale::LatinScript, QLocale::Spain));

    // Configurar la voz.
    speechConfig(0.2,0.2,17);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startBtn_pressed()
{
    if(!videoStopped)
    {
        videoStopped = true;
        return;
    }

    using namespace cv;
    using namespace dnn;
    using namespace std;

    // see the following for more on this parameters
    // https://www.tensorflow.org/tutorials/image_retraining
    const int inWidth = 300;
    const int inHeight = 300;
    const float meanVal = 127.5; // 255 divided by 2
    const float inScaleFactor = 1.0f / meanVal;
    const float confidenceThreshold = 0.6f;

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

    tfNetwork = cv::dnn::readNetFromTensorflow( "/home/cosimani/Proyecto/2018/ssd_mobilenet_v1_coco_2017_11_17/frozen_inference_graph.pb",
                                                "/home/cosimani/Proyecto/2018/ssd_mobilenet_v1_coco_2017_11_17/frozen_inference_graph.pbtxt" );


    VideoCapture video;
    video.open( 0 );

    videoStopped = false;

    Mat image;

    while(!videoStopped && video.isOpened())
    {
        video >> image;
        Mat inputBlob = cv::dnn::blobFromImage( image,
                                                inScaleFactor,
                                                Size(inWidth, inHeight),
                                                Scalar(meanVal, meanVal, meanVal),
                                                true,
                                                false );
        tfNetwork.setInput( inputBlob );
        Mat result = tfNetwork.forward();
        Mat detections( result.size[ 2 ], result.size[ 3 ], CV_32F, result.ptr< float >() );

        QStringList lista; // creo la lista en donde voy a agregar los objetos que detecte

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

                int left = static_cast<int>(detections.at<float>(i, 3) * image.cols);
                int top = static_cast<int>(detections.at<float>(i, 4) * image.rows);
                int right = static_cast<int>(detections.at<float>(i, 5) * image.cols);
                int bottom = static_cast<int>(detections.at<float>(i, 6) * image.rows);

                rectangle( image, Point( left, top ), Point( right, bottom ), Scalar( 0, 255, 0 ) );
                String label = classNames[ objectClass ].toStdString();
                int baseLine = 0;
                Size labelSize = getTextSize( label, FONT_HERSHEY_SIMPLEX, 0.5, 2, &baseLine );
                top = max( top, labelSize.height );
                rectangle( image, Point( left, top - labelSize.height ),
                           Point( left + labelSize.width, top + baseLine ),
                           Scalar( 255, 255, 255 ), FILLED );
                putText( image, label, Point( left, top ),
                         FONT_HERSHEY_SIMPLEX, 0.5, Scalar( 0, 0, 0 ) );

                //Si la deteccion esta activada, entramos en este if para ir agregando aquellos objetos que hayan
                // superado el umbral de confianza
                if( detecting == true )
                {
                    QString lab = QString::fromStdString(label);
                    lista << lab;
//                    std::cout << "v.size=" << v.size() << "-se agrego un " << lab.toStdString() << std::endl;
                }
            }
        }

        if( detecting == true )
        {
            v.append( lista );
        }
        else   {
            // Entra aca cuando pasa el tiempo de deteccion y queda dando vueltas aca
            // Analizar aca el QVector. Junto antes de borrarlo.
            on_reproduceBtn_pressed();
            // Borramos el vector
            v.clear();
        }
        pixmap.setPixmap(QPixmap::fromImage(QImage(image.data,
                                                   image.cols,
                                                   image.rows,
                                                   image.step,
                                                   QImage::Format_RGB888).rgbSwapped()));
        ui->videoView->fitInView(&pixmap, Qt::KeepAspectRatio);
        qApp->processEvents();
    }
    video.release();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!videoStopped)
    {
        QMessageBox::warning(this,
                             "Advertencia",
                             "Primero debe detener la camara!");
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::on_reproduceBtn_pressed()
{
    // En este ciclo for buscamos los distintos tipos de objetos que se detectaron cuando estaba activa la deteccion, y
    // lo almacenamos en un vector
    for ( int i=0 ; i < v.size() ; i++ )  {
        QStringList listado = v.at( i );

//        std::cout << "vector.at(" << QString::number( i ).toStdString() << ") = ";

        for ( int j=0 ; j < listado.size() ; j++ )  {
//             std::cout << listado.at( j ).toStdString() << " ";
             QString obj1 = listado.at(j);
             if(!objetosRepr.contains(obj1))
             {
                 objetosRepr.append(obj1);
             }
        }
//        std::cout<<std::endl;
    }

    //Llamamos al metodo maximoApariciones para que calcule cual es la cantidad maxima de veces que aparecio en
    // una lista un objeto
    maximoApariciones();
    d = 0;
    objetosRepr.clear();
}

void MainWindow::toSay(QString object,int c)
{
    QString decir;
    QChar ultimaLetra = object.right( 1 ).at( 0 );
    if(d==0)
    {
                    if(c == 1)
                    {
                      if(ultimaLetra != 'a')
                      {
                        decir = unObj + object;
                      }
                      else decir = "Se ha detectado una" + object;
                    }
                    if(c == 2)
                    {
                      decir =multObj +  " dos " + object;
                      decir.append('s');
                    }
                    if(c == 3)
                    {
                      decir = multObj + " tres " + object;
                      decir.append('s');
                    }
                    if(c == 4)
                    {
                       decir =multObj + " cuatro " + object;
                       decir.append('s');
                    }
                    if(c == 5)
                    {
                       decir = multObj + " cinco " + object;
                       decir.append('s');
                    }
                    speech->say(decir);
                    d++;
                    decir.clear();
    }
    else
    {
        if(c == 1)
        {
          if(ultimaLetra!= 'a')
          {
            decir = "y un" +  object;
          }
          else decir = "y una" +  object;
        }
        if(c == 2)
        {
           decir =  "y  dos " + object;
           decir.append('s');
        }
        if(c == 3)
        {
          decir = "y tres " + object;
          decir.append('s');
        }
        if(c == 4)
        {
           decir = "y cuatro " + object;
           decir.append('s');
        }
        if(c == 5)
        {
           decir = "y cinco " + object;
           decir.append('s');
        }
        speech->say(decir);
        d++;
        decir.clear();
        }
}

void MainWindow::maximoApariciones()
{
    // Recorremos el vector de los distintos objetos que detecto la camara
    for( int k = 0 ; k < objetosRepr.size() ; k++)
    {
        maxApariciones = 0, jk = 0;
        // Recorremos el vector que tiene las listas detectadas
        for(int i = 0; i < v.size() ; i++)
        {
            QStringList listado = v.at(i);
            // Recorremos la lista ubicada en la p                                                                                                                                                              osicion i del vector de listas
            for( int j = 0; j < listado.size() ; j++ ) {
                if(objetosRepr.at(k)==listado.at(j))
                {
                    jk++; // Contamos la cantidad de apariciones del objeto dentro de una lista
                }
            }
            // Si la cantidad de veces que aparecio el objeto en la lista es mayor a la maxima cantidad
            // de apariciones anterior, que empieza valiendo 0, entonces cambia la cantidad maxima por
            // la cantidad de apariciones que conto en la ultima lista
            if(jk>maxApariciones)
            {
                maxApariciones = jk ;
            }
            jk = 0;
        }

        // Con la maxima cantidad de apariciones obtenias, llamamos al siguiente metodo para ver si la maxima
        // cantidad de apariciones que encontramos, supera el umbral de probabilidad
        chekearApariciones(objetosRepr.at(k),maxApariciones);
    }
}

void MainWindow::chekearApariciones(QString object, int c)
{
//    std::cout<<"Maxima cantidad de apariciones de " << object.toStdString() <<" = " << c << std::endl;
    contar = 0;
    for(int i = 0; i < v.size() ; i++)
    {
            QStringList listado = v.at(i);
            for( int j = 0; j < listado.size() ; j++ ) {
                if(object==listado.at(j))
                {
                    jk++;
                }
            }
            if(jk==c)
            {
                contar++;
            }
            jk = 0;
    }
//        std::cout<<"Porcentaje de aparicion del "<< object.toStdString() << contar/v.size() <<std::endl;
        // Si el porcentaje de apariciones es mayor al 50% entonces llamamos al metodo toSay() que procede a
        // reproducir la cantidad y el nombre del objeto.
        if(contar/v.size()>umbralPorcentaje)
        {
//            std::cout<<"Se encontraron " << maxApariciones << object.toStdString()<<std::endl;
            toSay(object,maxApariciones);
        }
        else
        {
            // Si no supera el umbral de apariciones, bajamos la cantidad maxima de apariciones del objeto
            // y volvemos a llamar a la funcion para que vuelva a chekear si supera con el nuevo valorr.
            if(maxApariciones>0)
            {
                maxApariciones -=1;
                chekearApariciones(object,maxApariciones);
            }
        }
}

// Setear la configuracion de la voz
void MainWindow::speechConfig(int pitch, int rate, int language)
{
    speech->setPitch(pitch);

    speech->setRate(rate);
}

bool MainWindow::detectingObjects()
{
    detecting = false; //cuando termina el timer, se cambia la variable a false para que no se detecten mas objetos
}

void MainWindow::mensajeRecibido(const QString &mensaje)
{
    speech->say(mensaje);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_C: on_startBtn_pressed();    //inicia y pausa la camara
                break;

        case Qt::Key_R: detecting = true; //reproduce el texto contenido en los labels de los objetos detectados
                        QTimer::singleShot(tiempoProcesamiento, this, SLOT(detectingObjects())); // por 2 segundos tomamos los objetos que el programa caputra
                break;

        case Qt::Key_Escape: this->close();
                break;
    }
}
