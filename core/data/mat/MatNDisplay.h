/******************************************************************************\
|*                   Population library for C++ X.X.X                         *|
|*----------------------------------------------------------------------------*|
The Population License is similar to the MIT license in adding this clause:
for any writing public or private that has resulted from the use of the
software population, the reference of this book "Population library, 2012,
Vincent Tariel" shall be included in it.

So, the terms of the Population License are:

Copyright © 2012, Tariel Vincent

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software and for any writing
public or private that has resulted from the use of the software population,
the reference of this book "Population library, 2012, Vincent Tariel" shall
be included in it.

The Software is provided "as is", without warranty of any kind, express or
implied, including but not limited to the warranties of merchantability,
fitness for a particular purpose and noninfringement. In no event shall the
authors or copyright holders be liable for any claim, damages or other
liability, whether in an action of contract, tort or otherwise, arising
from, out of or in connection with the software or the use or other dealings
in the Software.
\***************************************************************************/

#ifndef MatNDISPLAY_H
#define MatNDISPLAY_H

#include"core/data/mat/MatN.h"
#include"core/data/mat/Mat2x.h"

#include"core/algorithm/Convertor.h"
#include"core/algorithm/GeometricalTransformation.h"
#include"core/algorithm/Processing.h"
namespace pop{
class POP_EXPORTS MatNDisplayInterface{

public:

