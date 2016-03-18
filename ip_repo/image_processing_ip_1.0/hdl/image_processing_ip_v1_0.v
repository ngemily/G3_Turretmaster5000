
`timescale 1 ns / 1 ps

	module image_processing_ip_v1_0 #
	(
		// Users to add parameters here
        parameter integer FRAME_WIDTH = 1280,
        parameter integer FRAME_HEIGHT = 720,
		parameter integer AXIS_TDATA_WIDTH	= 24,
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
	
	function integer clogb2 (input integer bit_depth);
      begin
        for(clogb2=0; bit_depth>0; clogb2=clogb2+1)
          bit_depth = bit_depth >> 1;
      end
    endfunction

    // Total number of input data.
    localparam NUMBER_OF_INPUT_WORDS  = FRAME_WIDTH;
    // bit_num gives the minimum number of bits needed to address 'NUMBER_OF_INPUT_WORDS' size of FIFO.
    localparam bit_number  = clogb2(NUMBER_OF_INPUT_WORDS-1);	
       
    wire  [bit_number-1:0] write_pointer;
    wire  [bit_number-1:0] read_pointer;
    wire  tx_enable;
    wire  rx_enable;
    wire  rx_mst_exec_state;
    wire  [1:0] tx_mst_exec_state;        
    wire  [C_S_AXIS_MM2S_TDATA_WIDTH-1:0] stream_data_from_rx;
    wire  [C_M_AXIS_S2MM_TDATA_WIDTH-1:0] stream_data_to_tx;
    
    	
// Instantiation of Axi Bus Interface S_AXI_LITE
	image_processing_ip_v1_0_S_AXI_LITE # ( 
		.C_S_AXI_DATA_WIDTH(C_S_AXI_LITE_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S_AXI_LITE_ADDR_WIDTH),
		
        .AXIS_TDATA_WIDTH(AXIS_TDATA_WIDTH),
		.FRAME_WIDTH(FRAME_WIDTH),
        .FRAME_HEIGHT(FRAME_HEIGHT)
	) image_processing_ip_v1_0_S_AXI_LITE_inst (
	
        .write_pointer(write_pointer),
        .read_pointer(read_pointer),
        .rx_enable(rx_enable),
        .tx_enable(tx_enable),
        .stream_data_from_rx(stream_data_from_rx),
        .stream_data_to_tx(stream_data_to_tx),
        .rx_mst_exec_state(rx_mst_exec_state),
        .tx_mst_exec_state(tx_mst_exec_state),
        
        .mm2s_tready(s_axis_mm2s_tready),
        .mm2s_tvalid(s_axis_mm2s_tvalid),
        .s2mm_tvalid(m_axis_s2mm_tvalid),
        .s2mm_tready(m_axis_s2mm_tready),
        
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
	
        .write_pointer(write_pointer),
        .read_pointer(read_pointer),
        .rx_enable(rx_enable),
        .stream_data_from_rx(stream_data_from_rx),
        .mst_exec_state(rx_mst_exec_state),
        
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
	
        .write_pointer(write_pointer),
        .read_pointer(read_pointer),
        .tx_enable(tx_enable),
        .stream_data_to_tx(stream_data_to_tx),
        .mst_exec_state(tx_mst_exec_state),
        
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
