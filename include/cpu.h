#pragma once

#include <common.h>
#include <instructions.h>

typedef struct {
    u8 a;
    u8 f;
    u8 b;
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 pc;
    u16 sp;
} cpu_registers;

typedef struct {
    cpu_registers registers;

    // current fetch
    u16 fetched_data;
    u16 memory_destination;
    bool destination_is_memory;
    u8 current_opcode;
    instruction *current_instruction;

    bool halted;
    bool stepping;
} cpu_context;

void cpu_init();
bool cpu_step();

u16 cpu_read_reg(reg_type rt);
