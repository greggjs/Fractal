#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class square {
public:
	float center_x_;
	float center_y_;
	float start_x_;
	float start_y_;
	float end_x_;
	float end_y_;
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
	int timer = drawcount_;
	if(timer > 15) timer = 15;
	center_x_ = (timer*end_x_ + (15-timer)*start_x_)/15;
	center_y_ = (timer*end_y_ + (15-timer)*start_y_)/15;
	
	if(flipped_ == 0){
		gl::color(blue);
	} else {
		gl::color(green);
	}
	float ulx = center_x_-(size_/2);
	float uly = center_y_-(size_/2);
	gl::drawSolidRect(Rectf(ulx,uly,ulx+size_,uly+size_));
	
	if(size_ <= 1.0){
		drawcount_++;
		if(drawcount_ > 45){
			deleted_ = 1;
		}
		return;
	}
	
	if(flipped_ == 1){
		gl::color(blue);
	} else {
		gl::color(green);
	}
	gl::drawSolidRect(Rectf(ulx+(size_/32),uly+(size_/32),ulx+size_-(size_/32),uly+size_-(size_/32)));
	
	if(flipped_ == 0){
		gl::color(blue);
	} else {
		gl::color(green);
	}
	
	if(size_ <= 2.0){
		drawcount_++;
		if(drawcount_ > 45){
			deleted_ = 1;
		}
		return;
	}

	float ulcx;
	float ulcy;
	float lrcx;
	float lrcy;
	//TODO visualize the flipping
	switch(direction_){
		case 0:
		default:
			ulcx = ulx;
			ulcy = uly;
			lrcx = ulx+(size_/8);
			lrcy = uly+(size_/4);
			break;
		case 1:
			ulcx = ulx+size_;
			ulcy = uly;
			lrcx = ulx+size_-(size_/4);
			lrcy = uly+(size_/8);
			break;
		case 2:
			ulcx = ulx+size_;
			ulcy = uly+size_;
			lrcx = ulx+size_-(size_/8);
			lrcy = uly+size_-(size_/4);
			break;
		case 3:
			ulcx = ulx;
			ulcy = uly+size_;
			lrcx = ulx+(size_/4);
			lrcy = uly+size_-(size_/8);
			break;
	}
	gl::drawSolidRect(Rectf(ulcx,ulcy,lrcx,lrcy));
	
	
	
	//Vertical divider
	timer = drawcount_ - 15;
	if(timer < 0) timer = 0;
	if(timer > 15) timer = 15;
	switch(direction_){
		case 0:
		default:
			ulcx = ulx+size_/2-size_/32;
			ulcy = uly;
			lrcx = ulx+size_/2+size_/32;
			lrcy = uly+(timer*size_)/15;
			break;
		case 1:
			ulcy = uly+size_/2-size_/32;
			ulcx = ulx+size_;
			lrcy = uly+size_/2+size_/32;
			lrcx = ulx+size_-(timer*size_)/15;
			break;
		case 2:
			ulcx = ulx+size_/2-size_/32;
			ulcy = uly+size_-(timer*size_)/15;
			lrcx = ulx+size_/2+size_/32;
			lrcy = uly+size_;
			break;
		case 3:
			ulcy = uly+size_/2-size_/32;
			ulcx = ulx;
			lrcy = uly+size_/2+size_/32;
			lrcx = ulx+(timer*size_)/15;
			break;
	}
	gl::drawSolidRect(Rectf(ulcx,ulcy,lrcx,lrcy));
	
	//Horizontal divider
	timer = drawcount_ - 30;
	if(timer < 0) timer = 0;
	if(timer > 15) timer = 15;
	
	switch(direction_){
		case 0:
		default:
			ulcy = uly+size_/2-size_/32;
			ulcx = ulx;
			lrcy = uly+size_/2+size_/32;
			lrcx = ulx+(timer*size_)/15;
			break;
		case 1:
			ulcx = ulx+size_/2-size_/32;
			ulcy = uly+size_;
			lrcx = ulx+size_/2+size_/32;
			lrcy = uly+size_-(timer*size_)/15;
			break;
		case 2:
			ulcy = uly+size_/2-size_/32;
			ulcx = ulx+size_-(timer*size_)/15;
			lrcy = uly+size_/2+size_/32;
			lrcx = ulx+size_;
			break;
		case 3:
			ulcx = ulx+size_/2-size_/32;
			ulcy = uly;
			lrcx = ulx+size_/2+size_/32;
			lrcy = uly+(timer*size_)/15;
			break;
	}
	gl::drawSolidRect(Rectf(ulcx,ulcy,lrcx,lrcy));
	
	drawcount_++;
	
	if(drawcount_ > 45){
		deleted_ = 1;
		return;
	}
}

