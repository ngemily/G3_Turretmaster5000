//Copyright 1986-2015 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2015.1 (win64) Build 1215546 Mon Apr 27 19:22:08 MDT 2015
//Date        : Wed Feb 24 13:45:23 2016
//Host        : DESKTOP-V8TVDCU running 64-bit major release  (build 9200)
//Command     : generate_target nexys4_bd_wrapper.bd
//Design      : nexys4_bd_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module nexys4_bd_wrapper
   (SD_MISO,
    SD_MOSI,
    SD_SCK,
    SD_SS,
    reset,
    reset_1,
    sd_inserted,
    sys_clock,
    usb_uart_rxd,
    usb_uart_txd);
  input SD_MISO;
  output SD_MOSI;
  output SD_SCK;
  output [0:0]SD_SS;
  input reset;
  input reset_1;
  input sd_inserted;
  input sys_clock;
  input usb_uart_rxd;
  output usb_uart_txd;

  wire SD_MISO;
  wire SD_MOSI;
  wire SD_SCK;
  wire [0:0]SD_SS;
  wire reset;
  wire reset_1;
  wire [15:0]sd_cd_tri_i;
  wire sys_clock;
  wire usb_uart_rxd;
  wire usb_uart_txd;
  
  assign sd_cd_tri_i[15:1] = 15'b0;
  assign sd_cd_tri_i[0] = sd_inserted;

  nexys4_bd nexys4_bd_i
       (.SD_CD_tri_i(sd_cd_tri_i),
        .SD_MISO(SD_MISO),
        .SD_MOSI(SD_MOSI),
        .SD_SCK(SD_SCK),
        .SD_SS(SD_SS),
        .reset(reset),
        .reset_1(reset_1),
        .sys_clock(sys_clock),
        .usb_uart_rxd(usb_uart_rxd),
        .usb_uart_txd(usb_uart_txd));
endmodule
