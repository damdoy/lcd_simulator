#ifndef SEGMENT_HPP
#define SEGMENT_HPP

#include <qimage.h>

class Segment{
public:

   Segment();
   ~Segment();
   void init(uint posx, uint posy, uint width, uint height, std::vector<std::vector<bool> > pixels);
   QImage *get_qimage();

   void activate(bool act);

   uint posx;
   uint posy;
   uint width;
   uint height;

   bool is_init;

protected:
   std::vector<std::vector<bool> > pixels;
   QImage *qimage;
};

Segment::Segment()
{
   qimage = NULL;
   is_init = false;
}

Segment::~Segment()
{
   if(qimage != NULL)
   {
      delete qimage;
      qimage = NULL;
   }
}

void Segment::init(uint posx, uint posy, uint width, uint height, std::vector<std::vector<bool> > pixels)
{
   this->posx = posx;
   this->posy = posy;
   this->width = width;
   this->height = height;
   this->pixels = pixels;

   qimage = new QImage(width, height, QImage::Format_RGB32);

   for (size_t y = 0; y < height; y++)
   {
      for (size_t x = 0; x < width; x++)
      {
         if(pixels[x][y] == 1)
         {
            qimage->setPixel(x, y, 0xff000000);
         }
         else
         {
            qimage->setPixel(x, y, 0xffffffff);
         }
      }
   }
}

QImage* Segment::get_qimage()
{
   return qimage;
}

void Segment::activate(bool act)
{
   for (size_t x = 0; x < width; x++)
   {
      for (size_t y = 0; y < height; y++)
      {
         if(pixels[x][y] == 1)
         {
            if(act == true)
            {
               qimage->setPixel(x, y, 0xff000000); //black
            }
            else
            {
               qimage->setPixel(x, y, 0xfff0f0f0); //light gray
            }
         }
         else
         {
            qimage->setPixel(x, y, 0xffffffff); //white
         }
      }
   }
}

#endif
