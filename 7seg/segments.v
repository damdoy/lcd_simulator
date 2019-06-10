//makes that verilator doesnt scream because module and filename are not the same
/* verilator lint_off DECLFILENAME */

/* verilator lint_off UNUSED */

module submodule(
clk, inputs, out_seg
);

input clk;
input [15:0] inputs;
output [255:0] out_seg;

reg [3:0] counter_sec_0;
reg [3:0] counter_sec_1;
reg [3:0] counter_min_0;
reg [3:0] counter_min_1;
reg [31:0] data_out;

assign out_seg[31:0] = data_out[31:0];
assign out_seg[255:32] = 0;


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

initial begin
   counter_sec_0 = 0;
   counter_sec_1 = 0;
   counter_min_0 = 0;
   counter_min_1 = 0;
end

always @*
begin
   bcd_to_7seg(counter_sec_0, data_out[6:0]);
   bcd_to_7seg(counter_sec_1, data_out[13:7]);
   bcd_to_7seg(counter_min_0, data_out[20:14]);
   bcd_to_7seg(counter_min_1, data_out[27:21]);

   //light up the two dots
   if(inputs[0] == 1)
   begin
      data_out[29:28] = 2'b11;
   end else begin
      data_out[29:28] = 2'b00;
   end

   data_out[31:30] = 0;
end

always @(posedge clk)
begin

   //reset button
   if(inputs[1] == 1)
   begin
      counter_sec_0 <= 0;
      counter_sec_1 <= 0;
      counter_min_0 <= 0;
      counter_min_1 <= 0;
   end

   //counting time
   counter_sec_0 <= counter_sec_0 + 1;
   if(counter_sec_0 == 9)
   begin
      counter_sec_0 <= 0;
      counter_sec_1 <= counter_sec_1 + 1;
      if(counter_sec_1 == 9)
      begin
         counter_sec_1 <= 0;
         counter_min_0 <= counter_min_0 + 1;
         if(counter_min_0 == 9)
         begin
            counter_min_0 <= 0;
            counter_min_1 <= counter_min_1 + 1;
            if(counter_min_1 == 9)
            begin
               counter_min_1 <= 0;
            end
         end
      end
   end
end

endmodule
