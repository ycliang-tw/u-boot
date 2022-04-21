// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2017 Andes Technology Corporation
 * Rick Chen, Andes Technology Corporation <rick@andestech.com>
 */

/* CPU specific code */
#include <common.h>
#include <cpu_func.h>
#include <irq_func.h>
#include <asm/cache.h>
#include <asm/csr.h>

#define CSR_MMISC_CTL							0x7d0

/* MMISC control register */
#define V5_MMISC_CTL_NON_BLOCKING_OFFSET		8

#define V5_MMISC_CTL_NON_BLOCKING_EN			BIT(V5_MMISC_CTL_NON_BLOCKING_OFFSET)

/*
 * cleanup_before_linux() is called just before we call linux
 * it prepares the processor for linux
 *
 * we disable interrupt and caches.
 */
int cleanup_before_linux(void)
{
	disable_interrupts();

	/* turn off I/D-cache */
#if !CONFIG_IS_ENABLED(RISCV_SMODE)
	cache_flush();
	icache_disable();
	dcache_disable();
#else
	invalidate_icache_all();
	if(!icache_status())
		icache_enable();

	if(!dcache_status())
		dcache_enable();
#endif

	return 0;
}

void harts_early_init(void)
{
	if (CONFIG_IS_ENABLED(RISCV_MMODE)) {
        unsigned long long mmisc_ctl_val = csr_read(CSR_MMISC_CTL);

		if (!(mmisc_ctl_val & V5_MMISC_CTL_NON_BLOCKING_EN))
			mmisc_ctl_val |= V5_MMISC_CTL_NON_BLOCKING_EN;

		csr_write(CSR_MMISC_CTL, mmisc_ctl_val);
	}
}
