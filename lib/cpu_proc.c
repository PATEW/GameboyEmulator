#include "instructions.h"
#include <cpu.h>
#include <emu.h>
#include <bus.h>

//process CPU instructions

void cpu_set_flags(cpu_context *ctx, char z, char n, char h, char c) {
    // set bit value to value passed in for each
    if (z != -1) {
        BIT_SET(ctx->registers.f, 7, z);
    }

    if (n != -1) {
        BIT_SET(ctx->registers.f, 6, n);
    }

    if (h != -1) {
            BIT_SET(ctx->registers.f, 5, h);
        }

    if (c != -1) {
            BIT_SET(ctx->registers.f, 4, c);
        }
}

static void proc_none(cpu_context *ctx) {
    printf("Invalid Instruction _____\n");
    exit(-7);
}

static void proc_nop(cpu_context *ctx) {

}

static void proc_di(cpu_context *ctx) {
    ctx->interrupt_master_enabled = false;
}

static void proc_ld(cpu_context *ctx) {
    if (ctx->destination_is_memory) {
        //such as load BC, A

        if (ctx->current_instruction->reg_2 >=RT_AF) { // if 16 bit register
            emu_cycles(1);
            bus_write(ctx->memory_destination, ctx->fetched_data);
        } else {
            bus_write(ctx->memory_destination, ctx->fetched_data);
        }
        return;
    }

    if (ctx->current_instruction->mode == AM_HL_SPR) {
        u8 hflag = (cpu_read_reg(ctx->current_instruction->reg_2) & 0xF) +
            (ctx->fetched_data & 0xF) >= 0x10;

        u8 cflag = (cpu_read_reg(ctx->current_instruction->reg_2) & 0xFF) +
            (ctx->fetched_data & 0xFF) >= 0x100;

        cpu_set_flags(ctx, 0, 0, hflag, cflag);
        cpu_set_reg(ctx->current_instruction->reg_1,
            cpu_read_reg(ctx->current_instruction->reg_2) + (char)ctx->fetched_data);
        return;
    }

    cpu_set_reg(ctx->current_instruction->reg_1, ctx->fetched_data);
}

static void proc_xor(cpu_context *ctx) {
    ctx->registers.a ^= ctx->fetched_data & 0x00FF; // get only the bottom 8 bits from fetched data, then xor
    cpu_set_flags(ctx, ctx->registers.a == 0, 0, 0, 0);
}

static bool check_condition(cpu_context *ctx) {
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch(ctx->current_instruction->cond) {
        case CT_NONE: return true; // condition type: none
        case CT_C: return c;
        case CT_NC: return !c;
        case CT_Z: return z;
        case CT_NZ: return !z;
    }

    return false;
}

static void proc_jp(cpu_context *ctx) {
    if (check_condition(ctx)) {
        ctx->registers.pc = ctx->fetched_data;
        emu_cycles(1);
    }
}

static IN_PROC processors[] = {
    [IN_NONE] = proc_none,
    [IN_NOP] = proc_nop,
    [IN_LD] = proc_ld,
    [IN_JP] = proc_jp,
    [IN_DI] = proc_di, // Disable Interrupt
    [IN_XOR] = proc_xor
};

IN_PROC inst_get_processor(in_type type) {
    return processors[type];
}
