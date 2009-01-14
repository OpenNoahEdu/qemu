#include "hw.h"
#include "mips.h"
#include "cpu.h"

/* Raise IRQ to CPU if necessary. It must be called every time the active
   IRQ may change */
void cpu_mips_update_irq(CPUState *env)
{
//	if (interrupt_request & CPU_INTERRUPT_HARD)
//{
//if (env->CP0_Cause&0x400)
//{
//	printf("cpu_mips_update_irq \n");
//	printf("env->CP0_Status %x env->CP0_Cause %x CP0Ca_IP_mask %x \n",env->CP0_Status,env->CP0_Cause,CP0Ca_IP_mask);
//	printf("CP0St_IE %x CP0St_EXL %x CP0St_ERL %x \n",CP0St_IE,CP0St_EXL,CP0St_ERL);
//	printf("env->hflags %x MIPS_HFLAG_DM %x \n",env->hflags ,MIPS_HFLAG_DM);
//}
//}
    if ((env->CP0_Status & (1 << CP0St_IE)) &&
        !(env->CP0_Status & (1 << CP0St_EXL)) &&
        !(env->CP0_Status & (1 << CP0St_ERL)) &&
        !(env->hflags & MIPS_HFLAG_DM)) {
        if ((env->CP0_Status & env->CP0_Cause & CP0Ca_IP_mask) &&
            !(env->interrupt_request & CPU_INTERRUPT_HARD)) {
            printf("cpu_mips_update_irq \n");
            cpu_interrupt(env, CPU_INTERRUPT_HARD);
	}
    } else
        cpu_reset_interrupt(env, CPU_INTERRUPT_HARD);
}

static void cpu_mips_irq_request(void *opaque, int irq, int level)
{
    CPUState *env = (CPUState *)opaque;

    if (irq < 0 || irq > 7)
        return;

    if (level) {
    	 env->CP0_Cause |= 1 << (irq + CP0Ca_IP);
    	 if (env->CP0_Status & 0x1)
    	 	{
    	 printf("irq %d env->CP0_Cause %x \n",irq,env->CP0_Cause);
    	 printf("status %x\n",env->CP0_Status);
    	 	}
    } else {
        env->CP0_Cause &= ~(1 << (irq + CP0Ca_IP));
        //printf("clear irq %d env->CP0_Cause %x \n",irq,env->CP0_Cause);
    }
    cpu_mips_update_irq(env);
}

void cpu_mips_irq_init_cpu(CPUState *env)
{
    qemu_irq *qi;
    int i;

    qi = qemu_allocate_irqs(cpu_mips_irq_request, env, 8);
    for (i = 0; i < 8; i++) {
        env->irq[i] = qi[i];
    }
}
