#ifndef SEGMENTS_GENERATOR_HPP
#define SEGMENTS_GENERATOR_HPP

#include <queue>
#include "segment.hpp"

class Segments_generator{
public:
   Segments_generator();
   bool get_segment(Segment *segment, uint pointx, uint pointy);
   void set_image(uchar *img_pixel_rgb32, uint width, uint height); //needs to be done before get_segment

   enum Pixel_discovery{NOT_VISITED, IN_QUEUE, ADDED, DISCARDED};

   struct Point
   {
      Point(uint x, uint y) : x(x), y(y){}
      uint x;
      uint y;
   };

protected:
   std::vector<std::vector<bool> > image;
   uint width;
   uint height;

   //returns a sub image the size of the segment
   std::vector<std::vector<bool> > get_subimage(std::vector<std::vector<Pixel_discovery> > pixels_exploration, uint *minx, uint *maxx, uint *miny, uint *maxy);
};

Segments_generator::Segments_generator()
{
}

bool Segments_generator::get_segment(Segment *segment, uint pointx, uint pointy)
{
   if(image[pointx][pointy] == 0) //selected pixel not even in a segment
   {
      return false;
   }
   std::vector<std::vector<Pixel_discovery> > pixels_exploration;
   std::queue<Point> queue_exploration;
   queue_exploration.push(Point(pointx, pointy));
   //init and copy
   pixels_exploration.resize(width);
   for (size_t x = 0; x < width; x++)
   {
      pixels_exploration[x].resize(height);
      for (size_t y = 0; y < height; y++)
      {
         if(image[x][y] == 1)
         {
            pixels_exploration[x][y] = NOT_VISITED;
         }
         else
         {
            pixels_exploration[x][y] = DISCARDED;
         }
      }
   }

   while(!queue_exploration.empty())
   {
      Point cur_point = queue_exploration.front();
      queue_exploration.pop();

      if(image[cur_point.x][cur_point.y] == 0)
      {
         pixels_exploration[cur_point.x][cur_point.y] = DISCARDED;
      }
      else //if(image[cur_point.x][cur_point.y] == 1)
      {
         pixels_exploration[cur_point.x][cur_point.y] = ADDED;

         //check adjascent pixels, add them to list if not visited
         if(cur_point.x > 0 && pixels_exploration[cur_point.x-1][cur_point.y] == NOT_VISITED)
         {
            pixels_exploration[cur_point.x-1][cur_point.y] = IN_QUEUE;
            queue_exploration.push(Point(cur_point.x-1, cur_point.y));
         }
         if(cur_point.x < width-1 && pixels_exploration[cur_point.x+1][cur_point.y] == NOT_VISITED)
         {
            pixels_exploration[cur_point.x+1][cur_point.y] = IN_QUEUE;
            queue_exploration.push(Point(cur_point.x+1, cur_point.y));
         }
         if(cur_point.y > 0 && pixels_exploration[cur_point.x][cur_point.y-1] == NOT_VISITED)
         {
            pixels_exploration[cur_point.x][cur_point.y-1] = IN_QUEUE;
            queue_exploration.push(Point(cur_point.x, cur_point.y-1));
         }
         if(cur_point.y < height-1 && pixels_exploration[cur_point.x][cur_point.y+1] == NOT_VISITED)
         {
            pixels_exploration[cur_point.x][cur_point.y+1] = IN_QUEUE;
            queue_exploration.push(Point(cur_point.x, cur_point.y+1));
         }
      }
   }

   uint subseg_minx = 0;
   uint subseg_maxx = 0;
   uint subseg_miny = 0;
   uint subseg_maxy = 0;

   std::vector<std::vector<bool> > sub_image = get_subimage(pixels_exploration, &subseg_minx, &subseg_maxx, &subseg_miny, &subseg_maxy);

   segment->init(subseg_minx, subseg_miny, subseg_maxx-subseg_minx+1, subseg_maxy-subseg_miny+1, sub_image);

   return image[pointx][pointy];
}

//image is per scanline
void Segments_generator::set_image(uchar *img_pixel_rgb32, uint width, uint height)
{
   this->width = width;
   this->height = height;
   image.resize(width);
   for (size_t x = 0; x < width; x++)
   {
      image[x].resize(height);
      for (size_t y = 0; y < height; y++)
      {
         //per scanline image, transform it
         int index = (y*width+x)*4;
         if(img_pixel_rgb32[index] == 0)
         {
            image[x][y] = 1;
         }
         else
         {
            image[x][y] = 0;
         }
      }
   }
}

std::vector<std::vector<bool> > Segments_generator::get_subimage(std::vector<std::vector<Pixel_discovery> > pixels_exploration, uint *out_minx, uint *out_maxx, uint *out_miny, uint *out_maxy)
{
   std::vector<std::vector<bool> > ret;

   //default values are extremes
   uint minx = width;
   uint maxx = 0;
   uint miny = height;
   uint maxy = 0;

   //find dimensions of segment
   for (uint x = 0; x < width; x++)
   {
      for (uint y = 0; y < height; y++)
      {
         if(pixels_exploration[x][y] == ADDED)
         {
            if(minx > x) minx = x;
            if(maxx < x) maxx = x;
            if(miny > y) miny = y;
            if(maxy < y) maxy = y;
         }
      }
   }

   uint subimage_width = maxx-minx+1;
   uint subimage_height = maxy-miny+1;

   ret.resize(subimage_width);
   for(uint x = 0; x < subimage_width; x++)
   {
      ret[x].resize(subimage_height);
      for(uint y = 0; y < subimage_height; y++)
      {
         if(pixels_exploration[minx+x][miny+y] == ADDED)
         {
            ret[x][y] = 1;
         }
         else
         {
            ret[x][y] = 0;
         }
      }
   }

   *out_minx = minx;
   *out_maxx = maxx;
   *out_miny = miny;
   *out_maxy = maxy;

   return ret;
}

#endif
