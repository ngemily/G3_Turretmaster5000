
`timescale 1 ns / 1 ps

	module image_processing_ip_v1_0_S_AXIS_MM2S #
	(
		// Users to add parameters here
        parameter integer FRAME_WIDTH = 1280,
        parameter integer FRAME_HEIGHT = 720,
		// User parameters ends
		// Do not modify the parameters beyond this line

		// AXI4Stream sink: Data Width
		parameter integer C_S_AXIS_TDATA_WIDTH	= 24
	)
	(
		// Users to add ports here
        output reg [C_S_AXIS_TDATA_WIDTH-1 : 0] buf_outof_rx,
        output reg [31:0] curr_pix_row, // row of current input pixel in frame
        output reg [31:0] curr_pix_col, // column of current input pixel in frame
        output wire rx_en,
        input wire  tx_en,
        input wire tx_mst_exec_state,

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
	// function called clogb2 that returns an integer which has the 
	// value of the ceiling of the log base 2.
	function integer clogb2 (input integer bit_depth);
	  begin
	    for(clogb2=0; bit_depth>0; clogb2=clogb2+1)
	      bit_depth = bit_depth >> 1;
	  end
	endfunction

	// Total number of input data.
	localparam NUMBER_OF_INPUT_WORDS = FRAME_WIDTH*FRAME_HEIGHT;
	// bit_num gives the minimum number of bits needed to address 'NUMBER_OF_INPUT_WORDS' size of FIFO.
	localparam bit_num  = clogb2(NUMBER_OF_INPUT_WORDS-1);
	// Define the states of state machine
	// The control state machine oversees the writing of input streaming data to the FIFO,
	// and outputs the streaming data from the FIFO
	localparam [1:0] IDLE = 1'b0,        // This is the initial/idle state 

	                WRITE_FIFO  = 1'b1; // In this state FIFO is written with the
	                                    // input stream data S_AXIS_TDATA 
	wire  	axis_tready;
	// State variable
	reg mst_exec_state;  
	// FIFO implementation signals
	genvar byte_index;     
	// FIFO write enable
	//wire fifo_wren;
	// FIFO full flag
	reg fifo_full_flag;
	// FIFO write pointer
	reg [bit_num-1:0] write_counter;
	// sink has accepted all the streaming data and stored in FIFO
	  reg writes_done;
	// I/O Connections assignments

	assign S_AXIS_TREADY	= axis_tready;
	// Control state machine implementation
	always @(posedge S_AXIS_ACLK) 
	begin  
	  if (!S_AXIS_ARESETN) 
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
	assign axis_tready = ((mst_exec_state == WRITE_FIFO) && (write_counter <= NUMBER_OF_INPUT_WORDS-1));
	
	// FIFO write enable generation
	assign rx_en = S_AXIS_TVALID && axis_tready;

    wire rx_mst_exec_state = mst_exec_state; //
    wire rxtx_en = rx_en && tx_en; // 
    wire rx_load = rx_en && (tx_mst_exec_state == 2'b01); // tx_mst_exec_state == INIT_COUNTER
    wire tx_flush = tx_en && (rx_mst_exec_state == IDLE); //


	always@(posedge S_AXIS_ACLK)
	begin
	    if(!S_AXIS_ARESETN)
	    begin
	        write_counter <= 0;
	        writes_done <= 1'b0;
	    end  
	    else
	    begin
	        if (write_counter < NUMBER_OF_INPUT_WORDS)
	        begin
	            if (rxtx_en || rx_load)
	            begin
	                // write pointer is incremented after every write to the FIFO
	                // when FIFO write signal is enabled.
	                write_counter <= write_counter + 1;
	                writes_done <= 1'b0;
	            end
	            else 
	            begin 
	                write_counter <= write_counter;
                    writes_done <= 1'b0;
                end
            end
            if ((write_counter == NUMBER_OF_INPUT_WORDS-1)|| S_AXIS_TLAST)
            begin
                // reads_done is asserted when NUMBER_OF_INPUT_WORDS numbers of streaming data 
                // has been written to the FIFO which is also marked by S_AXIS_TLAST(kept for optional usage).
                writes_done <= 1'b1;
	            write_counter <= 0;
            end
	    end  
	end


	// FIFO Implementation
    
    
    //reg  [C_S_AXIS_TDATA_WIDTH-1:0] stream_data_fifo [0 : NUMBER_OF_INPUT_WORDS-1];
    
    // Streaming input data is stored in FIFO
    
    /*
    
    always @( posedge S_AXIS_ACLK )
    begin
      if (fifo_wren)// && S_AXIS_TSTRB[byte_index])
        begin
          stream_data_fifo[write_pointer] <= S_AXIS_TDATA[C_S_AXIS_TDATA_WIDTH-1:0];
        end  
    end 
    
    */
     
	// Add user logic here    
    reg tlast_latch;
    wire tlast_edge;
    
    
    always @( posedge S_AXIS_ACLK )
    begin
        // rx_en <= fifo_wren; // delay the rx_in_en signal to align with the loading of buf_outof_rx
        if (rxtx_en || rx_load) begin
            buf_outof_rx <= S_AXIS_TDATA[C_S_AXIS_TDATA_WIDTH-1:0];
        end  
    end  
    
    always @( posedge S_AXIS_ACLK )
    begin
        tlast_latch <= S_AXIS_TLAST; 
    end
    
    assign tlast_edge = S_AXIS_TLAST & ~tlast_latch;

    
    
    reg hsync;
    
    always @( posedge S_AXIS_ACLK )
    begin
        if (!S_AXIS_ARESETN) begin
            curr_pix_col <= 0;
            curr_pix_row <= 0;
        end
        if (rx_en) begin
            hsync = 1;
        end
        if (rx_en & (curr_pix_col < FRAME_WIDTH)) begin
            curr_pix_col <= curr_pix_col + 1;
            curr_pix_row <= curr_pix_row;
        end
        else if (rx_en) begin
            curr_pix_col <= 0;
            curr_pix_row <= curr_pix_row + 1; 
        end  
    end  
    
	// User logic ends

	endmodule
