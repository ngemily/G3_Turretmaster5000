`timescale 1 ns / 1 ps

module tb();

reg sys_clk;
reg reset;
reg dma_valid;
reg dma_ready;
reg [31:0] AXIS_TDATA;
wire [31:0] stream_out;

// Instantiate DUT
image_processing_ip_v1_0 dut
(   // Ports of Axi Slave Bus Interface S_AXI_LITE
       .s_axi_lite_aclk(sys_clk),
       .s_axi_lite_aresetn(reset),

    // Ports of Axi Slave Bus Interface S_AXIS_MM2S
       .s_axis_mm2s_aclk(sys_clk),
       .s_axis_mm2s_aresetn(reset),
       .s_axis_mm2s_tdata(AXIS_TDATA),
       .s_axis_mm2s_tvalid(dma_valid),

    // Ports of Axi Master Bus Interface M_AXIS_S2MM
       .m_axis_s2mm_aclk(sys_clk),
       .m_axis_s2mm_aresetn(reset),
       .m_axis_s2mm_tdata(stream_out),
       .m_axis_s2mm_tready(dma_ready)
);


always #5 sys_clk = ~sys_clk;
always #20 AXIS_TDATA  = ~AXIS_TDATA;

initial
begin
    sys_clk = 1'b0;
    AXIS_TDATA_WIDTH = 32'b0;
    dma_ready = 1'b0;
    dma_valid = 1'b0;
    reset = 1'b1;
    #50
    reset = 1'b0;
    #50
    reset = 1'b1;
    dma_ready = 1'b1;
    dma_valid = 1'b1;

end

endmodule
