#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vfifo.h"
#include <queue>

#define MAX_SIM_TIME 300
#define VERIF_START_TIME 7
vluint64_t sim_time = 0;
vluint64_t posedge_cnt = 0;


void dut_reset (Vfifo *dut, vluint64_t &sim_time){
    dut->n_rst = 1;
    if(sim_time >= 3 && sim_time < 6){
        dut->n_rst = 0;
        dut->wdata = 0;
        dut->fifo_pop = 0;
        dut->fifo_push = 0;
        dut->flush = 0;
    }
}


void check_out_valid(Vfifo *dut, 
    vluint64_t &sim_time, 
    Vfifo___024unit::operation_t expected_rdata 
  )
  {
    static unsigned char sim_rdata; //Read from the DUT
    static unsigned char sim_empty;
    static unsigned char sim_empty_d; //sim_empty delayed by one clock edge

    if (sim_time >= VERIF_START_TIME) {
    }
}


int main(int argc, char** argv, char** env) {
    srand (time(NULL));
    Verilated::commandArgs(argc, argv);
    Vfifo *dut = new Vfifo;

    Verilated::traceEverOn(true);
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    dut->trace(m_trace, 5);
    m_trace->open("waveform.vcd");

    //FIFO golden model, based on queue datatype
    std::queue<Vfifo___024unit::operation_t> expected_queue;
    const int num_entries = 12;
    static Vfifo___024unit::operation_t push_opcode;

    while (sim_time < MAX_SIM_TIME) {
        dut_reset(dut, sim_time);

        dut->clk ^= 1;
        dut->eval();

        if (dut->clk == 1){
           
            //Can either use posedge_cnt or sim_time. I prefer sim_time since it easily maps to the waveforms 
            posedge_cnt++;
            //switch (posedge_cnt){
            switch (sim_time){
                case 20:
                    push_opcode = Vfifo___024unit::operation_t::add;
                    dut->wdata = push_opcode;
                    dut->fifo_pop = 0;
                    dut->fifo_push = 1;
                    dut->flush = 0;

                    break;
                case 22:
                    push_opcode = Vfifo___024unit::operation_t::sub;
                    dut->wdata = push_opcode;
                    dut->fifo_pop = 0;
                    dut->fifo_push = 1;
                    dut->flush = 0;

                    break;
            }
            //check_out_valid(dut, sim_time, expected_queue.front(), expected_queue.empty());
        }

        m_trace->dump(sim_time);
        sim_time++;
    }

    m_trace->close();
    delete dut;
    exit(EXIT_SUCCESS);
}