    virtual ~MatNDisplayInterface ();
    MatNDisplayInterface ();
    MatNDisplayInterface (const MatNDisplayInterface &disp);
    MatNDisplayInterface & 	operator= (const MatNDisplayInterface &disp);
    virtual MatNDisplayInterface & display(const MatN<2, RGBUI8 > &img);
    virtual MatNDisplayInterface & display(const MatN<2, RGBAUI8 > &img);
    virtual MatNDisplayInterface & display(const MatN<2, UI8 > &img);
    virtual operator  bool () const;
    virtual bool 	is_empty () const;
    virtual bool 	is_closed () const;
    virtual bool 	is_resized () const;
    virtual bool 	is_moved () const;
    virtual bool 	is_event () const;
    virtual bool 	is_fullscreen () const;
    virtual bool 	is_key () const;
    virtual bool 	is_key (const unsigned int keycode) const;
    virtual bool 	is_key (const char *const keycode) const;
    virtual bool 	is_key_sequence (const unsigned int *const keycodes_sequence, const unsigned int length, const  bool remove_sequence=false);
    virtual bool 	is_keyESC () const;
    virtual bool 	is_keyF1 () const;
    virtual bool 	is_keyF2 () const;
    virtual bool 	is_keyF3 () const;
    virtual bool 	is_keyF4 () const;
    virtual bool 	is_keyF5 () const;
    virtual bool 	is_keyF6 () const;
    virtual bool 	is_keyF7 () const;
    virtual bool 	is_keyF8 () const;
    virtual bool 	is_keyF9 () const;
    virtual bool 	is_keyF10 () const;
    virtual bool 	is_keyF11 () const;
    virtual bool 	is_keyF12 () const;
    virtual bool 	is_keyPAUSE () const;
    virtual bool 	is_key1 () const;
    virtual bool 	is_key2 () const;
    virtual bool 	is_key3 () const;
    virtual bool 	is_key4 () const;
    virtual bool 	is_key5 () const;
    virtual bool 	is_key6 () const;
    virtual bool 	is_key7 () const;
    virtual bool 	is_key8 () const;
    virtual bool 	is_key9 () const;
    virtual bool 	is_key0 () const;
    virtual bool 	is_keyBACKSPACE () const;
    virtual bool 	is_keyINSERT () const;
    virtual bool 	is_keyHOME () const;
    virtual bool 	is_keyPAGEUP () const;
    virtual bool 	is_keyTAB () const;
    virtual bool 	is_keyQ () const;
    virtual bool 	is_keyW () const;
    virtual bool 	is_keyE () const;
    virtual bool 	is_keyR () const;
    virtual bool 	is_keyT () const;
    virtual bool 	is_keyY () const;
    virtual bool 	is_keyU () const;
    virtual bool 	is_keyI () const;
    virtual bool 	is_keyO () const;
    virtual bool 	is_keyP () const;
    virtual bool 	is_keyDELETE () const;
    virtual bool 	is_keyEND () const;
    virtual bool 	is_keyPAGEDOWN () const;
    virtual bool 	is_keyCAPSLOCK () const;
    virtual bool 	is_keyA () const;
    virtual bool 	is_keyS () const;
    virtual bool 	is_keyD () const;
    virtual bool 	is_keyF () const;
    virtual bool 	is_keyG () const;
    virtual bool 	is_keyH () const;
    virtual bool 	is_keyJ () const;
    virtual bool 	is_keyK () const;
    virtual bool 	is_keyL () const;
    virtual bool 	is_keyENTER () const;
    virtual bool 	is_keySHIFTLEFT () const;
    virtual bool 	is_keyZ () const;
    virtual bool 	is_keyX () const;
    virtual bool 	is_keyC () const;
    virtual bool 	is_keyV () const;
    virtual bool 	is_keyB () const;
    virtual bool 	is_keyN () const;
    virtual bool 	is_keyM () const;
    virtual bool 	is_keySHIFTRIGHT () const;
    virtual bool 	is_keyARROWUP () const;
    virtual bool 	is_keyCTRLLEFT () const;
    virtual bool 	is_keyAPPLEFT () const;
    virtual bool 	is_keyALT () const;
    virtual bool 	is_keySPACE () const;
    virtual bool 	is_keyALTGR () const;
    virtual bool 	is_keyAPPRIGHT () const;
    virtual bool 	is_keyMENU () const;
    virtual bool 	is_keyCTRLRIGHT () const;
    virtual bool 	is_keyARROWLEFT () const;
    virtual bool 	is_keyARROWDOWN () const;
    virtual bool 	is_keyARROWRIGHT () const;
    virtual bool 	is_keyPAD0 () const;
    virtual bool 	is_keyPAD1 () const;
    virtual bool 	is_keyPAD2 () const;
    virtual bool 	is_keyPAD3 () const;
    virtual bool 	is_keyPAD4 () const;
    virtual bool 	is_keyPAD5 () const;
    virtual bool 	is_keyPAD6 () const;
    virtual bool 	is_keyPAD7 () const;
    virtual bool 	is_keyPAD8 () const;
    virtual bool 	is_keyPAD9 () const;
    virtual bool 	is_keyPADADD () const;
    virtual bool 	is_keyPADSUB () const;
    virtual bool 	is_keyPADMUL () const;
    virtual bool 	is_keyPADDIV () const;
    virtual int 	width () const;
    virtual int 	height () const;
    virtual unsigned int 	normalization () const;
    virtual const char * 	title () const;
    virtual int 	window_width () const;
    virtual int 	window_height () const;
    virtual int 	window_x () const;
    virtual int 	window_y () const;
    virtual int 	mouse_x () const;
    virtual int 	mouse_y () const;
    virtual unsigned int 	button () const;
    virtual int 	wheel () const;
    virtual unsigned int 	key (const unsigned int pos=0) const;
    virtual unsigned int 	released_key (const unsigned int pos=0) const;
    virtual float 	frames_per_second ();
    virtual int 	screen_width ();
    virtual int 	screen_height ();
    virtual unsigned int 	keycode (const char *const keycode);
    virtual MatNDisplayInterface & 	show ();
    virtual MatNDisplayInterface & 	close ();
    virtual MatNDisplayInterface & 	move (const int pos_x, const int pos_y);
    virtual MatNDisplayInterface & 	resize (const  bool force_redraw=true);
    virtual MatNDisplayInterface & 	resize (const int width, const int height, const bool force_redraw=true);
    virtual MatNDisplayInterface & 	resize (const MatNDisplayInterface &disp, const bool force_redraw=true);
    virtual MatNDisplayInterface & 	set_normalization (const unsigned int normalization);
    virtual MatNDisplayInterface & 	set_title (const char *const format,...);
    virtual MatNDisplayInterface & 	set_fullscreen (const bool is_fullscreen, const bool force_redraw=true);
    virtual MatNDisplayInterface & 	toggle_fullscreen (const bool force_redraw=true);
    virtual MatNDisplayInterface & 	show_mouse ();
    virtual MatNDisplayInterface & 	hide_mouse ();
    virtual MatNDisplayInterface & 	set_mouse (const int pos_x, const int pos_y);
    virtual MatNDisplayInterface & 	set_button ();
    virtual MatNDisplayInterface & 	set_button (const unsigned int button, const bool is_pressed=true);
    virtual MatNDisplayInterface & 	set_wheel ();
    virtual MatNDisplayInterface & 	set_wheel (const int amplitude);
    virtual MatNDisplayInterface & 	set_key ();
    virtual MatNDisplayInterface & 	set_key (const unsigned int keycode, const bool is_pressed=true);
    virtual MatNDisplayInterface & 	flush ();

