//makes that verilator doesnt scream because module and filename are not the same
/* verilator lint_off DECLFILENAME */

/* verilator lint_off UNUSED */

module submodule(
clk, inputs, out_seg
);

input clk;
input [15:0] inputs;
output [255:0] out_seg;

//both digits of score
reg [3:0] score_low;
reg [3:0] score_high;

reg [255:0] data_out;

//the display of falling balls
reg [4:0] ball_drop [0:4];

reg [3:0] nb_balls; //current number of balls in the game
reg [15:0] counter_ball_drop; //counter of cycles before moving a ball
reg [15:0] ball_cycle_count; //number of cycles before moving a ball

reg new_ball_delay; //wait a bit before a new ball appears

reg game_over; // game over status
reg [4:0] game_over_count; //number of time the screen will blink because of a game over

reg [5:0] player_position; //position of player on the field
reg [1:0] inputs_buf; //buffer for inputs, to detect rising edge of an input

assign out_seg[255:0] = data_out[255:0];

//defines how many clock count to have screen and gamestate update
parameter DEFAULT_CLOCK_COUNT = 400;
parameter MIN_CLOCK_COUNT = 100;
parameter GAMEOVER_CLOCK_COUNT = 200; //speed to display gameover

task bcd_to_7seg;
input [3:0] bcd_in;
output [6:0] seg_out;
begin
   assign seg_out =  (bcd_in==4'h0) ? 7'b0111111 :
                     (bcd_in==4'h1) ? 7'b0000110 :
                     (bcd_in==4'h2) ? 7'b1011011 :
                     (bcd_in==4'h3) ? 7'b1001111 :
                     (bcd_in==4'h4) ? 7'b1100110 :
                     (bcd_in==4'h5) ? 7'b1101101 :
                     (bcd_in==4'h6) ? 7'b1111101 :
                     (bcd_in==4'h7) ? 7'b0000111 :
                     (bcd_in==4'h8) ? 7'b1111111 :
                     (bcd_in==4'h9) ? 7'b1101111 :
                     (bcd_in==4'ha) ? 7'b1110111 :
                     (bcd_in==4'hb) ? 7'b1111100 :
                     (bcd_in==4'hc) ? 7'b0111001 :
                     (bcd_in==4'hd) ? 7'b1011110 :
                     (bcd_in==4'he) ? 7'b1111001 :
                     (bcd_in==4'hf) ? 7'b1110001 :
                     7'b0110110; //does a H, default shouldn't happen
end
endtask;

integer i;
integer j;
initial begin
   score_low = 0;
   score_high = 0;
   data_out = 0;
   ball_cycle_count = DEFAULT_CLOCK_COUNT;
   counter_ball_drop = DEFAULT_CLOCK_COUNT;
   nb_balls = 1;
   new_ball_delay = 0;
   game_over = 0;
   game_over_count = 0;

   for(i=0; i<=4; i=i+1)
      ball_drop[i] = 0;

   ball_drop[0][0] = 1; //initial ball
   player_position = 0;
end

always @*
begin
   bcd_to_7seg(score_low, data_out[36:30]);
   bcd_to_7seg(score_high, data_out[43:37]);

   data_out[29:25] = 0;
   data_out[25+player_position] = 1;
   data_out[4:0] = ball_drop[0];
   data_out[9:5] = ball_drop[1];
   data_out[14:10] = ball_drop[2];
   data_out[19:15] = ball_drop[3];
   data_out[24:20] = ball_drop[4];
end


always @(posedge clk)
begin

   inputs_buf <= inputs[2:1];

   //check inputs rising edge
   if(inputs[1] == 1 && inputs_buf[0] == 0 && player_position > 0)
   begin
      player_position <= player_position - 1;
   end

   if(inputs[2] == 1 && inputs_buf[1] == 0 && player_position < 4)
   begin
      player_position <= player_position + 1;
   end

   //counter to wait for a screen update
   counter_ball_drop <= counter_ball_drop - 1;

   if (counter_ball_drop == 0 && !game_over)
   begin
      counter_ball_drop <= ball_cycle_count; //reset the counter screen update
      for(j=0; j<=4; j=j+1)
      begin
         for(i=0; i<=3; i=i+1)
         begin
            if(ball_drop[j][i] == 1)
            begin
               //ball falling
               ball_drop[j][i] <= 0;
               ball_drop[j][i+1] <= ball_drop[j][i];
            end
         end

         //check that last balls really touch the player
         if(ball_drop[j][4] == 1)
         begin
            if(player_position == j[5:0])
            begin
               score_low <= score_low + 1;
               ball_drop[j][4] <= 0;
               ball_drop[(j+2)%5][0] <= 1;

               //difficulty, the screen will update faster
               if(ball_cycle_count > MIN_CLOCK_COUNT)
               begin
                  ball_cycle_count <= ball_cycle_count-10;
               end
            end else begin
               game_over <= 1;
               ball_cycle_count <= GAMEOVER_CLOCK_COUNT;
               counter_ball_drop <= 1; //force screen update
               game_over_count <= 0;
               nb_balls <= 1;
            end
         end
      end

      //add balls to the game
      if(score_high == 0 && score_low == 5 && nb_balls == 1) //when score is 05
      begin
         if(new_ball_delay == 1)
         begin
            ball_drop[3][0] <= 1;
            nb_balls <= 2;
            new_ball_delay <= 0;
         end else begin
            new_ball_delay <= 1;
         end
      end
      if(score_high == 2 && score_low == 0 && nb_balls == 2) //when score is 20
      begin
         ball_drop[3][0] <= 1;
         nb_balls <= 3;
      end
   end

   //game over "screen" blink all segements except score
   if (counter_ball_drop == 0 && game_over)
   begin
      counter_ball_drop <= ball_cycle_count;
      game_over_count <= game_over_count + 1;
      if(game_over_count % 2 == 0) //light all balls
      begin
         for(j=0; j<=4; j=j+1)
         begin
            for(i=0; i<=4; i=i+1)
            begin
               ball_drop[j][i] <= 1;
            end
         end
      end else begin //make all balls off
         for(j=0; j<=4; j=j+1)
         begin
            for(i=0; i<=4; i=i+1)
            begin
               ball_drop[j][i] <= 0;
            end
         end
         if(game_over_count >= 8) //8 blinks
         begin
            game_over_count <= 0;
            game_over <= 0;
            ball_drop[0][1] <= 1;
            score_low <= 0;
            score_high <= 0;
            ball_cycle_count <= DEFAULT_CLOCK_COUNT; //get back to initial speed
         end
      end
   end

   //increase score segments
   if(score_low == 10)
   begin
      score_low <= 0;
      score_high <= score_high + 1;
      if(score_high == 10)
      begin
         score_high <= 0;
      end
   end
end

endmodule
