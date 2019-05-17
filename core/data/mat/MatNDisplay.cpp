#include"core/data/mat/MatNDisplay.h"

#ifndef NOCIMG
#include"cImg/CImg.h"
#endif
namespace pop{


MatNDisplayInterface::~MatNDisplayInterface (){

}

MatNDisplayInterface::MatNDisplayInterface ()

{

}


 MatNDisplayInterface & MatNDisplayInterface::display(const MatN<2, RGBUI8 > &){
    return *this;
 }

 MatNDisplayInterface & MatNDisplayInterface::display(const MatN<2, RGBAUI8 > &){
        return *this;
 }

 MatNDisplayInterface & MatNDisplayInterface::display(const MatN<2, UI8 > &){
        return *this;
 }

MatNDisplayInterface::MatNDisplayInterface(const MatNDisplayInterface &){

}




MatNDisplayInterface & 	MatNDisplayInterface::operator= (const MatNDisplayInterface &){
    return *this;
}

MatNDisplayInterface::operator bool () const{
    return true;
}
bool 	MatNDisplayInterface::is_empty () const{
    return true;
}
bool 	MatNDisplayInterface::is_closed () const{
    return true;
}
bool 	MatNDisplayInterface::is_resized () const{
    return true;
}
bool 	MatNDisplayInterface::is_moved () const{
    return true;
}
bool 	MatNDisplayInterface::is_event () const{
    return true;
}
bool 	MatNDisplayInterface::is_fullscreen () const{
    return true;
}
bool 	MatNDisplayInterface::is_key () const{
    return true;
}
bool 	MatNDisplayInterface::is_key (const unsigned int ) const{
    return true;
}
bool 	MatNDisplayInterface::is_key (const char *const ) const{
    return true;
}
bool 	MatNDisplayInterface::is_key_sequence (const unsigned int *const , const unsigned int , const bool ){
    return true;
}
bool 	MatNDisplayInterface::is_keyESC () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF1 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF2 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF3 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF4 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF5 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF6 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF7 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF8() const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF9 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF10 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF11 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF12 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAUSE () const{
    return true;
}
bool 	MatNDisplayInterface::is_key1 () const{
    return true;
}
bool 	MatNDisplayInterface::is_key2 () const{
    return true;
}
bool 	MatNDisplayInterface::is_key3 () const{
    return true;
}
bool 	MatNDisplayInterface::is_key4 () const{
    return true;
}
bool 	MatNDisplayInterface::is_key5 () const{
    return true;
}
bool 	MatNDisplayInterface::is_key6 () const{
    return true;
}
bool 	MatNDisplayInterface::is_key7 () const{
    return true;
}
bool 	MatNDisplayInterface::is_key8 () const{
    return true;
}
bool 	MatNDisplayInterface::is_key9 () const{
    return true;
}
bool 	MatNDisplayInterface::is_key0 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyBACKSPACE () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyINSERT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyHOME () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAGEUP () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyTAB () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyQ () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyW () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyE () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyR () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyY () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyU () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyI () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyO () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyP () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyDELETE () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyEND () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAGEDOWN () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyCAPSLOCK () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyA () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyS () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyD () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyF () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyG () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyH () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyJ () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyK () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyL () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyENTER () const{
    return true;
}
bool 	MatNDisplayInterface::is_keySHIFTLEFT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyZ () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyX () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyC () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyV () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyB () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyN () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyM () const{
    return true;
}
bool 	MatNDisplayInterface::is_keySHIFTRIGHT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyARROWUP () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyCTRLLEFT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyAPPLEFT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyALT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keySPACE () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyALTGR () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyAPPRIGHT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyMENU () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyCTRLRIGHT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyARROWLEFT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyARROWDOWN () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyARROWRIGHT () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD0 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD1 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD2 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD3 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD4 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD5 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD6 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD7 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD8 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPAD9 () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPADADD () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPADSUB () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPADMUL () const{
    return true;
}
bool 	MatNDisplayInterface::is_keyPADDIV () const{
    return true;
}
int 	MatNDisplayInterface::width () const{
    return  0;
}
int 	MatNDisplayInterface::height () const{
    return 0;
}
unsigned int 	MatNDisplayInterface::normalization () const{
    return 0;
}
const char * 	MatNDisplayInterface::title () const{
    return 0;
}
int 	MatNDisplayInterface::window_width () const{
    return 0;
}
int 	MatNDisplayInterface::window_height () const{
    return 0;
}
int 	MatNDisplayInterface::window_x () const{
    return 0;
}
int 	MatNDisplayInterface::window_y () const{
    return 0;
}
int 	MatNDisplayInterface::mouse_x () const{
    return 0;
}
int 	MatNDisplayInterface::mouse_y () const{
    return 0;
}
unsigned int 	MatNDisplayInterface::button () const{
    return 0;
}
int 	MatNDisplayInterface::wheel () const{
    return 0;
}
unsigned int 	MatNDisplayInterface::key (const unsigned int ) const{
    return 0;
}
unsigned int 	MatNDisplayInterface::released_key (const unsigned int ) const{
    return 0;
}
float 	MatNDisplayInterface::frames_per_second (){
    return 0;
}
int 	MatNDisplayInterface::screen_width (){
    return 0;
}
int 	MatNDisplayInterface::screen_height (){
    return 0;
}
unsigned int 	MatNDisplayInterface::keycode (const char *const ){
    return 0;
}
MatNDisplayInterface & 	MatNDisplayInterface::show (){
    show();
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::close (){
    close();
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::move (const int pos_x, const int pos_y){
    move(pos_x,pos_y);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::resize (const bool force_redraw){
    resize(force_redraw);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::resize (const int width, const int height, const bool force_redraw){
    resize(width,height,force_redraw);
    return *this;
}

MatNDisplayInterface & 	MatNDisplayInterface::resize (const MatNDisplayInterface &disp, const bool force_redraw){
    resize(disp,force_redraw);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_normalization (const unsigned int normalization){
    set_normalization(normalization);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_title (const char *const format,...){
    set_title(format);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_fullscreen (const bool is_fullscreen, const bool force_redraw){
    set_fullscreen(is_fullscreen,force_redraw);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::toggle_fullscreen (const bool force_redraw){
    toggle_fullscreen(force_redraw);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::show_mouse (){
    show_mouse();
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::hide_mouse (){
    hide_mouse();
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_mouse (const int pos_x, const int pos_y){
    set_mouse(pos_x,pos_y);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_button (){
    set_button();
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_button (const unsigned int button, const bool is_pressed){
    set_button(button,  is_pressed);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_wheel (){
    set_wheel();
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_wheel (const int amplitude){
    set_wheel(amplitude);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_key (){
    set_key();
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::set_key (const unsigned int keycode, const bool is_pressed){
    set_key( keycode,  is_pressed);
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::flush (){
    flush();
    return *this;
}

MatNDisplayInterface & 	MatNDisplayInterface::waitTime (){
    return *this;
}
MatNDisplayInterface & 	MatNDisplayInterface::waitTime (const unsigned int ){

    return *this;
}

MatNDisplayInterface & 	MatNDisplayInterface::paint (){
    return *this;
}

void 	MatNDisplayInterface::waitTime (MatNDisplayInterface &){

}
void 	MatNDisplayInterface::waitTime (MatNDisplayInterface &, MatNDisplayInterface &){

}
void 	MatNDisplayInterface::waitTime (MatNDisplayInterface &, MatNDisplayInterface &, MatNDisplayInterface &){

}
void 	MatNDisplayInterface::waitTime (MatNDisplayInterface &, MatNDisplayInterface &, MatNDisplayInterface &, MatNDisplayInterface &){

}
void 	MatNDisplayInterface::waitTime (MatNDisplayInterface &, MatNDisplayInterface &, MatNDisplayInterface &, MatNDisplayInterface &, MatNDisplayInterface &){
}
void 	MatNDisplayInterface::wait_all (){

}

#ifndef NOCIMG
class MatNDisplay::impl
{
public:
    cimg_library::CImgDisplay * display;
};
MatNDisplay::~MatNDisplay (){
    delete _pImpl->display;
    delete _pImpl;
}

MatNDisplay::MatNDisplay ()

{
    _pImpl = new impl();
    _pImpl->display = new cimg_library::CImgDisplay();
}



MatNDisplay & MatNDisplay::display(const MatN<2, RGBUI8 > &img){
    cimg_library::CImg<UI8> temp(img.getDomain()(1),img.getDomain()(0),1,3);
    for(int i =0;i<img.getDomain()[0];i++)
        for(int j =0;j<img.getDomain()[1];j++){
            temp.operator ()(j,i,0,0)=img.operator ()(i,j).r();
            temp.operator ()(j,i,0,1)=img.operator ()(i,j).g();
            temp.operator ()(j,i,0,2)=img.operator ()(i,j).b();
        }

    _pImpl->display->display(temp);
    return *this;
}

MatNDisplay & MatNDisplay::display(const MatN<2, UI16 > &img){
    Mat2UI8 temp = pop::Processing::greylevelRange(img,0,255);
    return display(temp);
}

MatNDisplay & MatNDisplay::display(const MatN<2, UI32 > &img){
    Mat2UI8 temp = pop::Processing::greylevelRange(img,0,255);
    return display(temp);
}

MatNDisplay & MatNDisplay::display(const MatN<2, RGBAUI8 > &img){
    cimg_library::CImg<UI8> temp(img.getDomain()(1),img.getDomain()(0),1,4);
    for(int i =0;i<img.getDomain()[0];i++)
        for(int j =0;j<img.getDomain()[1];j++){
            temp.operator ()(j,i,0,0)=img.operator ()(i,j).r();
            temp.operator ()(j,i,0,1)=img.operator ()(i,j).g();
            temp.operator ()(j,i,0,2)=img.operator ()(i,j).b();
            temp.operator ()(j,i,0,3)=img.operator ()(i,j).a();
        }

    _pImpl->display->display(temp);
    return *this;
}
MatNDisplay & MatNDisplay::display(const MatN<2, UI8 > &img){
    cimg_library::CImg<UI8> temp(img.getDomain()(1),img.getDomain()(0));
    for(int i =0;i<img.getDomain()[0];i++)
        for(int j =0;j<img.getDomain()[1];j++){
            temp.operator ()(j,i)=img.operator ()(i,j);
        }
    _pImpl->display->display(temp);
    return *this;
}
MatNDisplay & MatNDisplay::display(const MatN<2, F64 > &imgf){
     MatN<2, UI8 > img =Processing::greylevelRange(imgf,0,255);
    cimg_library::CImg<UI8> temp(img.getDomain()(1),img.getDomain()(0));
    for(int i =0;i<img.getDomain()[0];i++)
        for(int j =0;j<img.getDomain()[1];j++){
            temp.operator ()(j,i)=img.operator ()(i,j);
        }
    _pImpl->display->display(temp);
    return *this;
}
MatNDisplay::MatNDisplay(const MatNDisplay &disp){
    _pImpl = new impl();
    _pImpl->display = new cimg_library::CImgDisplay(*disp._pImpl->display);
}




MatNDisplay & 	MatNDisplay::operator= (const MatNDisplay &disp){
    _pImpl->display->assign(*(disp._pImpl->display));
    return *this;
}

MatNDisplay::operator bool () const{
    return _pImpl->display->operator bool();
}
bool 	MatNDisplay::is_empty () const{
    return _pImpl->display->is_empty();
}
bool 	MatNDisplay::is_closed () const{
    return _pImpl->display->is_closed();
}
bool 	MatNDisplay::is_resized () const{
    return _pImpl->display->is_resized();
}
bool 	MatNDisplay::is_moved () const{
    return _pImpl->display->is_moved();
}
bool 	MatNDisplay::is_event () const{
    return _pImpl->display->is_event();
}
bool 	MatNDisplay::is_fullscreen () const{
    return _pImpl->display->is_fullscreen();
}
bool 	MatNDisplay::is_key () const{
    return _pImpl->display->is_key();
}
bool 	MatNDisplay::is_key (const unsigned int keycode) const{
    return _pImpl->display->is_key(keycode);
}
bool 	MatNDisplay::is_key (const char *const keycode) const{
    return _pImpl->display->is_key(keycode);
}
bool 	MatNDisplay::is_key_sequence (const unsigned int *const keycodes_sequence, const unsigned int length, const bool remove_sequence){
    return _pImpl->display->is_key_sequence(keycodes_sequence,length,remove_sequence);
}
bool 	MatNDisplay::is_keyESC () const{
    return _pImpl->display->is_keyESC();
}
bool 	MatNDisplay::is_keyF1 () const{
    return _pImpl->display->is_keyF1();
}
bool 	MatNDisplay::is_keyF2 () const{
    return _pImpl->display->is_keyF2();
}
bool 	MatNDisplay::is_keyF3 () const{
    return _pImpl->display->is_keyF3();
}
bool 	MatNDisplay::is_keyF4 () const{
    return _pImpl->display->is_keyF4();
}
bool 	MatNDisplay::is_keyF5 () const{
    return _pImpl->display->is_keyF5();
}
bool 	MatNDisplay::is_keyF6 () const{
    return _pImpl->display->is_keyF6();
}
bool 	MatNDisplay::is_keyF7 () const{
    return _pImpl->display->is_keyF7();
}
bool 	MatNDisplay::is_keyF8() const{
    return _pImpl->display->is_keyF8();
}
bool 	MatNDisplay::is_keyF9 () const{
    return _pImpl->display->is_keyF9();
}
bool 	MatNDisplay::is_keyF10 () const{
    return _pImpl->display->is_keyF10();
}
bool 	MatNDisplay::is_keyF11 () const{
    return _pImpl->display->is_keyF11();
}
bool 	MatNDisplay::is_keyF12 () const{
    return _pImpl->display->is_keyF12();
}
bool 	MatNDisplay::is_keyPAUSE () const{
    return _pImpl->display->is_keyPAUSE();
}
bool 	MatNDisplay::is_key1 () const{
    return _pImpl->display->is_key1();
}
bool 	MatNDisplay::is_key2 () const{
    return _pImpl->display->is_key2();
}
bool 	MatNDisplay::is_key3 () const{
    return _pImpl->display->is_key3();
}
bool 	MatNDisplay::is_key4 () const{
    return _pImpl->display->is_key4();
}
bool 	MatNDisplay::is_key5 () const{
    return _pImpl->display->is_key5();
}
bool 	MatNDisplay::is_key6 () const{
    return _pImpl->display->is_key6();
}
bool 	MatNDisplay::is_key7 () const{
    return _pImpl->display->is_key7();
}
bool 	MatNDisplay::is_key8 () const{
    return _pImpl->display->is_key8();
}
bool 	MatNDisplay::is_key9 () const{
    return _pImpl->display->is_key9();
}
bool 	MatNDisplay::is_key0 () const{
    return _pImpl->display->is_key0();
}
bool 	MatNDisplay::is_keyBACKSPACE () const{
    return _pImpl->display->is_keyBACKSPACE();
}
bool 	MatNDisplay::is_keyINSERT () const{
    return _pImpl->display->is_keyINSERT();
}
bool 	MatNDisplay::is_keyHOME () const{
    return _pImpl->display->is_keyHOME();
}
bool 	MatNDisplay::is_keyPAGEUP () const{
    return _pImpl->display->is_keyPAGEUP();
}
bool 	MatNDisplay::is_keyTAB () const{
    return _pImpl->display->is_keyTAB();
}
bool 	MatNDisplay::is_keyQ () const{
    return _pImpl->display->is_keyQ();
}
bool 	MatNDisplay::is_keyW () const{
    return _pImpl->display->is_keyW();
}
bool 	MatNDisplay::is_keyE () const{
    return _pImpl->display->is_keyE();
}
bool 	MatNDisplay::is_keyR () const{
    return _pImpl->display->is_keyR();
}
bool 	MatNDisplay::is_keyT () const{
    return _pImpl->display->is_keyT();
}
bool 	MatNDisplay::is_keyY () const{
    return _pImpl->display->is_keyY();
}
bool 	MatNDisplay::is_keyU () const{
    return _pImpl->display->is_keyU();
}
bool 	MatNDisplay::is_keyI () const{
    return _pImpl->display->is_keyI();
}
bool 	MatNDisplay::is_keyO () const{
    return _pImpl->display->is_keyO();
}
bool 	MatNDisplay::is_keyP () const{
    return _pImpl->display->is_keyP();
}
bool 	MatNDisplay::is_keyDELETE () const{
    return _pImpl->display->is_keyDELETE();
}
bool 	MatNDisplay::is_keyEND () const{
    return _pImpl->display->is_keyEND();
}
bool 	MatNDisplay::is_keyPAGEDOWN () const{
    return _pImpl->display->is_keyPAGEDOWN();
}
bool 	MatNDisplay::is_keyCAPSLOCK () const{
    return _pImpl->display->is_keyCAPSLOCK();
}
bool 	MatNDisplay::is_keyA () const{
    return _pImpl->display->is_keyA();
}
bool 	MatNDisplay::is_keyS () const{
    return _pImpl->display->is_keyS();
}
bool 	MatNDisplay::is_keyD () const{
    return _pImpl->display->is_keyD();
}
bool 	MatNDisplay::is_keyF () const{
    return _pImpl->display->is_keyF();
}
bool 	MatNDisplay::is_keyG () const{
    return _pImpl->display->is_keyG();
}
bool 	MatNDisplay::is_keyH () const{
    return _pImpl->display->is_keyH();
}
bool 	MatNDisplay::is_keyJ () const{
    return _pImpl->display->is_keyJ();
}
bool 	MatNDisplay::is_keyK () const{
    return _pImpl->display->is_keyK();
}
bool 	MatNDisplay::is_keyL () const{
    return _pImpl->display->is_keyL();
}
bool 	MatNDisplay::is_keyENTER () const{
    return _pImpl->display->is_keyENTER();
}
bool 	MatNDisplay::is_keySHIFTLEFT () const{
    return _pImpl->display->is_keySHIFTLEFT();
}
bool 	MatNDisplay::is_keyZ () const{
    return _pImpl->display->is_keyZ();
}
bool 	MatNDisplay::is_keyX () const{
    return _pImpl->display->is_keyX();
}
bool 	MatNDisplay::is_keyC () const{
    return _pImpl->display->is_keyC();
}
bool 	MatNDisplay::is_keyV () const{
    return _pImpl->display->is_keyV();
}
bool 	MatNDisplay::is_keyB () const{
    return _pImpl->display->is_keyB();
}
bool 	MatNDisplay::is_keyN () const{
    return _pImpl->display->is_keyN();
}
bool 	MatNDisplay::is_keyM () const{
    return _pImpl->display->is_keyM();
}
bool 	MatNDisplay::is_keySHIFTRIGHT () const{
    return _pImpl->display->is_keySHIFTRIGHT();
}
bool 	MatNDisplay::is_keyARROWUP () const{
    return _pImpl->display->is_keyARROWUP();
}
bool 	MatNDisplay::is_keyCTRLLEFT () const{
    return _pImpl->display->is_keyCTRLLEFT();
}
bool 	MatNDisplay::is_keyAPPLEFT () const{
    return _pImpl->display->is_keyAPPLEFT();
}
bool 	MatNDisplay::is_keyALT () const{
    return _pImpl->display->is_keyALT();
}
bool 	MatNDisplay::is_keySPACE () const{
    return _pImpl->display->is_keySPACE();
}
bool 	MatNDisplay::is_keyALTGR () const{
    return _pImpl->display->is_keyALTGR();
}
bool 	MatNDisplay::is_keyAPPRIGHT () const{
    return _pImpl->display->is_keyAPPRIGHT();
}
bool 	MatNDisplay::is_keyMENU () const{
    return _pImpl->display->is_keyMENU();
}
bool 	MatNDisplay::is_keyCTRLRIGHT () const{
    return _pImpl->display->is_keyCTRLRIGHT();
}
bool 	MatNDisplay::is_keyARROWLEFT () const{
    return _pImpl->display->is_keyARROWLEFT();
}
bool 	MatNDisplay::is_keyARROWDOWN () const{
    return _pImpl->display->is_keyARROWDOWN();
}
bool 	MatNDisplay::is_keyARROWRIGHT () const{
    return _pImpl->display->is_keyARROWRIGHT();
}
bool 	MatNDisplay::is_keyPAD0 () const{
    return _pImpl->display->is_keyPAD0();
}
bool 	MatNDisplay::is_keyPAD1 () const{
    return _pImpl->display->is_keyPAD1();
}
bool 	MatNDisplay::is_keyPAD2 () const{
    return _pImpl->display->is_keyPAD2();
}
bool 	MatNDisplay::is_keyPAD3 () const{
    return _pImpl->display->is_keyPAD3();
}
bool 	MatNDisplay::is_keyPAD4 () const{
    return _pImpl->display->is_keyPAD4();
}
bool 	MatNDisplay::is_keyPAD5 () const{
    return _pImpl->display->is_keyPAD5();
}
bool 	MatNDisplay::is_keyPAD6 () const{
    return _pImpl->display->is_keyPAD6();
}
bool 	MatNDisplay::is_keyPAD7 () const{
    return _pImpl->display->is_keyPAD7();
}
bool 	MatNDisplay::is_keyPAD8 () const{
    return _pImpl->display->is_keyPAD8();
}
bool 	MatNDisplay::is_keyPAD9 () const{
    return _pImpl->display->is_keyPAD9();
}
bool 	MatNDisplay::is_keyPADADD () const{
    return _pImpl->display->is_keyPADADD();
}
bool 	MatNDisplay::is_keyPADSUB () const{
    return _pImpl->display->is_keyPADSUB();
}
bool 	MatNDisplay::is_keyPADMUL () const{
    return _pImpl->display->is_keyPADMUL();
}
bool 	MatNDisplay::is_keyPADDIV () const{
    return _pImpl->display->is_keyPADDIV();
}
int 	MatNDisplay::width () const{
    return _pImpl->display->width();
}
int 	MatNDisplay::height () const{
    return _pImpl->display->height();
}
unsigned int 	MatNDisplay::normalization () const{
    return _pImpl->display->normalization();
}
const char * 	MatNDisplay::title () const{
    return _pImpl->display->title();
}
int 	MatNDisplay::window_width () const{
    return _pImpl->display->window_width();
}
int 	MatNDisplay::window_height () const{
    return _pImpl->display->window_height();
}
int 	MatNDisplay::window_x () const{
    return _pImpl->display->window_x();
}
int 	MatNDisplay::window_y () const{
    return _pImpl->display->window_y();
}
int 	MatNDisplay::mouse_x () const{
    return _pImpl->display->mouse_x();
}
int 	MatNDisplay::mouse_y () const{
    return _pImpl->display->mouse_y();
}
unsigned int 	MatNDisplay::button () const{
    return _pImpl->display->button();
}
int 	MatNDisplay::wheel () const{
    return _pImpl->display->wheel();
}
unsigned int 	MatNDisplay::key (const unsigned int pos) const{
    return _pImpl->display->key(pos);
}
unsigned int 	MatNDisplay::released_key (const unsigned int pos) const{
    return _pImpl->display->released_key(pos);
}
float 	MatNDisplay::frames_per_second (){
    return _pImpl->display->frames_per_second();
}
int 	MatNDisplay::screen_width (){
    return _pImpl->display->screen_width();
}
int 	MatNDisplay::screen_height (){
    return _pImpl->display->screen_height();
}
unsigned int 	MatNDisplay::keycode (const char *const keycode){
    return _pImpl->display->keycode(keycode);
}
MatNDisplay & 	MatNDisplay::show (){
    _pImpl->display->show();
    return *this;
}
MatNDisplay & 	MatNDisplay::close (){
    _pImpl->display->close();
    return *this;
}
MatNDisplay & 	MatNDisplay::move (const int pos_x, const int pos_y){
    _pImpl->display->move(pos_x,pos_y);
    return *this;
}
MatNDisplay & 	MatNDisplay::resize (const bool force_redraw){
    _pImpl->display->resize(force_redraw);
    return *this;
}
MatNDisplay & 	MatNDisplay::resize (const int width, const int height, const bool force_redraw){
    _pImpl->display->resize(width,height,force_redraw);
    return *this;
}

MatNDisplay & 	MatNDisplay::resize (const MatNDisplay &disp, const bool force_redraw){
    _pImpl->display->resize(disp,force_redraw);
    return *this;
}
MatNDisplay & 	MatNDisplay::set_normalization (const unsigned int normalization){
    _pImpl->display->set_normalization(normalization);
    return *this;
}
MatNDisplay & 	MatNDisplay::set_title (const char *const format,...){
    _pImpl->display->set_title(format);
    return *this;
}
MatNDisplay & 	MatNDisplay::set_fullscreen (const bool is_fullscreen, const bool force_redraw){
    _pImpl->display->set_fullscreen(is_fullscreen,force_redraw);
    return *this;
}
MatNDisplay & 	MatNDisplay::toggle_fullscreen (const bool force_redraw){
    _pImpl->display->toggle_fullscreen(force_redraw);
    return *this;
}
MatNDisplay & 	MatNDisplay::show_mouse (){
    _pImpl->display->show_mouse();
    return *this;
}
MatNDisplay & 	MatNDisplay::hide_mouse (){
    _pImpl->display->hide_mouse();
    return *this;
}
MatNDisplay & 	MatNDisplay::set_mouse (const int pos_x, const int pos_y){
    _pImpl->display->set_mouse(pos_x,pos_y);
    return *this;
}
MatNDisplay & 	MatNDisplay::set_button (){
    _pImpl->display->set_button();
    return *this;
}
MatNDisplay & 	MatNDisplay::set_button (const unsigned int button, const bool is_pressed){
    _pImpl->display->set_button(button,  is_pressed);
    return *this;
}
MatNDisplay & 	MatNDisplay::set_wheel (){
    _pImpl->display->set_wheel();
    return *this;
}
MatNDisplay & 	MatNDisplay::set_wheel (const int amplitude){
    _pImpl->display->set_wheel(amplitude);
    return *this;
}
MatNDisplay & 	MatNDisplay::set_key (){
    _pImpl->display->set_key();
    return *this;
}
MatNDisplay & 	MatNDisplay::set_key (const unsigned int keycode, const bool is_pressed){
    _pImpl->display->set_key( keycode,  is_pressed);
    return *this;
}
MatNDisplay & 	MatNDisplay::flush (){
    _pImpl->display->flush();
    return *this;
}

MatNDisplay & 	MatNDisplay::waitTime (){
    _pImpl->display->wait();
    return *this;
}
MatNDisplay & 	MatNDisplay::waitTime (const unsigned int milliseconds){
    _pImpl->display->wait(milliseconds);
    return *this;
}

MatNDisplay & 	MatNDisplay::paint (){
    _pImpl->display->paint();
    return *this;
}

void 	MatNDisplay::waitTime (MatNDisplay &disp1){
    _pImpl->display->wait(*disp1._pImpl->display);
}
void 	MatNDisplay::waitTime (MatNDisplay &disp1, MatNDisplay &disp2){
    _pImpl->display->wait(*disp1._pImpl->display,*disp2._pImpl->display);
}
void 	MatNDisplay::waitTime (MatNDisplay &disp1, MatNDisplay &disp2, MatNDisplay &disp3){
    _pImpl->display->wait(*disp1._pImpl->display,*disp2._pImpl->display,*disp3._pImpl->display);
}
void 	MatNDisplay::waitTime (MatNDisplay &disp1, MatNDisplay &disp2, MatNDisplay &disp3, MatNDisplay &disp4){
    _pImpl->display->wait(*disp1._pImpl->display,*disp2._pImpl->display,*disp3._pImpl->display,*disp4._pImpl->display);
}
void 	MatNDisplay::waitTime (MatNDisplay &disp1, MatNDisplay &disp2, MatNDisplay &disp3, MatNDisplay &disp4, MatNDisplay &disp5){
    _pImpl->display->wait(*disp1._pImpl->display,*disp2._pImpl->display,*disp3._pImpl->display,*disp4._pImpl->display,*disp5._pImpl->display);
}
void 	MatNDisplay::wait_all (){
    _pImpl->display->wait_all();
}
#endif
}
