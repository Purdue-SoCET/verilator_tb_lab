`timescale 1ns/10ps

typedef enum logic [1:0] {
     nop     = 2'h0,
     add     = 2'h1,
     sub     = 2'h2,
     mul     = 2'h3
} operation_t /*verilator public*/;

module fifo #(parameter NUM_ENTRIES=12)(
    input clk, n_rst,
    input operation_t wdata,
    input fifo_pop, fifo_push, flush,
    output operation_t rdata,
    output logic full, empty
);

localparam COUNTER_WIDTH = $clog2(NUM_ENTRIES);

operation_t mem[NUM_ENTRIES-1:0];
logic [COUNTER_WIDTH-1:0] read_ptr, write_ptr;
logic [COUNTER_WIDTH-1:0] read_ptr_nxt, write_ptr_nxt;
logic full_nxt, empty_nxt;
integer i;

//Assign statements
assign rdata = mem[read_ptr];

//Next state logic
always_comb begin
    read_ptr_nxt = read_ptr;
    write_ptr_nxt = write_ptr;
    empty_nxt = empty && read_ptr == write_ptr;
    full_nxt = full && read_ptr == write_ptr;
    if(fifo_pop && !empty) begin
        read_ptr_nxt = read_ptr + 1;
        empty_nxt = write_ptr == read_ptr;
        full_nxt = 0;
    end
    else if(fifo_push && !full) begin
        write_ptr_nxt = write_ptr + 1;
        full_nxt = write_ptr_nxt == read_ptr;
        empty_nxt = 0;
    end
    
end


always_ff @ (posedge clk, negedge n_rst) begin
    write_ptr <= write_ptr_nxt;
    read_ptr <= read_ptr_nxt;
    full <= full_nxt;
    empty <= empty_nxt;

    //Reset case
    if(!n_rst) begin
        for(i = 0; i < NUM_ENTRIES; i++) begin
            mem[i] <= nop;
        end
        read_ptr <= 0;
        write_ptr <= 1;
        full <= 0;
        empty <= 1;
    end //End reset case

    else if(flush) begin
        for(i = 0; i < NUM_ENTRIES; i++) begin
            mem[i] <= nop;
        end
        read_ptr <= 1;
        write_ptr <= 0;
        full <= 0;
        empty <= 1;
    end

    else if(fifo_push && !full) begin
        mem[write_ptr] <= wdata;
    end

end

endmodule
