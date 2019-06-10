module top(
clk, inputs, out_seg
);

input clk;
input [15:0] inputs;
output [255:0] out_seg;

// wire [4:0] read_reg_1;
// wire [4:0] read_reg_2;
// wire [4:0] write_reg;
// wire [31:0] reg_write_data;

// register_file register_file_inst(
// .reset(reset), .read_reg_1(read_reg_1), .read_reg_2(read_reg_2), .write_reg(write_reg), .write_data(reg_write_data), .write_en(write_en), .data_out_1(data_out_1), .data_out_2(data_out_2)
// );

submodule submodule_inst(
.clk(clk), .inputs(inputs), .out_seg(out_seg)
);

endmodule
