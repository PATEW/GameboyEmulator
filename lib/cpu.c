#include <cpu.h>
#include <bus.h>
#include <emu.h>

cpu_context ctx = {0};

void cpu_init() {
    ctx.registers.pc = 0x100;
    ctx.registers.a = 0x01;
}

static void fetch_instruction() {
    ctx.current_opcode = bus_read(ctx.registers.pc++);
    ctx.current_instruction = instruction_by_opcode(ctx.current_opcode);
}

void fetch_data();

static void execute() {
    IN_PROC proc = inst_get_processor(ctx.current_instruction->type);

    if (!proc) {
        NO_IMPL
    }

    proc(&ctx);
}

bool cpu_step() {

    if (!ctx.halted) {
        u16 pc = ctx.registers.pc;
        // aka fetch-decode-execute
        fetch_instruction();
        fetch_data();

        printf("%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X\n",
            pc, inst_name(ctx.current_instruction->type), ctx.current_opcode,
            bus_read(pc + 1), bus_read(pc + 2), ctx.registers.a, ctx.registers.b, ctx.registers.c);

        if (ctx.current_instruction == NULL) {
            printf("Unknown Instruction! %02X\n", ctx.current_opcode);
            exit(-7);
        }

        execute();
    }

    return true;
}