    virtual MatNDisplayInterface & 	waitTime ();
    virtual MatNDisplayInterface & 	waitTime (const unsigned int milliseconds);
    virtual MatNDisplayInterface & 	paint ();
    virtual void 	waitTime (MatNDisplayInterface &disp1);
    virtual void 	waitTime (MatNDisplayInterface &disp1, MatNDisplayInterface &disp2);
    virtual void 	waitTime (MatNDisplayInterface &disp1, MatNDisplayInterface &disp2, MatNDisplayInterface &disp3);
    virtual void 	waitTime (MatNDisplayInterface &disp1, MatNDisplayInterface &disp2, MatNDisplayInterface &disp3, MatNDisplayInterface &disp4);
    virtual void 	waitTime (MatNDisplayInterface &disp1, MatNDisplayInterface &disp2, MatNDisplayInterface &disp3, MatNDisplayInterface &disp4, MatNDisplayInterface &disp5);
    virtual void 	wait_all ();
};
#ifndef NOCIMG
/*! \ingroup Matrix
* \defgroup MatNDisplay  MatNDisplay
* \brief class to display matrices in windows handling mouse and keyboard events
*/

class POP_EXPORTS MatNDisplay{
private:
private:
    class impl;
    impl *_pImpl;


public:
    /*!
     * \class pop::MatNDisplay
     * \brief a window which can display pop::MatN matrices and handles mouse and keyboard events
     * \author Tariel Vincent
     * \ingroup MatNDisplay
     *
     * As the implementation is similar to the class CImgDisplay,   for the documentation see http://cimg.sourceforge.net/reference/structcimg__library_1_1CImgDisplay.html .
     * \code
    Mat2UI8 img;
    img.load("../Lena.bmp");
    MatNDisplay disp;
    while(1==1){
        img = Processing::erosion(img,1);
        disp.display(img);//display the current image
    }
     * \endcode

*/
    ~MatNDisplay ();
    MatNDisplay ();
   // MatNDisplay (const unsigned int width, const unsigned int height, const char *const title=0, const unsigned int normalization=3, const bool is_fullscreen=false, const bool is_closed=false);



