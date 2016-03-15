
`timescale 1 ns / 1 ps

	module image_processing_ip_v1_0 #
	(
		// Users to add parameters here
        parameter integer FRAME_WIDTH = 1280,
        parameter integer FRAME_HEIGHT = 720,
		parameter integer AXIS_TDATA_WIDTH	= 24,
        parameter integer WORDS_PER_LINE = 1280,
		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S_AXI_LITE
		parameter integer C_S_AXI_LITE_DATA_WIDTH	= 32,
		parameter integer C_S_AXI_LITE_ADDR_WIDTH	= 7,

		// Parameters of Axi Slave Bus Interface S_AXIS_MM2S
		parameter integer C_S_AXIS_MM2S_TDATA_WIDTH	= 24,

		// Parameters of Axi Master Bus Interface M_AXIS_S2MM
		parameter integer C_M_AXIS_S2MM_TDATA_WIDTH	= 24,
		parameter integer C_M_AXIS_S2MM_START_COUNT	= 32
	)
	(
		// Users to add ports here

		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S_AXI_LITE
		input wire  s_axi_lite_aclk,
		input wire  s_axi_lite_aresetn,
		input wire [C_S_AXI_LITE_ADDR_WIDTH-1 : 0] s_axi_lite_awaddr,
		input wire [2 : 0] s_axi_lite_awprot,
		input wire  s_axi_lite_awvalid,
		output wire  s_axi_lite_awready,
		input wire [C_S_AXI_LITE_DATA_WIDTH-1 : 0] s_axi_lite_wdata,
		input wire [(C_S_AXI_LITE_DATA_WIDTH/8)-1 : 0] s_axi_lite_wstrb,
		input wire  s_axi_lite_wvalid,
		output wire  s_axi_lite_wready,
		output wire [1 : 0] s_axi_lite_bresp,
		output wire  s_axi_lite_bvalid,
		input wire  s_axi_lite_bready,
		input wire [C_S_AXI_LITE_ADDR_WIDTH-1 : 0] s_axi_lite_araddr,
		input wire [2 : 0] s_axi_lite_arprot,
		input wire  s_axi_lite_arvalid,
		output wire  s_axi_lite_arready,
		output wire [C_S_AXI_LITE_DATA_WIDTH-1 : 0] s_axi_lite_rdata,
		output wire [1 : 0] s_axi_lite_rresp,
		output wire  s_axi_lite_rvalid,
		input wire  s_axi_lite_rready,

		// Ports of Axi Slave Bus Interface S_AXIS_MM2S
		input wire  s_axis_mm2s_aclk,
		input wire  s_axis_mm2s_aresetn,
		output wire  s_axis_mm2s_tready,
		input wire [C_S_AXIS_MM2S_TDATA_WIDTH-1 : 0] s_axis_mm2s_tdata,
		input wire [(C_S_AXIS_MM2S_TDATA_WIDTH/8)-1 : 0] s_axis_mm2s_tstrb,
		input wire  s_axis_mm2s_tlast,
		input wire  s_axis_mm2s_tvalid,

		// Ports of Axi Master Bus Interface M_AXIS_S2MM
		input wire  m_axis_s2mm_aclk,
		input wire  m_axis_s2mm_aresetn,
		output wire  m_axis_s2mm_tvalid,
		output wire [C_M_AXIS_S2MM_TDATA_WIDTH-1 : 0] m_axis_s2mm_tdata,
		output wire [(C_M_AXIS_S2MM_TDATA_WIDTH/8)-1 : 0] m_axis_s2mm_tstrb,
		output wire  m_axis_s2mm_tlast,
		input wire  m_axis_s2mm_tready
	);
	
    wire [C_S_AXIS_MM2S_TDATA_WIDTH-1 : 0] wire_rx2lite;
    wire [C_M_AXIS_S2MM_TDATA_WIDTH-1 : 0] wire_lite2tx;
        wire rx_in_en;
    wire [31:0] curr_pix_row;
    wire [31:0] curr_pix_col;
    wire rx_en;
    wire tx_en;
    wire tx_mst_exec_state;
    wire rx_mst_exec_state;

    wire rxtx_en = rx_en && tx_en; // 
    wire rx_load = rx_en && (tx_mst_exec_state == 2'b01); // tx_mst_exec_state == INIT_COUNTER
    wire tx_flush = tx_en && (rx_mst_exec_state == 1'b0); // rx_mst_exec_state == IDLE
    
// Instantiation of Axi Bus Interface S_AXI_LITE
	image_processing_ip_v1_0_S_AXI_LITE # ( 
		.C_S_AXI_DATA_WIDTH(C_S_AXI_LITE_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S_AXI_LITE_ADDR_WIDTH),
		.FRAME_WIDTH(FRAME_WIDTH),
        .FRAME_HEIGHT(FRAME_HEIGHT),
        .AXIS_TDATA_WIDTH(AXIS_TDATA_WIDTH)
	) image_processing_ip_v1_0_S_AXI_LITE_inst (
        .buf_from_rx(wire_rx2lite),
        .buf_to_tx(wire_lite2tx),
        .rx_in_en(rx_in_en),
	//.rxtx_en(rxtx_en),
        //.rx_load(rx_load),
        //.tx_flush(tx_flush),
	//    .curr_pix_row(curr_pix_row),
        //.curr_pix_col(curr_pix_col),
		.S_AXI_ACLK(s_axi_lite_aclk),
		.S_AXI_ARESETN(s_axi_lite_aresetn),
		.S_AXI_AWADDR(s_axi_lite_awaddr),
		.S_AXI_AWPROT(s_axi_lite_awprot),
		.S_AXI_AWVALID(s_axi_lite_awvalid),
		.S_AXI_AWREADY(s_axi_lite_awready),
		.S_AXI_WDATA(s_axi_lite_wdata),
		.S_AXI_WSTRB(s_axi_lite_wstrb),
		.S_AXI_WVALID(s_axi_lite_wvalid),
		.S_AXI_WREADY(s_axi_lite_wready),
		.S_AXI_BRESP(s_axi_lite_bresp),
		.S_AXI_BVALID(s_axi_lite_bvalid),
		.S_AXI_BREADY(s_axi_lite_bready),
		.S_AXI_ARADDR(s_axi_lite_araddr),
		.S_AXI_ARPROT(s_axi_lite_arprot),
		.S_AXI_ARVALID(s_axi_lite_arvalid),
		.S_AXI_ARREADY(s_axi_lite_arready),
		.S_AXI_RDATA(s_axi_lite_rdata),
		.S_AXI_RRESP(s_axi_lite_rresp),
		.S_AXI_RVALID(s_axi_lite_rvalid),
		.S_AXI_RREADY(s_axi_lite_rready)
	);

        
// Instantiation of Axi Bus Interface S_AXIS_MM2S
	image_processing_ip_v1_0_S_AXIS_MM2S # ( 
		.C_S_AXIS_TDATA_WIDTH(C_S_AXIS_MM2S_TDATA_WIDTH),
        .FRAME_WIDTH(FRAME_WIDTH),
        .FRAME_HEIGHT(FRAME_HEIGHT)
	) image_processing_ip_v1_0_S_AXIS_MM2S_inst (
	    .buf_outof_rx(wire_rx2lite),
        	    .rx_in_en(rx_in_en),
	//.rx_en(rx_en),
        //.tx_en(tx_en),
        //.tx_mst_exec_state(tx_mst_exec_state),
	//    .curr_pix_row(curr_pix_row),
	//    .curr_pix_col(curr_pix_col),
		.S_AXIS_ACLK(s_axis_mm2s_aclk),
		.S_AXIS_ARESETN(s_axis_mm2s_aresetn),
		.S_AXIS_TREADY(s_axis_mm2s_tready),
		.S_AXIS_TDATA(s_axis_mm2s_tdata),
		.S_AXIS_TSTRB(s_axis_mm2s_tstrb),
		.S_AXIS_TLAST(s_axis_mm2s_tlast),
		.S_AXIS_TVALID(s_axis_mm2s_tvalid)
	);

// Instantiation of Axi Bus Interface M_AXIS_S2MM
	image_processing_ip_v1_0_M_AXIS_S2MM # ( 
		.C_M_AXIS_TDATA_WIDTH(C_M_AXIS_S2MM_TDATA_WIDTH),
		.C_M_START_COUNT(C_M_AXIS_S2MM_START_COUNT),
		.FRAME_WIDTH(FRAME_WIDTH),
		.FRAME_HEIGHT(FRAME_HEIGHT)
	) image_processing_ip_v1_0_M_AXIS_S2MM_inst (
        .buf_into_tx(wire_lite2tx),
        .rx_in_en(rx_in_en),        
	//.rx_en(rx_en),
        //.tx_en(tx_en),
        //.rx_mst_exec_state(rx_mst_exec_state),
		.M_AXIS_ACLK(m_axis_s2mm_aclk),
		.M_AXIS_ARESETN(m_axis_s2mm_aresetn),
		.M_AXIS_TVALID(m_axis_s2mm_tvalid),
		.M_AXIS_TDATA(m_axis_s2mm_tdata),
		.M_AXIS_TSTRB(m_axis_s2mm_tstrb),
		.M_AXIS_TLAST(m_axis_s2mm_tlast),
		.M_AXIS_TREADY(m_axis_s2mm_tready)
	);

	// Add user logic here
	// User logic ends

	endmodule
