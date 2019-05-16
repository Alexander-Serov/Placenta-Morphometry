#ifndef VIDEO_H
#define VIDEO_H

#include"core/data/mat/MatN.h"
#include"core/data/mat/Mat2x.h"
/// @cond DEV
namespace pop
{

/*! \ingroup Other
* \defgroup Video Video
* \brief  video player frame by frame (ip-camera, avi)
*
* \section InstallExternal External library vlc
*
* OS
* - for linux,  install the vlc library as usual (ubuntu  sudo apt-get install libvlc-dev vlc-nox vlc)
* - for windows, execute the installer package http://www.videolan.org/vlc/download-windows.html and you can find the SDK files in the install directory, i.e. development headers and import libraries.
*
* IDE
* - in qt-creator, you have to uncomment the CONFIG += vlc to use vlv player implementation.
* - in others,
*   -# add preprocessor definitions -DLIBVLC
*   -# add include path C:/Program Files (x86)/VideoLAN/VLC/sdk/include/ for windows
*   -# add library path C:/Program Files (x86)/VideoLAN/VLC for windows
*   -# link shared library -lvlc
*/



class POP_EXPORTS Video
{
public:

    /*!
  \class pop::Video
  \ingroup Video
  \brief simple class to deal with video
  \author Tariel Vincent

   The implementation uses the ffmpeg library or vlc library. In qtcreator, you uncomment this line  CONFIG += vlc or this one CONFIG += ffmpeg. For visual studio,
   you download QT 5.0 and the Visual Studio Add-in 1.2.0 for Qt5 http://qt-project.org/downloads . Then, you can open the project population.pro with the uncommented line.

\code
    Video video;
//    video.open( "rtsp://192.168.30.142/channel1");
    video.open( "/home/vtariel/Bureau/IPCameraRecord.avi");
    MatNDisplay disp;
    while(video.grabMatrixGrey()){
        disp.display(video.retrieveMatrixGrey());
    }
    video.open( "/home/vtariel/Bureau/IPCameraRecord.avi");
    MatNDisplay disp;
    while(video.grabMatrixRGB()){
        disp.display(video.retrieveMatrixRGB());
    }

\endcode
In the following code, I present the integration in QT

\code
    QApplication a(argc, argv);
    QVideo v;
    QLabelVideo l;
    QObject::connect(&v,SIGNAL(nextFrame(pop::Mat2UI8)),&l,SLOT(nextFrame(pop::Mat2UI8)));
    l.show();
    try{
        v.open("rtsp://192.168.30.149/channel1");
    }catch(const pexception & e){
        e.display();
    }
    v.play();
    return a.exec();



::QVideo.h
class QWidgetVideo : public QWidget
{
    Q_OBJECT
public:

    QWidgetVideo(QWidget *parent=0);
public slots:
    void nextFrame(pop::Mat2UI8 img);
protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage qimg;
};

class QVideoEngine : public QObject
{
    Q_OBJECT


public slots:
    void play();
signals:
    void nextFrame(pop::Mat2UI8  img);
private slots:
    void nextFrame();
public:
    QVideoEngine(QObject * parent=0);
    bool open(QString file)throw(pop::pexception);
    void stop();
private:
    bool _isreadable;
    bool _isplay;
    pop::Video _video;
};
::QVideo.cpp
QWidgetVideo::QWidgetVideo(QWidget *parent)
    :QWidget(parent)
{
}
void QWidgetVideo::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawImage(rect(), qimg , qimg.rect());
}
void QWidgetVideo::nextFrame(pop::Mat2UI8 img){

    qimg = pop::ConvertorQImage::toQImage(img);
    qimg =qimg.convertToFormat(QImage::Format_RGB888);
    update();
}
void QVideoEngine::nextFrame(){

    if(_isplay==true&&_video.grabMatrixGrey()){
        emit nextFrame(_video.retrieveMatrixGrey());
    }
}

bool QVideoEngine::open(QString file)throw(pop::pexception){

    try{
        _isreadable = _video.open(file.toStdString());
    }catch(const pop::pexception & e){
    }
    return _isreadable;
}
void QVideoEngine::play(){
    if(_isreadable==true){
        _isplay =true;
    }else{
        _isplay =false;
    }
}
void QVideoEngine::stop(){
    _isplay =false;
}

QVideoEngine::QVideoEngine(QObject * parent)
    :QObject(parent)
{
    _isreadable =false;
    _isplay = false;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    timer->start(50);
}

::main.cpp
    QApplication a(argc, argv);
    QVideoEngine v;
    QWidgetVideo l;
    qRegisterMetaType< pop::Mat2UI8 >("Mat2UI8");
    QObject::connect(&v,SIGNAL(nextFrame(pop::Mat2UI8,int)),&l,SLOT(nextFrame(pop::Mat2UI8,int)));
    l.show();
    try{
        v.open("rtsp://192.168.30.149/channel1");
    }catch(const pexception & e){
        e.whar();
    }
    v.play();
    return a.exec();
\endcode
  */

    /*!
    \brief destructor
    *
    */
    virtual ~Video();
    /*!
    \param   filename IP adress or file path
    \brief open the file or the network stream
    *
    */
    virtual bool open(const std::string & filename)throw(pexception)=0;
    /*!
    \return false  no  frame anymore
    \brief  grab the next frame
    *
    */
    virtual bool grabMatrixGrey()=0;

    /*!
    \return grey Matrix frame
    \brief reads the frame
     *
    */
    virtual Mat2UI8 &retrieveMatrixGrey()=0;


    virtual bool grabMatrixRGB()=0;
    virtual Mat2RGBUI8 &retrieveMatrixRGB()=0;
    virtual bool tryOpen(const std::string & filename)=0;
};
#ifdef LIBVLC
class POP_EXPORTS VideoVLC
{
private:
    class impl;
    impl *_pimpl;
public:
    VideoVLC();
    VideoVLC(const VideoVLC& vlc);
    virtual ~VideoVLC();
    bool open(const std::string & filename)throw(pexception);
    bool grabMatrixGrey();
    Mat2UI8 &retrieveMatrixGrey();
    bool grabMatrixRGB();
    Mat2RGBUI8 &retrieveMatrixRGB();
    bool tryOpen(const std::string & filename);
    bool isFile()const;
};

#endif
#ifdef FFMPEG
class POP_EXPORTS VideoFFMPEG
{
private:
    class impl;
    impl *_pimpl;
public:
    VideoFFMPEG();
    VideoFFMPEG(const VideoFFMPEG& vlc);
    virtual ~VideoFFMPEG();
    bool open(const std::string & filename)throw(pexception);
    bool grabMatrixGrey();
    Mat2UI8 &retrieveMatrixGrey();
    bool grabMatrixRGB();
    Mat2RGBUI8 &retrieveMatrixRGB();
    bool tryOpen(const std::string & filename);
};

#endif
}
/// @endcond
#endif // VIDEO_H
