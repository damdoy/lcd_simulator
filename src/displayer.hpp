#ifndef DISPLAYER_HPP
#define DISPLAYER_HPP

#include <vector>

#include <qpainter.h>
#include <qevent.h>

#include "segment.hpp"

//displayer handles display and inputs
class Displayer : public QLabel
{
public:
   void init(uint width, uint height);
   void add_segment(Segment *seg);
   void draw();
   void set_input_map(std::vector<char> inputs);
   uint get_inputs(); //returns bits for the input for the current time
   bool is_closed();

protected:
   uint width;
   uint height;
   std::vector<Segment*> list_segments;
   std::vector<char> lst_inputs; //map of inputs
   uint input_bits;
   bool closed; //is this window closed?

   void keyPressEvent(QKeyEvent *evt);
   void keyReleaseEvent(QKeyEvent *evt);
   void closeEvent(QCloseEvent *evt);
};

void Displayer::init(uint width, uint height)
{
   this->closed = false;
   this->input_bits = 0;
   this->width = width;
   this->height = height;

   //disable resize
   setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
   this->setFixedSize(width, height);
}

void Displayer::add_segment(Segment *seg)
{
   this->list_segments.push_back(seg);
}

void Displayer::draw()
{

   QImage background_image(this->width, this->height, QImage::Format_RGB32);
   background_image.fill(0xffffffff);

   for (size_t i = 0; i < list_segments.size(); i++)
   {
      Segment *seg = list_segments[i];
      QPainter painter(&background_image);
      painter.drawImage(seg->posx, seg->posy, *seg->get_qimage());
      painter.end();
   }

   this->setPixmap(QPixmap::fromImage(background_image));
   this->show();
}

void Displayer::set_input_map(std::vector<char> inputs){
   this->lst_inputs = inputs;
}

uint Displayer::get_inputs()
{
   return input_bits;
}

bool Displayer::is_closed()
{
   return closed;
}

void Displayer::keyPressEvent(QKeyEvent *evt)
{
   for (size_t i = Qt::Key_A; i < Qt::Key_Z; i++) {
      if(evt->key() == (int)i)
      {
         int current_press_key = i-Qt::Key_A+'a'; //convert from qt key to ascii
         for (size_t j = 0; j < lst_inputs.size(); j++) {
            if(lst_inputs.at(j) == current_press_key){
               input_bits |= 1<<j; //add key to bitmask
            }
         }
      }
   }
}

void Displayer::keyReleaseEvent(QKeyEvent *evt)
{
   for (size_t i = Qt::Key_A; i < Qt::Key_Z; i++) {
      if(evt->key() == (int)i)
      {
         int current_press_key = i-Qt::Key_A+'a'; //convert from qt key to ascii
         for (size_t j = 0; j < lst_inputs.size(); j++) {
            if(lst_inputs.at(j) == current_press_key){
               input_bits &= ~(1<<j); //remove key from bitmask
            }
         }
      }
   }
}

void Displayer::closeEvent(QCloseEvent *evt)
{
   closed = true;
   evt->accept();
}

#endif
