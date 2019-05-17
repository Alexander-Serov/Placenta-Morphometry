#include "Video.h"
#include<cstring>
#include "core/data/utility/Exception.h"
#ifdef LIBVLC
#include "vlc/libvlc.h"
#include "vlc/vlc.h"
#include"core/dependency/tinythread.h"
#endif
#ifdef FFMPEG
extern "C"
{
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#endif
namespace pop
{
Video::~Video(){
}
#ifdef LIBVLC
namespace Details
{
struct ctx
{
    pop::MatN<2,pop::RGBAUI8> * image;
    tthread::mutex * pmutex;
    int * index;
};

void *lock(void *data, void**p_pixels)
{
    ctx *context = static_cast<ctx*>(data);
    context->pmutex->lock();
    *p_pixels = (unsigned char *)context->image->data();
    return NULL;
}
void display(void *data, void *id)
{
    (void) data;
    assert(id == NULL);
}
void unlock(void *data, void *id, void *const *){
    ctx *context = static_cast<ctx*>(data);
    if(*context->index>=0){
        (*context->index)++;
    }
    context->pmutex->unlock();
    assert(id == NULL); // picture identifier, not needed here
}
}

class VideoVLC::impl
{
    libvlc_instance_t* instance;
    libvlc_media_player_t* mediaPlayer;
    std::string file_playing;
    bool isplaying;
    int my_index;
    struct Details::ctx* context;
    pop::Mat2UI8 imggrey;
    pop::Mat2RGBUI8 imgcolor;

public:
    bool _isfile;
    impl(){
        instance = libvlc_new(0,NULL);
        mediaPlayer = NULL;
        context = new  Details::ctx;
        context->pmutex = new tthread::mutex();
        context->image = new pop::MatN<2,pop::RGBAUI8>(10,10);
        context->index = new int(-1);
        my_index = -1;
        isplaying = false;
        _isfile =true;
    }
    void release(){
        if(mediaPlayer!=NULL){
            if(libvlc_media_player_is_playing(mediaPlayer))
                libvlc_media_player_stop(mediaPlayer);
            libvlc_media_player_release(mediaPlayer);
            mediaPlayer = NULL;
        }
    }

    ~impl(){
        release();
        delete context->pmutex;
        delete context->image;
        delete context->index;
        delete context;
        libvlc_release(instance);

    }

    void copy(const VideoVLC::impl & video)
    {
        if(video.isplaying==true){
            this->open(video.file_playing);
        }
    }
    bool tryOpen(const std::string & path){
        if(path=="")
            return false;
        libvlc_media_t* media = NULL;
        bool isfile = BasicUtility::isFile(path);
        if(isfile==true){
#if Pop_OS==2
            media = libvlc_media_new_path(instance, BasicUtility::replaceSlashByAntiSlash(path).c_str());
#else
            media = libvlc_media_new_path(instance, path.c_str());
#endif
            _isfile =true;

        }
        else{
            media = libvlc_media_new_location(instance,path.c_str() );
            _isfile =true;
        }
        if(media!=NULL){
            file_playing = path;
            isplaying    = true;
            mediaPlayer = libvlc_media_player_new(instance);
            libvlc_media_player_set_media( mediaPlayer, media);
            libvlc_media_release (media);
            media =NULL;
            libvlc_media_player_play(mediaPlayer);
            libvlc_video_set_callbacks(mediaPlayer, Details::lock, Details::unlock, Details::display, context);
            libvlc_video_set_format(mediaPlayer, "RV32", context->image->sizeJ(), context->image->sizeI(), context->image->sizeJ()*4);
#if Pop_OS==2
            Sleep(2000);
#endif
#if Pop_OS==1
            sleep(2);
#endif
            if(libvlc_media_player_is_playing(mediaPlayer)){
                libvlc_media_player_stop(mediaPlayer);
                libvlc_media_player_release(mediaPlayer);
                mediaPlayer =NULL;
                return true;
            }
            else{
                libvlc_media_player_stop(mediaPlayer);
                libvlc_media_player_release(mediaPlayer);
                mediaPlayer =NULL;
                return false;
            }
        }else
            return false;
    }

    bool open(const std::string & path)
    {
        release();
        if(path=="")
            return false;
        libvlc_media_t* media = NULL;
        bool isfile = BasicUtility::isFile(path);
        if(isfile==true){
#if Pop_OS==2
            media = libvlc_media_new_path(instance, BasicUtility::replaceSlashByAntiSlash(path).c_str());
#else
            media = libvlc_media_new_path(instance, path.c_str());
#endif
            _isfile=true;
        }
        else{
            media = libvlc_media_new_location(instance,path.c_str() );
            _isfile=false;
        }
        if(media!=NULL){
            file_playing = path;
            mediaPlayer = libvlc_media_player_new(instance);
            libvlc_media_player_set_media( mediaPlayer, media);
            libvlc_media_release (media);
            libvlc_media_player_play(mediaPlayer);
            libvlc_video_set_callbacks(mediaPlayer, Details::lock, Details::unlock, Details::display, context);
            libvlc_video_set_format(mediaPlayer, "RV32", context->image->sizeJ(), context->image->sizeI(), context->image->sizeJ()*4);

            unsigned int w=0,h=0;
            bool find=false;
            int numbertest=0;
            do{

                for(int i=0;i<10;i++){
                    if(libvlc_video_get_size( mediaPlayer, i, &w, &h )==0)
                        if(w>0&&h>0){
                            i=5;
                            find =true;

                        }
                }
                numbertest++;
                if(find==false){
#if Pop_OS==2
                    Sleep(2000);
#endif
#if Pop_OS==1
                    sleep(2);
#endif
                }
            }while(find==false&&numbertest<10);
            if(numbertest<20){
                isplaying    = true;
                libvlc_media_player_stop(mediaPlayer);
                libvlc_media_player_release(mediaPlayer);
                mediaPlayer =NULL;
//                std::cout<<h<<std::endl;
//                std::cout<<w<<std::endl;
                context->image->resize(h,w);
                media = libvlc_media_new_path(instance, path.c_str());
                if(isfile==true){
#if Pop_OS==2
                    media = libvlc_media_new_path(instance, BasicUtility::replaceSlashByAntiSlash(path).c_str());
#else
                    media = libvlc_media_new_path(instance, path.c_str());
#endif
                }
                else
                    media = libvlc_media_new_location(instance,path.c_str() );
                mediaPlayer = libvlc_media_player_new(instance);
                libvlc_media_player_set_media( mediaPlayer, media);
                libvlc_media_release (media);
                libvlc_media_player_play(mediaPlayer);
                libvlc_video_set_callbacks(mediaPlayer, Details::lock, Details::unlock, Details::display, context);
                libvlc_video_set_format(mediaPlayer, "RV32", context->image->sizeJ(), context->image->sizeI(), context->image->sizeJ()*4);
                *(context->index) =0;
//                std::cout<<"return true"<<std::endl;
#if Pop_OS==2
                    Sleep(1000);
#endif
#if Pop_OS==1
                    sleep(1);
#endif
                return true;
            }else{
                libvlc_media_player_stop(mediaPlayer);
                libvlc_media_player_release(mediaPlayer);
                mediaPlayer =NULL;
                return false;
            }
        }else{
            return false;
        }
    }
    bool grabMatrixGrey(){
        if(isplaying==true){
            while(my_index==*context->index){
                if(_isfile==true&&libvlc_media_player_is_playing(mediaPlayer)==false){
                    isplaying = false;
                    return false;
                }
                if(_isfile==false){
#if Pop_OS==2
                    Sleep(10);
#endif
#if Pop_OS==1
                    usleep(10000);
#endif
                }
            }
            my_index=*context->index;
            return true;
        }else
        {
            return false;
        }
    }
    bool grabMatrixRGB(){
        if(isplaying==true){
            while(my_index==*context->index){
#if Pop_OS==2
                Sleep(40);
#endif
#if Pop_OS==1
                usleep(40000);
#endif
            }
            my_index=*context->index;
            return true;
        }else
        {
            return false;
        }
    }
    Mat2RGBUI8 & retrieveMatrixRGB(){
        context->pmutex->lock();
        imgcolor = *context->image;
        context->pmutex->unlock();
        return imgcolor;
    }

    Mat2UI8 & retrieveMatrixGrey(){
        context->pmutex->lock();
        imggrey = *context->image;
        context->pmutex->unlock();
        return imggrey;
    }

};
//libvlc_instance_t* VideoVLC::impl::instance = libvlc_new(0,NULL);
VideoVLC::VideoVLC()
{
    _pimpl = new   impl;
}
VideoVLC::VideoVLC(const VideoVLC & v)
{
    _pimpl = new   impl;
    _pimpl->copy(* (v._pimpl));
}

VideoVLC::~VideoVLC()
{
    delete _pimpl;
}
bool VideoVLC::open(const std::string & filename)throw(pexception){
    //    delete _pimpl;
    //    _pimpl = new   impl;
    return _pimpl->open(filename);
}
bool VideoVLC::tryOpen(const std::string & filename){
    return _pimpl->tryOpen(filename);
}
bool VideoVLC::grabMatrixGrey(){
    return _pimpl->grabMatrixGrey();
}
Mat2UI8& VideoVLC::retrieveMatrixGrey(){
    return _pimpl->retrieveMatrixGrey();
}
bool VideoVLC::isFile()const{
    return _pimpl->_isfile;
}
bool VideoVLC::grabMatrixRGB(){
    return _pimpl->grabMatrixRGB();
}
Mat2RGBUI8& VideoVLC::retrieveMatrixRGB(){
    return _pimpl->retrieveMatrixRGB();
}
#endif
#ifdef FFMPEG
class VideoFFMPEG::impl
{
    static bool init_global;
    bool init_local;
    AVFormatContext* context;
    AVCodecContext* ccontext;
    AVFormatContext* oc;
    AVPacket packet;
    AVStream* stream;
    AVCodecContext * pCodecCtx;
    AVCodec *codec;
    int video_stream_index;

    AVFrame* picture ;
    AVFrame* picture_grey ;
    uint8_t * picture_grey_buf ;
    AVFrame* picture_RGB ;
    uint8_t * picture_RGB_buf ;
    struct SwsContext *ctx;
    bool rgb;
    pop::Mat2RGBUI8 imgcolor;
    pop::Mat2UI8 imggrey;

public:
    std::string file_playing;
    bool isplaying;

    impl(){
        if(init_global==false){
            av_register_all();
            avformat_network_init();
            init_global =true;
        }
        isplaying =false;
        init_local=false;
        context=NULL;
        ccontext=NULL;
        oc=NULL;
        stream=NULL;
        pCodecCtx=NULL;
        codec=NULL;
        video_stream_index=0;

        picture=NULL ;
        picture_grey=NULL ;
        picture_grey_buf=NULL ;
        picture_RGB=NULL ;
        picture_RGB_buf=NULL ;
        packet.data=NULL;
        ctx =NULL;

    }
    static bool tryOpen(const std::string & str)
    {
        AVFormatContext* contexttemp=NULL;
        if(avformat_open_input(&contexttemp,str.c_str() ,NULL,NULL) != 0){
            return false;
        }
        if(contexttemp!=NULL)
            avformat_close_input(&contexttemp);
        return true;
    }
    void copy(const VideoFFMPEG::impl & video)
    {
        if(video.isplaying==true){
            this->open(video.file_playing);
        }
    }

    bool open(const std::string & str)throw(pexception)
    {
        if(init_local==true)
            release();
        init_local=true;
        context = avformat_alloc_context();
        ccontext = avcodec_alloc_context3(NULL);
        //TODO multithreading if no god rtsp infinite loop
        if(avformat_open_input(&context,str.c_str() ,NULL,NULL) != 0){
            throw  pop::pexception("Open failure: "+str);
        }
        if(avformat_find_stream_info(context,NULL) < 0){
            throw pexception("Open failure: "+str);

        }
        //search video stream
        for(unsigned int i =0;i<context->nb_streams;i++){
            if(context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
                video_stream_index = i;
        }


        //open output file
        oc = avformat_alloc_context();

        //start reading packets from stream and write them to file
        av_read_play(context);//play RTSP


        // Get a pointer to the codec context for the video stream
        pCodecCtx =context->streams[video_stream_index]->codec;

        // Find the decoder for the video stream

        codec = avcodec_find_decoder(pCodecCtx->codec_id);
        if (!codec) {
            throw(pexception("No codec"));
            return false;
        }

        avcodec_get_context_defaults3(ccontext, codec);
        avcodec_copy_context(ccontext,context->streams[video_stream_index]->codec);

        if(codec->capabilities&CODEC_CAP_TRUNCATED)
            ccontext->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

        if (avcodec_open2(ccontext, codec,NULL) < 0) {
            throw(pexception("Cannot find the codec"));
            return false;
        }

        isplaying =true;
        file_playing = str;
        return true;
    }
    bool grabMatrixGrey(){

        if(isplaying==true){
            int check=0;
            int result=-1;
            int try_read=0;
            do{
                if(packet.data!=NULL)
                    av_free_packet(&packet);

                av_init_packet(&packet);
                //packet.size=0;
                if(av_read_frame(context,&packet)>=0){
                    check =0;
                    result=-1;
                    if(packet.stream_index == video_stream_index){//packet is video

                        if(picture_grey == NULL)
                        {//create stream in file
                            if(picture!=NULL)
                                av_free(picture);
                            stream = avformat_new_stream(oc,context->streams[video_stream_index]->codec->codec);
                            avcodec_copy_context(stream->codec,context->streams[video_stream_index]->codec);
                            stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio;
                            picture = avcodec_alloc_frame();
                            picture_grey = avcodec_alloc_frame();
                            int size2 = avpicture_get_size(PIX_FMT_GRAY8, ccontext->width, ccontext->height);
                            picture_grey_buf = (uint8_t*)(av_malloc(size2));
                            avpicture_fill((AVPicture *) picture_grey, picture_grey_buf, PIX_FMT_GRAY8, ccontext->width, ccontext->height);

                        }

                        packet.stream_index = stream->id;
                        result = avcodec_decode_video2(ccontext, picture, &check, &packet);
                    }
                }
                else{
                    try_read++;
                    if(try_read>100)
                        check=1;
                }
            }while((check==0||result<0)&&try_read<=100);
            rgb =false;
            if(try_read>100)
                return false;
            else
                return true;
        }else{
            return false;
        }
    }
    bool grabMatrixRGB(){
        if(isplaying==true){
            int check=0;
            int try_read=0;
            int result=-1;
            do{
                if(packet.data!=NULL)
                    av_free_packet(&packet);
                av_init_packet(&packet);

                if(av_read_frame(context,&packet)>=0){
                    check =0;
                    result=-1;
                    if(packet.stream_index == video_stream_index){//packet is video
                        if(picture_RGB == NULL)
                        {//create stream in file
                            if(picture!=NULL)
                                av_free(picture);
                            stream = avformat_new_stream(oc,context->streams[video_stream_index]->codec->codec);
                            avcodec_copy_context(stream->codec,context->streams[video_stream_index]->codec);
                            stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio;
                            picture = avcodec_alloc_frame();
                            picture_RGB = avcodec_alloc_frame();
                            int size2 = avpicture_get_size(PIX_FMT_RGB24, ccontext->width, ccontext->height);
                            picture_RGB_buf = (uint8_t*)(av_malloc(size2));
                            result=avpicture_fill((AVPicture *) picture_RGB, picture_RGB_buf, PIX_FMT_RGB24, ccontext->width, ccontext->height);

                        }
                        packet.stream_index = stream->id;
                        result=avcodec_decode_video2(ccontext, picture, &check, &packet);
                    }
                    //                std::cout << "Bytes decoded " << result << " check " << check << std::endl;
                    //                std::cout << "Line size "<<picture->linesize[0]<<std::endl;
                    //                std::cout << "ccontext->width "<<ccontext->width*ccontext->height<<std::endl;
                }
                else{
                    try_read++;
                    if(try_read>100)
                        check=1;
                }
            }while(check==0||result<0);
            rgb =true;
            if(try_read>100)
                return false;
            else
                return true;
        }else{
            return false;
        }
    }
    Mat2RGBUI8  &retrieveMatrixRGB(){
        if(isplaying==true){
            imgcolor.resize(ccontext->height,ccontext->width);
            if(ctx==NULL)
                ctx= sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width,
                                    pCodecCtx->height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
            sws_scale(ctx, picture->data, picture->linesize, 0, pCodecCtx->height,
                      picture_RGB->data, picture_RGB->linesize);

            unsigned char * data =reinterpret_cast<unsigned char *> (imgcolor.data());
            std::copy(picture_RGB->data[0],picture_RGB->data[0]+3*ccontext->height*ccontext->width,data);

            return imgcolor;
        }else{
            return imgcolor;
        }
    }

    Mat2UI8  &retrieveMatrixGrey(){
        if(isplaying==true){
            imggrey.resize(ccontext->height,ccontext->width);
            if(ctx==NULL)
                ctx=  sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width,
                                     pCodecCtx->height, PIX_FMT_GRAY8, SWS_BICUBIC, NULL, NULL, NULL);
            sws_scale(ctx, picture->data, picture->linesize, 0, pCodecCtx->height,
                      picture_grey->data, picture_grey->linesize);
            std::copy(picture_grey->data[0],picture_grey->data[0]+ccontext->height*ccontext->width,imggrey.begin());

            return imggrey;
        }else{
            return imggrey;
        }
    }
    void release(){
        isplaying =false;
        if(packet.data!=NULL)
            av_free_packet(&packet);
        if(picture!=NULL)
            av_free(picture);
        if(picture_grey!=NULL)
            av_free(picture_grey);
        if(picture_grey_buf!=NULL)
            av_free(picture_grey_buf);
        if(picture_RGB!=NULL)
            av_free(picture_RGB);
        if(picture_RGB_buf!=NULL)
            av_free(picture_RGB_buf);
        if(oc!=NULL)
            avformat_free_context(oc);
        // Close the codec
        if(ccontext!=NULL)
            avcodec_close(ccontext);
        // Close the video file
        if(context!=NULL)
            avformat_close_input(&context);
        if(ctx!=NULL)
            sws_freeContext(ctx);
        video_stream_index=0;
        init_local=false;
        context=NULL;
        ccontext=NULL;
        oc=NULL;
        stream=NULL;
        pCodecCtx=NULL;
        codec=NULL;
        picture_grey=NULL ;
        picture_grey_buf=NULL ;
        picture_RGB=NULL ;
        picture_RGB_buf=NULL ;
        picture=NULL ;
        packet.data=NULL;
        ctx =NULL;
    }

    ~impl()
    {
        if(init_local==true)
            release();
    }

};
bool VideoFFMPEG::impl::init_global = false;
VideoFFMPEG::VideoFFMPEG()
{
    _pimpl = new   impl;
}
VideoFFMPEG::VideoFFMPEG(const VideoFFMPEG & v)
{
    _pimpl = new   impl;
    _pimpl->copy(* (v._pimpl));
}



VideoFFMPEG::~VideoFFMPEG()
{
    delete _pimpl;
}
bool VideoFFMPEG::open(const std::string & filename)throw(pexception){

    return _pimpl->open(filename);
}
bool VideoFFMPEG::tryOpen(const std::string & filename){
    return _pimpl->tryOpen(filename);
}
bool VideoFFMPEG::grabMatrixGrey(){
    return _pimpl->grabMatrixGrey();
}

Mat2UI8& VideoFFMPEG::retrieveMatrixGrey(){
    return _pimpl->retrieveMatrixGrey();
}
bool VideoFFMPEG::grabMatrixRGB(){
    return _pimpl->grabMatrixRGB();
}

Mat2RGBUI8& VideoFFMPEG::retrieveMatrixRGB(){
    return _pimpl->retrieveMatrixRGB();
}
#endif
}