    MatNDisplay (const MatNDisplay &disp);
    MatNDisplay & 	operator= (const MatNDisplay &disp);
    operator bool () const;
    bool 	is_empty () const;
    bool 	is_closed () const;
    bool 	is_resized () const;
    bool 	is_moved () const;
    bool 	is_event () const;
    bool 	is_fullscreen () const;
    bool 	is_key () const;
    bool 	is_key (const unsigned int keycode) const;
    bool 	is_key (const char *const keycode) const;
    bool 	is_key_sequence (const unsigned int *const keycodes_sequence, const unsigned int length, const bool remove_sequence=false);
    bool 	is_keyESC () const;
    bool 	is_keyF1 () const;
    bool 	is_keyF2 () const;
    bool 	is_keyF3 () const;
    bool 	is_keyF4 () const;
    bool 	is_keyF5 () const;
    bool 	is_keyF6 () const;
    bool 	is_keyF7 () const;
    bool 	is_keyF8 () const;
    bool 	is_keyF9 () const;
    bool 	is_keyF10 () const;
    bool 	is_keyF11 () const;
    bool 	is_keyF12 () const;
    bool 	is_keyPAUSE () const;
    bool 	is_key1 () const;
    bool 	is_key2 () const;
    bool 	is_key3 () const;
    bool 	is_key4 () const;
    bool 	is_key5 () const;
    bool 	is_key6 () const;
    bool 	is_key7 () const;
    bool 	is_key8 () const;
    bool 	is_key9 () const;
    bool 	is_key0 () const;
    bool 	is_keyBACKSPACE () const;
    bool 	is_keyINSERT () const;
    bool 	is_keyHOME () const;
    bool 	is_keyPAGEUP () const;
    bool 	is_keyTAB () const;
    bool 	is_keyQ () const;
    bool 	is_keyW () const;
    bool 	is_keyE () const;
    bool 	is_keyR () const;
    bool 	is_keyT () const;
    bool 	is_keyY () const;
    bool 	is_keyU () const;
    bool 	is_keyI () const;
    bool 	is_keyO () const;
    bool 	is_keyP () const;
    bool 	is_keyDELETE () const;
    bool 	is_keyEND () const;
    bool 	is_keyPAGEDOWN () const;
    bool 	is_keyCAPSLOCK () const;
    bool 	is_keyA () const;
    bool 	is_keyS () const;
    bool 	is_keyD () const;
    bool 	is_keyF () const;
    bool 	is_keyG () const;
    bool 	is_keyH () const;
    bool 	is_keyJ () const;
    bool 	is_keyK () const;
    bool 	is_keyL () const;
    bool 	is_keyENTER () const;
    bool 	is_keySHIFTLEFT () const;
    bool 	is_keyZ () const;
    bool 	is_keyX () const;
    bool 	is_keyC () const;
    bool 	is_keyV () const;
    bool 	is_keyB () const;
    bool 	is_keyN () const;
    bool 	is_keyM () const;
    bool 	is_keySHIFTRIGHT () const;
    bool 	is_keyARROWUP () const;
    bool 	is_keyCTRLLEFT () const;
    bool 	is_keyAPPLEFT () const;
    bool 	is_keyALT () const;
    bool 	is_keySPACE () const;
    bool 	is_keyALTGR () const;
    bool 	is_keyAPPRIGHT () const;
    bool 	is_keyMENU () const;
    bool 	is_keyCTRLRIGHT () const;
    bool 	is_keyARROWLEFT () const;
    bool 	is_keyARROWDOWN () const;
    bool 	is_keyARROWRIGHT () const;
    bool 	is_keyPAD0 () const;
    bool 	is_keyPAD1 () const;
    bool 	is_keyPAD2 () const;
    bool 	is_keyPAD3 () const;
    bool 	is_keyPAD4 () const;
    bool 	is_keyPAD5 () const;
    bool 	is_keyPAD6 () const;
    bool 	is_keyPAD7 () const;
    bool 	is_keyPAD8 () const;
    bool 	is_keyPAD9 () const;
    bool 	is_keyPADADD () const;
    bool 	is_keyPADSUB () const;
    bool 	is_keyPADMUL () const;
    bool 	is_keyPADDIV () const;
    int 	width () const;
    int 	height () const;
    unsigned int 	normalization () const;
    const char * 	title () const;
    int 	window_width () const;
    int 	window_height () const;
    int 	window_x () const;
    int 	window_y () const;
    int 	mouse_x () const;
    int 	mouse_y () const;
    unsigned int 	button () const;
    int 	wheel () const;
    unsigned int 	key (const unsigned int pos=0) const;
    unsigned int 	released_key (const unsigned int pos=0) const;
    float 	frames_per_second ();
    int 	screen_width ();
    int 	screen_height ();
    unsigned int 	keycode (const char *const keycode);

