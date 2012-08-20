#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class square {
public:
	float center_x_;
	float center_y_;
	float size_;
	int direction_; //0 is up, 1 right, 2 down, 3 left;
	int flipped_; //0 is right-side-up, 1 is up-side-down
	int drawcount_;
	int deleted_; //0 if live, 1 if ready to die.
	
	void draw();
	
	square();
};

Color blue(0.0,0.45,.9);
Color green(0.0,.9,0.45);

void square::draw(){	
	if(flipped_ == 0){
		gl::color(blue);
	} else {
		gl::color(green);
	}
	float ulx = center_x_-(size_/2);
	float uly = center_y_-(size_/2);
	gl::drawSolidRect(Rectf(ulx,uly,ulx+size_,uly+size_));
	
	if(flipped_ == 1){
		gl::color(blue);
	} else {
		gl::color(green);
	}
	gl::drawSolidRect(Rectf(ulx+1,uly+1,ulx+size_-1,uly+size_-1));
	
	if(flipped_ == 0){
		gl::color(blue);
	} else {
		gl::color(green);
	}
	float ulcx;
	float ulcy;
	float lrcx;
	float lrcy;
	gl::drawSolidRect(Rectf(ulx,uly,ulx+(size_/8),uly+(size_/4)));
	
	drawcount_++;
	
	if(drawcount_ > 30){
		deleted_ = 1;
		return;
	}
}

square::square(){
	size_ = 2*getWindowHeight()/3;
	center_x_ = 2*getWindowHeight()/3;
	center_y_ = getWindowHeight()/3;
	direction_ = 0;
	flipped_ = 0;
	drawcount_ = 0;
	deleted_ = 0;
}

class FractalApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings( Settings *settings );
	void propogate();
	
	deque<square> square_list_;
	long framenum;
};

void FractalApp::prepareSettings( Settings *settings ){
    settings->setWindowSize( 256*4, 256*3 );
    settings->setFrameRate( 10.0f );
}

