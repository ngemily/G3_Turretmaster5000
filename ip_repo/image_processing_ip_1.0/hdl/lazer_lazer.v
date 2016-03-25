`timescale 1ns / 1ps

`include "global.vh"

module lazer_lazer (
    input wire clk,
    input wire reset_n,
    input wire en,
    input wire [7:0] red_threshold,
    input wire [15:0] pixel_col,
    input wire [15:0] pixel_row,
    input wire [`PIXEL_SIZE - 1:0] data,
    output wire [31:0] laser_xy,
    output wire [`PIXEL_SIZE - 1:0] debug
    );
    
    wire diff;
    wire red_valid_edge;
    reg [15:0] laser_x;
    reg [15:0] laser_y;
    reg [15:0] count;
    reg red_valid;
    reg red_valid_flopped;
    reg [2:0] state;  

    r_minus_gb_threshold P0 (
        .pixel(data),
        .threshold(red_threshold),
        .q(diff),
        .debug(debug)
    );

    assign laser_xy = {laser_x, laser_y};
    assign red_valid_edge = red_valid && ~red_valid_flopped;

    always@(posedge clk)    
    begin
        if (!reset_n)
            red_valid_flopped <= 0;
        else
            red_valid_flopped <= red_valid;
    end
    
    always@(posedge clk)    
    begin
        if (!reset_n)
        begin
            laser_x <= 0;
            laser_y <= 0;
        end        
        else if (red_valid_edge)
        begin
            laser_x <= pixel_col - (count >> 1);
            laser_y <= pixel_row;
        end
        else
        begin
            laser_x <= laser_x;
            laser_y <= laser_y;
        end
    end
    
	localparam [1:0]    LESSTHANFIVE    = 2'd0,
                        FIVEORMORE      = 2'd1,
                        DONE            = 2'd2;
                           
	always @(posedge clk)
    begin
        if (!reset_n)
        begin
            state <= LESSTHANFIVE;
            count <= 0;
            red_valid <= 0;
        end
        else if(en)
        begin
            case (state)
                LESSTHANFIVE:
                    if (diff)
                    begin
                        state <= (count > 4) ? FIVEORMORE : LESSTHANFIVE;
                        count <= count + 1;
                        red_valid <= 0;
                    end
                    else
                    begin
                        state <= LESSTHANFIVE;
                        count <= 0;
                        red_valid <= 0;                        
                    end
                FIVEORMORE:
                    if (diff)
                    begin
                        state <= FIVEORMORE;
                        count <= count + 1;
                        red_valid <= 0;
                    end
                    else
                    begin
                        state <= DONE;
                        count <= count;
                        red_valid <= 1;
                    end
                DONE:
                    begin
                        state <= DONE;
                        count <= count;
                        red_valid <= 1;                        
                    end
            endcase
        end
    end
endmodule

// Take the difference of the red, blue, and green components.
module r_minus_gb_threshold (
    input [`PIXEL_SIZE - 1:0] pixel,
    input [7:0] threshold,
    output q,
    output [`PIXEL_SIZE - 1:0] debug
    );
    wire [15:0] G = pixel[7:0];
    wire [15:0] B = pixel[15:8];
    wire [15:0] R = pixel[23:16];
    wire [15:0] d;
    
    wire [15:0] GB_SUM = G + B;
    
    assign d = (R > GB_SUM) ? (R - GB_SUM) : 0;
    
    assign q = (d > threshold) ? 1 : 0;

    assign debug = {d[7:0], d[15:8], 8'b0};
endmodule
    
    
