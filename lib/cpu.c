#include <instructions.h>
#include <cpu.h>
#include <bus.h>
#include <emu.h>

cpu_context ctx = {0};

void cpu_init() {
    ctx.registers.pc = 0x100;
}

static void fetch_instruction() {
    ctx.current_opcode = bus_read(ctx.registers.pc++);
    ctx.current_instruction = instruction_by_opcode(ctx.current_opcode);

    if (ctx.current_instruction == NULL) {
        printf("Unknown Instruction %02X\n", ctx.current_opcode);
    }
}

static void fetch_data() {
    ctx.memory_destination = 0;
    ctx.destination_is_memory = false;

    switch(ctx.current_instruction->mode) {
        case AM_IMP:
            return;
        case AM_R:
            ctx.fetched_data = cpu_read_reg(ctx.current_instruction->reg_1);
            return;
        case AM_R_D8:
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            return;
        case AM_D16: {
            u16 lo = bus_read(ctx.registers.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.registers.pc + 1);
            emu_cycles(1);

            ctx.fetched_data = lo | (hi << 8);

            ctx.registers.pc += 2;

            return;
        }

        default:
            printf("Unknown addressing mode...%d\n", ctx.current_instruction->mode);
            exit(-7);
            return;
    }
}

static void execute() {
    printf("not executing yet...\n");
}

bool cpu_step() {

    if (!ctx.halted) {
        u16 pc = ctx.registers.pc;
        // aka fetch-decode-execute
        fetch_instruction();
        printf("Executing instruction: %02X    PC: %04X\n", ctx.current_opcode, pc);
        fetch_data();
        execute();
    }

    return true;
}