    MatNDisplay & display(const MatN<2, RGBUI8 > &img);
    MatNDisplay & display(const MatN<2, RGBAUI8 > &img);
    MatNDisplay & display(const MatN<2, UI8 > &img);
    MatNDisplay & display(const MatN<2, UI16 > &img);
    MatNDisplay & display(const MatN<2, UI32 > &img);
    MatNDisplay & display(const MatN<2, F64 > &img);

   template<int DIM,typename Type>
    MatNDisplay & display(const MatN<DIM,Type >&  )throw(pexception){
        throw(pexception("Cannot display this image with this pixel type. For 2D, you have to convert it in UI8 or F64 pixel type before to display it"));
    }

    MatNDisplay & 	show ();
    MatNDisplay & 	close ();
    MatNDisplay & 	move (const int pos_x, const int pos_y);
    MatNDisplay & 	resize (const bool force_redraw=true);
    MatNDisplay & 	resize (const int width, const int height, const bool force_redraw=true);
    MatNDisplay & 	resize (const MatNDisplay &disp, const bool force_redraw=true);
    MatNDisplay & 	set_normalization (const unsigned int normalization);
    MatNDisplay & 	set_title (const char *const format,...);
    MatNDisplay & 	set_fullscreen (const bool is_fullscreen, const bool force_redraw=true);
    MatNDisplay & 	toggle_fullscreen (const bool force_redraw=true);
    MatNDisplay & 	show_mouse ();
    MatNDisplay & 	hide_mouse ();
    MatNDisplay & 	set_mouse (const int pos_x, const int pos_y);
    MatNDisplay & 	set_button ();
    MatNDisplay & 	set_button (const unsigned int button, const bool is_pressed=true);
    MatNDisplay & 	set_wheel ();
    MatNDisplay & 	set_wheel (const int amplitude);
    MatNDisplay & 	set_key ();
    MatNDisplay & 	set_key (const unsigned int keycode, const bool is_pressed=true);
    MatNDisplay & 	flush ();

