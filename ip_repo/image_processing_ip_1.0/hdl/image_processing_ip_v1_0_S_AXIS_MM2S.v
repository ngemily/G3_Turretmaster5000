`timescale 1 ns / 1 ps

	module image_processing_ip_v1_0_S_AXIS_MM2S #
	(
		// Users to add parameters here
        parameter integer FRAME_WIDTH = 1280,
        parameter integer FRAME_HEIGHT = 720,
        parameter integer FIFO_SIZE = 1024,
        parameter integer fifo_bits = 10,
        parameter integer line_bits = 11,
		// User parameters ends
		// Do not modify the parameters beyond this line

		// AXI4Stream sink: Data Width
		parameter integer C_S_AXIS_TDATA_WIDTH	= 24
	)
	(
		// Users to add ports here
        output reg [line_bits-1:0] write_pointer, 	                     // FIFO write pointer
        output wire rx_en,	                                             // FIFO write enable
	    output wire [C_S_AXIS_TDATA_WIDTH-1:0] stream_data_from_rx,
        input wire [fifo_bits-1:0] rx_fifo_track,
        output reg mst_exec_state,                                       // State variable
        input wire AXIS_FRAME_RESETN,
		// User ports ends
		// Do not modify the ports beyond this line

		// AXI4Stream sink: Clock
		input wire  S_AXIS_ACLK,
		// AXI4Stream sink: Reset
		input wire  S_AXIS_ARESETN,
		// Ready to accept data in
		output wire  S_AXIS_TREADY,
		// Data in
		input wire [C_S_AXIS_TDATA_WIDTH-1 : 0] S_AXIS_TDATA,
		// Byte qualifier
		input wire [(C_S_AXIS_TDATA_WIDTH/8)-1 : 0] S_AXIS_TSTRB,
		// Indicates boundary of last packet
		input wire  S_AXIS_TLAST,
		// Data is in valid
		input wire  S_AXIS_TVALID
	);

	// Define the states of state machine
	// The control state machine oversees the writing of input streaming data to the FIFO,
	// and outputs the streaming data from the FIFO
	localparam [1:0] IDLE = 1'b0,        // This is the initial/idle state

	                WRITE_FIFO  = 1'b1; // In this state FIFO is written with the
	                                    // input stream data S_AXIS_TDATA
	wire  	axis_tready;
	// FIFO full flag
	reg fifo_full_flag;
	// sink has accepted all the streaming data and stored in FIFO
	reg writes_done;
	// I/O Connections assignments

	assign S_AXIS_TREADY	= axis_tready;
	// Control state machine implementation
	always @(posedge S_AXIS_ACLK)
	begin
	  if (!S_AXIS_ARESETN || !AXIS_FRAME_RESETN)
	  // Synchronous reset (active low)
	    begin
	      mst_exec_state <= IDLE;
	    end
	  else
	    case (mst_exec_state)
	      IDLE:
	        // The sink starts accepting tdata when
	        // there tvalid is asserted to mark the
	        // presence of valid streaming data
	          if (S_AXIS_TVALID)
	            begin
	              mst_exec_state <= WRITE_FIFO;
	            end
	          else
	            begin
	              mst_exec_state <= IDLE;
	            end
	      WRITE_FIFO:
	        // When the sink has accepted all the streaming input data,
	        // the interface swiches functionality to a streaming master
	        if (writes_done)
	          begin
	            mst_exec_state <= IDLE;
	          end
	        else
	          begin
	            // The sink accepts and stores tdata
	            // into FIFO
	            mst_exec_state <= WRITE_FIFO;
	          end

	    endcase
	end
	// AXI Streaming Sink
	//
	// The example design sink is always ready to accept the S_AXIS_TDATA  until
	// the FIFO is not filled with NUMBER_OF_INPUT_WORDS number of input words.
	assign axis_tready = ((mst_exec_state == WRITE_FIFO) && (write_pointer < FRAME_WIDTH) && (rx_fifo_track < FIFO_SIZE));

	always@(posedge S_AXIS_ACLK)
	begin
	    if (!S_AXIS_ARESETN || !AXIS_FRAME_RESETN)
	    begin
            write_pointer <= 0;
            writes_done <= 1'b0;
	    end
        else
        begin
            if (write_pointer < FRAME_WIDTH)
            begin
                if (rx_en)
	            begin
                    // write pointer is incremented after every write to the FIFO
	                // when FIFO write signal is enabled.
	                write_pointer <= write_pointer + 1;
	                writes_done <= 1'b0;
                end
            end
	        else if ((write_pointer >= FRAME_WIDTH)|| S_AXIS_TLAST)
            begin
	            // reads_done is asserted when NUMBER_OF_INPUT_WORDS numbers of streaming data
	            // has been written to the FIFO which is also marked by S_AXIS_TLAST(kept for optional usage).
	            writes_done <= 1'b1;
                write_pointer <= 0;
	        end
        end
	end

	// FIFO write enable generation
	assign rx_en = S_AXIS_TVALID && axis_tready;
	assign stream_data_from_rx[C_S_AXIS_TDATA_WIDTH-1:0] = S_AXIS_TDATA[C_S_AXIS_TDATA_WIDTH-1:0];

	// Add user logic here

    // User logic ends

	endmodule
