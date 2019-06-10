#include <cstdio>
#include <thread>

#include <unistd.h>

#include <qapplication.h>
#include <qpushbutton.h>
#include <qimage.h>
#include <qlabel.h>
#include <qfile.h>
#include <qdebug.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

#include "segments_generator.hpp"
#include "segment.hpp"
#include "displayer.hpp"
#include "Vtop.h"

bool read_json(const char *filename, Segments_generator *sg, std::vector<Segment> *lst_seg, std::vector<char> *lst_inputs);

void display_loop(Displayer *displayer, std::vector<Segment> *lst_seg);

//feed clock and inputs to the verilator class and read the segments
void simulation_loop(Displayer *displayer, std::vector<Segment> *lst_seg, Vtop *vtop)
{
   while(!displayer->is_closed())
   {
      vtop->clk = 0;
      vtop->inputs = 0;
      vtop->eval();
      vtop->clk = 1;
      vtop->inputs = displayer->get_inputs();
      vtop->eval();
      uint *out_seg = vtop->out_seg;
      uint segments = out_seg[0];

      //update the segments on screen
      for (size_t i = 0; i < lst_seg->size(); i++)
      {
         if(lst_seg->at(i).is_init) //only segments initialized ==> in the json
         {
            lst_seg->at(i).activate(segments&(1<<i));
         }
      }
      displayer->draw();
      usleep(1000); //~1KHz
   }
}

int main( int argc, char **argv )
{
   std::string folder_name; //folder containing the png

   const int max_nb_segments = 256;
   const int max_nb_inputs = 16;

   if(argc != 2){
      printf("usage: %s name_of_folder\n", argv[0]);
      return -1;
   }
   else{
      folder_name = argv[1];
   }

   QApplication app( argc, argv );

   std::string image_path = folder_name+"/segments.png";
   QImage image(image_path.c_str());
   Displayer displayer;
   displayer.init(image.width(), image.height());

   image = image.convertToFormat(QImage::Format_RGB32);

   uchar *array = image.bits();

   Segments_generator sg;
   sg.set_image(array, image.width(), image.height());

   std::vector<Segment> lst_seg(max_nb_segments);
   std::vector<char> lst_inputs(max_nb_inputs);

   std::string json_path = folder_name+"/segments.json";
   read_json(json_path.c_str(), &sg, &lst_seg, &lst_inputs);

   displayer.set_input_map(lst_inputs);

   for (size_t i = 0; i < max_nb_segments; i++)
   {
      if(lst_seg[i].is_init)
      {
         displayer.add_segment(&lst_seg[i]);
      }
   }

   //simulated verilog module
   //output of verilator
   Vtop vtop;

   //main simulation loop
   std::thread thr (simulation_loop, &displayer, &lst_seg, &vtop);

   app.exec();

   thr.join();

   return 0;
}


bool read_json(const char *filename, Segments_generator *sg, std::vector<Segment> *lst_seg, std::vector<char> *lst_inputs)
{
   QString file_str;
   QFile file;
   file.setFileName(filename);
   file.open(QIODevice::ReadOnly | QIODevice::Text);
   file_str = file.readAll();
   file.close();

   QJsonDocument doc = QJsonDocument::fromJson(file_str.toUtf8());
   QJsonObject obj = doc.object();
   QJsonArray array = obj["segments"].toArray();

   for (size_t i = 0; i < (uint)array.size(); i++)
   {
      QJsonObject obj = array[i].toObject();
      int posx = obj["posx"].toInt();
      int posy = obj["posy"].toInt();
      int output = obj["output"].toInt();

      if(output >= (int)lst_seg->size()){
         printf("output %d over %lu, dismiss\n", output, lst_seg->size());
      }

      bool is_segment_get = sg->get_segment(&lst_seg->at(output), posx, posy);
      if(is_segment_get)
      {
         lst_seg->at(output).is_init = true;
      }
      else
      {
         printf("Couldn't add segment with output %d, at x:%d, y:%d\n", output, posx, posy);
      }
   }

   array = obj["inputs"].toArray();

   for (size_t i = 0; i < (uint) array.size(); i++) {
      QJsonObject obj = array[i].toObject();
      std::string key = obj["key"].toString().toStdString();
      int input = obj["input"].toInt();

      if(key.size() > 1){
         printf("key: %s is too long (should be only one character), dismiss it\n", key.c_str());
      }

      if(input > (int)lst_inputs->size()){
         printf("input of key %s is over %lu\n", key.c_str(), lst_inputs->size());
      }

      lst_inputs->at(input) = key[0];
   }


   return true;
}

void display_loop()
{

}