void FractalApp::propogate(){
	square old_square = square_list_[0];
	if(old_square.size_ <= 1) return;
	
	square_list_.pop_front();
	
	square s1,s2,s3,s4;
	//s1 is the top left square.
	//Stays exactly where it is, inherits properties from parent.
	s1.direction_ = old_square.direction_;
	s1.size_ = old_square.size_/2;
	s1.flipped_ = old_square.flipped_;
	switch(old_square.direction_){
		case 0:
			s1.center_x_ = old_square.center_x_ - (1-2*old_square.flipped_)*s1.size_/2;
			s1.center_y_ = old_square.center_y_ - (1-2*old_square.flipped_)*s1.size_/2;
			break;
		case 1:
			s1.center_x_ = old_square.center_x_ + (1-2*old_square.flipped_)*s1.size_/2;
			s1.center_y_ = old_square.center_y_ - (1-2*old_square.flipped_)*s1.size_/2;
			break;
		case 2:
			s1.center_x_ = old_square.center_x_ + (1-2*old_square.flipped_)*s1.size_/2;
			s1.center_y_ = old_square.center_y_ + (1-2*old_square.flipped_)*s1.size_/2;
			break;
		case 3:
		default:
			s1.center_x_ = old_square.center_x_ - (1-2*old_square.flipped_)*s1.size_/2;
			s1.center_y_ = old_square.center_y_ + (1-2*old_square.flipped_)*s1.size_/2;
			break;
	}
	
	//s2 is the bottom-left square.
	//Moves left one space, and rotates counter-clockwise.
	s2.direction_ = ((old_square.direction_)+3) % 4;
	s2.size_ = old_square.size_/2;
	s2.flipped_ = old_square.flipped_;
	switch(old_square.direction_){
		case 0:
			s2.center_x_ = old_square.center_x_ - (1-2*old_square.flipped_)*3*s2.size_/2;
			s2.center_y_ = old_square.center_y_ + (1-2*old_square.flipped_)*s2.size_/2;
			break;
		case 1:
			s2.center_x_ = old_square.center_x_ - (1-2*old_square.flipped_)*s2.size_/2;
			s2.center_y_ = old_square.center_y_ - (1-2*old_square.flipped_)*3*s2.size_/2;
			break;
		case 2:
			s2.center_x_ = old_square.center_x_ + (1-2*old_square.flipped_)*3*s2.size_/2;
			s2.center_y_ = old_square.center_y_ - (1-2*old_square.flipped_)*s2.size_/2;
			break;
		case 3:
		default:
			s2.center_x_ = old_square.center_x_ + (1-2*old_square.flipped_)*s2.size_/2;
			s2.center_y_ = old_square.center_y_ + (1-2*old_square.flipped_)*3*s2.size_/2;
	}
	
	//s3 is the bottom-right square.
	//rotates clockwise.
	s3.direction_ = ((old_square.direction_)+1) % 4;
	s3.size_ = old_square.size_/2;
	s3.flipped_ = old_square.flipped_;
	switch(old_square.direction_){
		case 0:
			s3.center_x_ = old_square.center_x_ + (1-2*old_square.flipped_)*s3.size_/2;
			s3.center_y_ = old_square.center_y_ + (1-2*old_square.flipped_)*s3.size_/2;
			break;
		case 1:
			s3.center_x_ = old_square.center_x_ - (1-2*old_square.flipped_)*s3.size_/2;
			s3.center_y_ = old_square.center_y_ + (1-2*old_square.flipped_)*s3.size_/2;
			break;
		case 2:
			s3.center_x_ = old_square.center_x_ - (1-2*old_square.flipped_)*s3.size_/2;
			s3.center_y_ = old_square.center_y_ - (1-2*old_square.flipped_)*s3.size_/2;
			break;
		case 3:
		default:
			s3.center_x_ = old_square.center_x_ + (1-2*old_square.flipped_)*s3.size_/2;
			s3.center_y_ = old_square.center_y_ - (1-2*old_square.flipped_)*s3.size_/2;
	}		
	
	//s4 is the top-right square.
	//flips over, ends up beneath the bottom-right square
	s4.direction_ = ((old_square.direction_)+2) % 2;
	s4.size_ = old_square.size_/2;
	s4.flipped_ = 1-old_square.flipped_;
	switch(old_square.direction_){
		case 0:
			s4.center_x_ = old_square.center_x_ + (1-2*old_square.flipped_)*s4.size_/2;
			s4.center_y_ = old_square.center_y_ + (1-2*old_square.flipped_)*3*s4.size_/2;
			break;
		case 1:
			s4.center_x_ = old_square.center_x_ - (1-2*old_square.flipped_)*3*s4.size_/2;
			s4.center_y_ = old_square.center_y_ + (1-2*old_square.flipped_)*s4.size_/2;
			break;
		case 2:
			s4.center_x_ = old_square.center_x_ - (1-2*old_square.flipped_)*s4.size_/2;
			s4.center_y_ = old_square.center_y_ - (1-2*old_square.flipped_)*3*s4.size_/2;
			break;
		case 3:
		default:
			s4.center_x_ = old_square.center_x_ + (1-2*old_square.flipped_)*3*s4.size_/2;
			s4.center_y_ = old_square.center_y_ - (1-2*old_square.flipped_)*s4.size_/2;
	}
	square_list_.push_back(s1);
	square_list_.push_back(s2);
	square_list_.push_back(s3);
	square_list_.push_back(s4);
}	

void FractalApp::setup()
{
	framenum = 0;
	square_list_.push_back(square());
}

void FractalApp::mouseDown( MouseEvent event )
{
}


void FractalApp::update()
{
	while(square_list_.size() > 0 && square_list_[0].deleted_ == 1){
		propogate();
	}
}

void FractalApp::draw()
{
	// clear out the window with dk gray
	gl::clear( Color( 0.125, 0.125, 0.125 ) );
	for(int i=0;i<square_list_.size();i++){
		square_list_[i].draw();
	}
}

CINDER_APP_BASIC( FractalApp, RendererGl )
