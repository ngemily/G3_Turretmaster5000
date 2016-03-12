//Copyright 1986-2015 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2015.1 (win64) Build 1215546 Mon Apr 27 19:22:08 MDT 2015
//Date        : Tue Mar 08 18:13:16 2016
//Host        : Robert-PC running 64-bit Service Pack 1  (build 7601)
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
    TMDS_IN_clk_n,
    TMDS_IN_clk_p,
    TMDS_IN_data_n,
    TMDS_IN_data_p,
    TMDS_OUT_clk_n,
    TMDS_OUT_clk_p,
    TMDS_OUT_data_n,
    TMDS_OUT_data_p,
    ddc_scl_io,
    ddc_sda_io,
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
    hdmi_hpd,
    hdmi_rx_txen,
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
  input TMDS_IN_clk_n;
  input TMDS_IN_clk_p;
  input [2:0]TMDS_IN_data_n;
  input [2:0]TMDS_IN_data_p;
  output TMDS_OUT_clk_n;
  output TMDS_OUT_clk_p;
  output [2:0]TMDS_OUT_data_n;
  output [2:0]TMDS_OUT_data_p;
  inout ddc_scl_io;
  inout ddc_sda_io;
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
  output [0:0]hdmi_hpd;
  output [0:0]hdmi_rx_txen;
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
  wire TMDS_IN_clk_n;
  wire TMDS_IN_clk_p;
  wire [2:0]TMDS_IN_data_n;
  wire [2:0]TMDS_IN_data_p;
  wire TMDS_OUT_clk_n;
  wire TMDS_OUT_clk_p;
  wire [2:0]TMDS_OUT_data_n;
  wire [2:0]TMDS_OUT_data_p;
  wire ddc_scl_i;
  wire ddc_scl_io;
  wire ddc_scl_o;
  wire ddc_scl_t;
  wire ddc_sda_i;
  wire ddc_sda_io;
  wire ddc_sda_o;
  wire ddc_sda_t;
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
  wire [0:0]hdmi_hpd;
  wire [0:0]hdmi_rx_txen;
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

  IOBUF ddc_scl_iobuf
       (.I(ddc_scl_o),
        .IO(ddc_scl_io),
        .O(ddc_scl_i),
        .T(ddc_scl_t));
  IOBUF ddc_sda_iobuf
       (.I(ddc_sda_o),
        .IO(ddc_sda_io),
        .O(ddc_sda_i),
        .T(ddc_sda_t));
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
        .DDC_scl_i(ddc_scl_i),
        .DDC_scl_o(ddc_scl_o),
        .DDC_scl_t(ddc_scl_t),
        .DDC_sda_i(ddc_sda_i),
        .DDC_sda_o(ddc_sda_o),
        .DDC_sda_t(ddc_sda_t),
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
        .TMDS_IN_clk_n(TMDS_IN_clk_n),
        .TMDS_IN_clk_p(TMDS_IN_clk_p),
        .TMDS_IN_data_n(TMDS_IN_data_n),
        .TMDS_IN_data_p(TMDS_IN_data_p),
        .TMDS_OUT_clk_n(TMDS_OUT_clk_n),
        .TMDS_OUT_clk_p(TMDS_OUT_clk_p),
        .TMDS_OUT_data_n(TMDS_OUT_data_n),
        .TMDS_OUT_data_p(TMDS_OUT_data_p),
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
        .hdmi_hpd(hdmi_hpd),
        .hdmi_rx_txen(hdmi_rx_txen),
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