square::square(){
	size_ = 2*256;
	center_x_ = 2*256;
	center_y_ = 2*128;
	start_x_ = 2*256;
	start_y_= 2*128;
	end_x_	= 2*256;
	end_y_	= 2*128;
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
    settings->setWindowSize( 800, 800 );
    settings->setFrameRate( 60.0f );
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
			s1.end_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s1.size_/2;
			s1.end_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s1.size_/2;
			break;
		case 1:
			s1.end_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s1.size_/2;
			s1.end_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s1.size_/2;
			break;
		case 2:
			s1.end_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s1.size_/2;
			s1.end_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s1.size_/2;
			break;
		case 3:
		default:
			s1.end_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s1.size_/2;
			s1.end_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s1.size_/2;
			break;
	}
	s1.start_x_ = s1.end_x_;
	s1.start_y_ = s1.end_y_;
	
	//s2 is the bottom-left square.
	//Moves left one space, and rotates counter-clockwise.
	s2.direction_ = ((old_square.direction_)+3) % 4;
	s2.size_ = old_square.size_/2;
	s2.flipped_ = old_square.flipped_;
	switch(old_square.direction_){
		case 0:
			s2.end_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*3*s2.size_/2;
			s2.end_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s2.size_/2;
			s2.start_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s2.size_/2;
			s2.start_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s2.size_/2;
			break;
		case 1:
			s2.end_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s2.size_/2;
			s2.end_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*3*s2.size_/2;
			s2.start_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s2.size_/2;
			s2.start_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s2.size_/2;
			break;
		case 2:
			s2.end_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*3*s2.size_/2;
			s2.end_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s2.size_/2;
			s2.start_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s2.size_/2;
			s2.start_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s2.size_/2;
			break;
		case 3:
		default:
			s2.end_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s2.size_/2;
			s2.end_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*3*s2.size_/2;
			s2.start_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s2.size_/2;
			s2.start_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s2.size_/2;
	}
	
	//s3 is the bottom-right square.
	//rotates clockwise.
	s3.direction_ = ((old_square.direction_)+1) % 4;
	s3.size_ = old_square.size_/2;
	s3.flipped_ = old_square.flipped_;
	switch(old_square.direction_){
		case 0:
			s3.end_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s3.size_/2;
			s3.end_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s3.size_/2;
			break;
		case 1:
			s3.end_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s3.size_/2;
			s3.end_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s3.size_/2;
			break;
		case 2:
			s3.end_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s3.size_/2;
			s3.end_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s3.size_/2;
			break;
		case 3:
		default:
			s3.end_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s3.size_/2;
			s3.end_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s3.size_/2;
	}	
	s3.start_x_ = s3.end_x_;
	s3.start_y_ = s3.end_y_;
	
	//s4 is the top-right square.
	//flips over, ends up beneath the bottom-right square
	s4.direction_ = ((old_square.direction_)+2) % 4;
	s4.size_ = old_square.size_/2;
	s4.flipped_ = 1-old_square.flipped_;
	switch(old_square.direction_){
		case 0:
			s4.end_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s4.size_/2;
			s4.end_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*3*s4.size_/2;
			s4.start_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s4.size_/2;
			s4.start_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s4.size_/2;
			break;
		case 1:
			s4.end_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*3*s4.size_/2;
			s4.end_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s4.size_/2;
			s4.start_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*s4.size_/2;
			s4.start_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s4.size_/2;
			break;
		case 2:
			s4.end_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s4.size_/2;
			s4.end_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*3*s4.size_/2;
			s4.start_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s4.size_/2;
			s4.start_y_ = old_square.end_y_ + (1-2*old_square.flipped_)*s4.size_/2;
			break;
		case 3:
		default:
			s4.end_x_ = old_square.end_x_ + (1-2*old_square.flipped_)*3*s4.size_/2;
			s4.end_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s4.size_/2;
			s4.start_x_ = old_square.end_x_ - (1-2*old_square.flipped_)*s4.size_/2;
			s4.start_y_ = old_square.end_y_ - (1-2*old_square.flipped_)*s4.size_/2;
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
		if(square_list_[0].end_x_ + square_list_[0].size_/2 < 0 || square_list_[0].end_x_ - square_list_[0].size_/2 >= getWindowWidth() ||
		   square_list_[0].end_y_ + square_list_[0].size_/2 < 0 || square_list_[0].end_y_ - square_list_[0].size_/2>= getWindowHeight()){
			square_list_.pop_front();
		} else {
			propogate();
		}
	}
}

void FractalApp::draw()
{
	// clear out the window with dk gray
	gl::clear( Color( 0.125, 0.125, 0.125 ) );
	int sqls = square_list_.size();
	for(int i=0;i<sqls;i++){
		if(square_list_[i].deleted_ != 1){
			square_list_[i].draw();
		}
	}
}

CINDER_APP_BASIC( FractalApp, RendererGl )
