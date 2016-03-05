//Copyright 1986-2015 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2015.1 (win64) Build 1215546 Mon Apr 27 19:22:08 MDT 2015
//Date        : Fri Mar 04 19:54:35 2016
//Host        : DESKTOP-V8TVDCU running 64-bit major release  (build 9200)
//Command     : generate_target turretmaster_video_bd_wrapper.bd
//Design      : turretmaster_video_bd_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module turretmaster_video_bd_wrapper
   (BCLK_O,
    JA,
    LRCLK_O,
    MCLK_O,
    SDATA_I,
    SDATA_O,
    SD_CD,
    SD_MISO,
    SD_MOSI,
    SD_SCK,
    SD_SS,
    SYSCLK,
    ddr3_sdram_addr,
    ddr3_sdram_ba,
    ddr3_sdram_cas_n,
    ddr3_sdram_ck_n,
    ddr3_sdram_ck_p,
    ddr3_sdram_cke,
    ddr3_sdram_dm,
    ddr3_sdram_dq,
    ddr3_sdram_dqs_n,
    ddr3_sdram_dqs_p,
    ddr3_sdram_odt,
    ddr3_sdram_ras_n,
    ddr3_sdram_reset_n,
    ddr3_sdram_we_n,
    dip_switches_8bits_tri_i,
    iic_rtl_scl_io,
    iic_rtl_sda_io,
    led_8bits_tri_o,
    push_buttons_5bits_tri_i,
    reset,
    usb_uart_rxd,
    usb_uart_txd);
  output BCLK_O;
  output [3:1]JA;
  output LRCLK_O;
  output MCLK_O;
  input SDATA_I;
  output SDATA_O;
  input [0:0]SD_CD;
  input SD_MISO;
  output SD_MOSI;
  output SD_SCK;
  output [0:0]SD_SS;
  input SYSCLK;
  output [14:0]ddr3_sdram_addr;
  output [2:0]ddr3_sdram_ba;
  output ddr3_sdram_cas_n;
  output [0:0]ddr3_sdram_ck_n;
  output [0:0]ddr3_sdram_ck_p;
  output [0:0]ddr3_sdram_cke;
  output [1:0]ddr3_sdram_dm;
  inout [15:0]ddr3_sdram_dq;
  inout [1:0]ddr3_sdram_dqs_n;
  inout [1:0]ddr3_sdram_dqs_p;
  output [0:0]ddr3_sdram_odt;
  output ddr3_sdram_ras_n;
  output ddr3_sdram_reset_n;
  output ddr3_sdram_we_n;
  input [7:0]dip_switches_8bits_tri_i;
  inout iic_rtl_scl_io;
  inout iic_rtl_sda_io;
  output [7:0]led_8bits_tri_o;
  input [4:0]push_buttons_5bits_tri_i;
  input reset;
  input usb_uart_rxd;
  output usb_uart_txd;

  wire BCLK_O;
  wire [3:1]JA;
  wire LRCLK_O;
  wire MCLK_O;
  wire SDATA_I;
  wire SDATA_O;
  wire [0:0]SD_CD;
  wire SD_MISO;
  wire SD_MOSI;
  wire SD_SCK;
  wire [0:0]SD_SS;
  wire SYSCLK;
  wire [14:0]ddr3_sdram_addr;
  wire [2:0]ddr3_sdram_ba;
  wire ddr3_sdram_cas_n;
  wire [0:0]ddr3_sdram_ck_n;
  wire [0:0]ddr3_sdram_ck_p;
  wire [0:0]ddr3_sdram_cke;
  wire [1:0]ddr3_sdram_dm;
  wire [15:0]ddr3_sdram_dq;
  wire [1:0]ddr3_sdram_dqs_n;
  wire [1:0]ddr3_sdram_dqs_p;
  wire [0:0]ddr3_sdram_odt;
  wire ddr3_sdram_ras_n;
  wire ddr3_sdram_reset_n;
  wire ddr3_sdram_we_n;
  wire [7:0]dip_switches_8bits_tri_i;
  wire iic_rtl_scl_i;
  wire iic_rtl_scl_io;
  wire iic_rtl_scl_o;
  wire iic_rtl_scl_t;
  wire iic_rtl_sda_i;
  wire iic_rtl_sda_io;
  wire iic_rtl_sda_o;
  wire iic_rtl_sda_t;
  wire [7:0]led_8bits_tri_o;
  wire [4:0]push_buttons_5bits_tri_i;
  wire reset;
  wire usb_uart_rxd;
  wire usb_uart_txd;

  IOBUF iic_rtl_scl_iobuf
       (.I(iic_rtl_scl_o),
        .IO(iic_rtl_scl_io),
        .O(iic_rtl_scl_i),
        .T(iic_rtl_scl_t));
  IOBUF iic_rtl_sda_iobuf
       (.I(iic_rtl_sda_o),
        .IO(iic_rtl_sda_io),
        .O(iic_rtl_sda_i),
        .T(iic_rtl_sda_t));
  turretmaster_video_bd turretmaster_video_bd_i
       (.BCLK_O(BCLK_O),
        .JA(JA),
        .LRCLK_O(LRCLK_O),
        .MCLK_O(MCLK_O),
        .SDATA_I(SDATA_I),
        .SDATA_O(SDATA_O),
        .SD_CD(SD_CD),
        .SD_MISO(SD_MISO),
        .SD_MOSI(SD_MOSI),
        .SD_SCK(SD_SCK),
        .SD_SS(SD_SS),
        .SYSCLK(SYSCLK),
        .ddr3_sdram_addr(ddr3_sdram_addr),
        .ddr3_sdram_ba(ddr3_sdram_ba),
        .ddr3_sdram_cas_n(ddr3_sdram_cas_n),
        .ddr3_sdram_ck_n(ddr3_sdram_ck_n),
        .ddr3_sdram_ck_p(ddr3_sdram_ck_p),
        .ddr3_sdram_cke(ddr3_sdram_cke),
        .ddr3_sdram_dm(ddr3_sdram_dm),
        .ddr3_sdram_dq(ddr3_sdram_dq),
        .ddr3_sdram_dqs_n(ddr3_sdram_dqs_n),
        .ddr3_sdram_dqs_p(ddr3_sdram_dqs_p),
        .ddr3_sdram_odt(ddr3_sdram_odt),
        .ddr3_sdram_ras_n(ddr3_sdram_ras_n),
        .ddr3_sdram_reset_n(ddr3_sdram_reset_n),
        .ddr3_sdram_we_n(ddr3_sdram_we_n),
        .dip_switches_8bits_tri_i(dip_switches_8bits_tri_i),
        .iic_rtl_scl_i(iic_rtl_scl_i),
        .iic_rtl_scl_o(iic_rtl_scl_o),
        .iic_rtl_scl_t(iic_rtl_scl_t),
        .iic_rtl_sda_i(iic_rtl_sda_i),
        .iic_rtl_sda_o(iic_rtl_sda_o),
        .iic_rtl_sda_t(iic_rtl_sda_t),
        .led_8bits_tri_o(led_8bits_tri_o),
        .push_buttons_5bits_tri_i(push_buttons_5bits_tri_i),
        .reset(reset),
        .usb_uart_rxd(usb_uart_rxd),
        .usb_uart_txd(usb_uart_txd));
endmodule
