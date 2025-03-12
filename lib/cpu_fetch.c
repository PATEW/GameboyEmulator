#include "instructions.h"
#include <cpu.h>
#include <bus.h>
#include <emu.h>

extern cpu_context ctx;

void fetch_data() {
    ctx.memory_destination = 0;
    ctx.destination_is_memory = false;

    if (ctx.current_instruction == NULL) {
        return;
    }

    switch(ctx.current_instruction->mode) {
        case AM_IMP: return;

        case AM_R:
            ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_1);
            return;

        case AM_R_R:
            ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);
            return;

        case AM_R_D8:
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            return;

        case AM_R_D16:
        case AM_D16: {
            u16 lo = bus_read(ctx.registers.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.registers.pc + 1);
            emu_cycles(1);

            ctx.fetched_data = lo | (hi << 8);

            ctx.registers.pc += 2;

            return;
        }

        case AM_MR_R:
            ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg_1);
            ctx.destination_is_memory = true;

            if (ctx.current_instruction->reg_1 == RT_C) {
                ctx.memory_destination |= 0xFF00;
            }

            return;

        case AM_R_MR: {
            u16 addr = cpu_read_reg(ctx.current_instruction->reg_2);

            if (ctx.current_instruction->reg_2 == RT_C) {
                addr |= 0xFF00;
            }

            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);

        } return;

        case AM_R_HLI:
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.current_instruction->reg_2));
            emu_cycles(1);
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;

        case AM_R_HLD:
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.current_instruction->reg_2));
            emu_cycles(1);
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;

        case AM_HLI_R:
            ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg_1);
            ctx.destination_is_memory = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;

        case AM_HLD_R:
            ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg_1);
            ctx.destination_is_memory = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;

        case AM_R_A8:
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            return;

        case AM_A8_R:
            ctx.memory_destination = bus_read(ctx.registers.pc) | 0xFF00;
            ctx.destination_is_memory = true;
            emu_cycles(1);
            ctx.registers.pc++;
            return;

        case AM_HL_SPR:
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            return;

        case AM_D8:
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            return;

        case AM_A16_R:
        case AM_D16_R: {
            u16 lo = bus_read(ctx.registers.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.registers.pc + 1);
            emu_cycles(1);

            ctx.memory_destination = lo | (hi << 8);
            ctx.destination_is_memory = true;

            ctx.registers.pc += 2;
            ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_2);

        } return;

        case AM_MR_D8:
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg_1);
            ctx.destination_is_memory = true;
            return;

        case AM_MR:
            ctx.memory_destination = cpu_read_reg(ctx.current_instruction->reg_1);
            ctx.destination_is_memory = true;
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.current_instruction->reg_1));
            emu_cycles(1);
            return;

        case AM_R_A16: {
            u16 lo = bus_read(ctx.registers.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.registers.pc + 1);
            emu_cycles(1);

            u16 addr = lo | (hi << 8);

            ctx.registers.pc += 2;
            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);

            return;
        }

        default:
            printf("Unknown Addressing Mode! %d (%02X)\n", ctx.current_instruction->mode, ctx.current_opcode);
            exit(-7);
            return;
    }
}