    MatNDisplay & 	waitTime ();
    MatNDisplay & 	waitTime (const unsigned int milliseconds);
    MatNDisplay & 	paint ();
    void 	waitTime (MatNDisplay &disp1);
    void 	waitTime (MatNDisplay &disp1, MatNDisplay &disp2);
    void 	waitTime (MatNDisplay &disp1, MatNDisplay &disp2, MatNDisplay &disp3);
    void 	waitTime (MatNDisplay &disp1, MatNDisplay &disp2, MatNDisplay &disp3, MatNDisplay &disp4);
    void 	waitTime (MatNDisplay &disp1, MatNDisplay &disp2, MatNDisplay &disp3, MatNDisplay &disp4, MatNDisplay &disp5);
    void 	wait_all ();
};
#endif
namespace Private {
template<int Dim, typename Type>
struct Display
{
#ifdef NOCIMG
      static std::vector<MatNDisplayInterface> v_display;
#else
        static std::vector<MatNDisplay> v_display;
#endif

};

#ifdef NOCIMG
template<int Dim, typename Type>
std::vector<MatNDisplayInterface> Display< Dim,  Type>::v_display;
#else
template<int Dim, typename Type>
std::vector<MatNDisplay> Display< Dim,  Type>::v_display;
#endif

}
namespace Private {
    template<int DIM,typename PixelType>
    struct DisplayOutputPixel{static std::string   print( PixelType v){ return BasicUtility::Any2String(v); }};
    template<int DIM>
    struct DisplayOutputPixel<DIM,unsigned char>{ static std::string   print( unsigned char v){ return BasicUtility::Any2String((int)v); }};
}
template<int Dim, typename Type>
void MatN<Dim,Type>::display(const char * title,bool stop_process, bool automaticresize)const throw(pexception){

    MatN<Dim,Type>  img(*this);

    if(DIM==2&&automaticresize ==true){
        VecN<Dim,pop::F64> scale(600./img.getDomain()(0));
        img= GeometricalTransformation::scale(img,scale,0);
    }
    if(DIM==2){

#ifdef NOCIMG
        Private::Display< Dim, Type>::v_display.push_back(MatNDisplayInterface());
#else
        Private::Display< Dim, Type>::v_display.push_back(MatNDisplay());
#endif

        Private::Display< Dim, Type>::v_display.rbegin()->display(img);
        Private::Display< Dim, Type>::v_display.rbegin()->set_title(title);
        if(stop_process==true){
            while (!Private::Display< Dim, Type>::v_display.rbegin()->is_closed()) {
                Private::Display< Dim, Type>::v_display.rbegin()->waitTime();
                int iimg =1.0*Private::Display< Dim, Type>::v_display.rbegin()->mouse_y()/Private::Display< Dim, Type>::v_display.rbegin()->height()*img.getDomain()(0);
                int jimg =1.0*Private::Display< Dim, Type>::v_display.rbegin()->mouse_x()/Private::Display< Dim, Type>::v_display.rbegin()->width()*img.getDomain()(1);
                int i =Private::Display< Dim, Type>::v_display.rbegin()->mouse_y()*this->getDomain()(0)*1.0/img.getDomain()(0);
                int j =Private::Display< Dim, Type>::v_display.rbegin()->mouse_x()*this->getDomain()(1)*1.0/img.getDomain()(1);
                if (Private::Display< Dim, Type>::v_display.rbegin()->button()) {
                    if(img.isValid(iimg,jimg)){
                        std::string t ="i="+ BasicUtility::Any2String(i)+", j="+BasicUtility::Any2String(j)+", f(i,j)="+Private::DisplayOutputPixel<DIM,Type>::print(img(iimg,jimg));
                        Private::Display< Dim, Type>::v_display.rbegin()->set_title(t.c_str());
                    }
                }
            }
        }
    }else if(DIM==3){
        Vec2I32 d;
        d(0)=img.getDomain()(0);
        d(1)=img.getDomain()(1);
        MatN<2, Type> plane(d);
        typename MatN<2, Type>::IteratorEDomain it(plane.getIteratorEDomain());
        VecN<DIM,int> x;
        x(2)=0;
        while(it.next()){
            x(0) = it.x()(0);
            x(1) = it.x()(1);
            plane(it.x())=img(x);
        }

        int index =0;
        std::string t =title;
        if(t=="")
            t = "Press  down(up)-arrow to move in the z axis";
#ifdef NOCIMG
      MatNDisplayInterface main_disp;
#else
        MatNDisplay main_disp;
#endif

        main_disp.display(plane);
        main_disp.set_title(t.c_str());
        main_disp.set_normalization(0);
        while (!main_disp.is_closed() ) {
            if(main_disp.is_keyARROWDOWN())
                index--;
            if(main_disp.is_keyARROWUP())
                index++;
            int xdiv =maximum(1,img.getDomain()(1));
            int ydiv =maximum(1,img.getDomain()(0));
            int zdiv =maximum(1,img.getDomain()(2));
            if (main_disp.button()) {
                t = "i="+BasicUtility::Any2String(main_disp.mouse_x()*this->getDomain()(1)*1.0/xdiv)+
                        ", j="+BasicUtility::Any2String(main_disp.mouse_y()*this->getDomain()(0)*1.0/ydiv)+
                        ", k="+BasicUtility::Any2String(index*this->getDomain()(2)*1.0/zdiv);
                main_disp.set_title(t.c_str());
            }
            if(index>=img.getDomain()[2])
                index = img.getDomain()[2]-1;
            if(index<0)
                index = 0;
            x(2)=index;
            it.init();
            while(it.next()){
                x(0) = it.x()(0);
                x(1) = it.x()(1);
                plane(it.x())=img(x);
            }
            main_disp.display(plane).waitTime(20);
        }
    }
}
}

#endif// MatNDISPLAY_H
