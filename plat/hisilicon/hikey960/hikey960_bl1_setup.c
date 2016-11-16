/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <arch_helpers.h>
#include <assert.h>
#include <bl_common.h>
#include <console.h>
#include <debug.h>
#include <errno.h>
#include <hi3660.h>
#include <mmio.h>
#include <platform.h>
#include <platform_def.h>
#include <sp804_delay_timer.h>
#include <string.h>
#include <tbbr/tbbr_img_desc.h>
#include <ufs.h>

#include "../../bl1/bl1_private.h"
#include "hikey960_def.h"
#include "hikey960_private.h"

enum {
	BOOT_MODE_RECOVERY = 0,
	BOOT_MODE_NORMAL,
	BOOT_MODE_MASK = 1,
};

/*
 * Declarations of linker defined symbols which will help us find the layout
 * of trusted RAM
 */
extern unsigned long __COHERENT_RAM_START__;
extern unsigned long __COHERENT_RAM_END__;

/*
 * The next 2 constants identify the extents of the coherent memory region.
 * These addresses are used by the MMU setup code and therefore they must be
 * page-aligned.  It is the responsibility of the linker script to ensure that
 * __COHERENT_RAM_START__ and __COHERENT_RAM_END__ linker symbols refer to
 * page-aligned addresses.
 */
#define BL1_COHERENT_RAM_BASE (unsigned long)(&__COHERENT_RAM_START__)
#define BL1_COHERENT_RAM_LIMIT (unsigned long)(&__COHERENT_RAM_END__)

/* Data structure which holds the extents of the trusted RAM for BL1 */
static meminfo_t bl1_tzram_layout;

meminfo_t *bl1_plat_sec_mem_layout(void)
{
	return &bl1_tzram_layout;
}

/*
 * Perform any BL1 specific platform actions.
 */
void bl1_early_platform_setup(void)
{
	const size_t bl1_size = BL1_RAM_LIMIT - BL1_RAM_BASE;

	/* Initialize the console to provide early debug support */
	console_init(CONSOLE_BASE, PL011_UART_CLK_IN_HZ, PL011_BAUDRATE);

	/* Allow BL1 to see the whole Trusted RAM */
	bl1_tzram_layout.total_base = BL1_RW_BASE;
	bl1_tzram_layout.total_size = BL1_RW_SIZE;

	/* Calculate how much RAM BL1 is using and how much remains free */
	bl1_tzram_layout.free_base = BL1_RW_BASE;
	bl1_tzram_layout.free_size = BL1_RW_SIZE;
	reserve_mem(&bl1_tzram_layout.free_base,
		    &bl1_tzram_layout.free_size,
		    BL1_RAM_BASE,
		    bl1_size);

	INFO("BL1: 0x%lx - 0x%lx [size = %lu]\n", BL1_RAM_BASE, BL1_RAM_LIMIT,
	     bl1_size);
}

/*
 * Perform the very early platform specific architecture setup here. At the
 * moment this only does basic initialization. Later architectural setup
 * (bl1_arch_setup()) does not do anything platform specific.
 */
void bl1_plat_arch_setup(void)
{
	hikey960_init_mmu_el3(bl1_tzram_layout.total_base,
			      bl1_tzram_layout.total_size,
			      BL1_RO_BASE,
			      BL1_RO_LIMIT,
			      BL1_COHERENT_RAM_BASE,
			      BL1_COHERENT_RAM_LIMIT);
}

static void hikey960_clk_init(void)
{
	mmio_write_32(CRG_REG_BASE + CRG_CLKDIV3_OFFSET, 0xf0001000);
}

static void hikey960_pmu_init(void)
{
	/* clear np_xo_abb_dig_START bit in PMIC_CLK_TOP_CTRL7 register */
	mmio_clrbits_32(PMU_SSI0_REG_BASE + PMU_SSI0_CLK_TOP_CTRL7_OFFSET,
			NP_XO_ABB_DIG);
}

static void hikey960_timer_init(void)
{
	unsigned int data;

	/* unreset TIMER9 */
	mmio_write_32(CRG_REG_BASE + CRG_PERRSTDIS1_OFFSET, PERI_TIMER9_BIT);
	do {
		data = mmio_read_32(CRG_REG_BASE + CRG_PERRSTSTAT1_OFFSET);
	} while (data & PERI_TIMER9_BIT);
	/* enable TIMER9 */
	mmio_write_32(CRG_REG_BASE + CRG_PEREN1_OFFSET, PERI_TIMER9_BIT);
	do {
		data = mmio_read_32(CRG_REG_BASE + CRG_PERSTAT1_OFFSET);
	} while ((data & PERI_TIMER9_BIT) == 0);
	sp804_timer_init(TIMER9_REG_BASE, 15625, 512); 		/* 32.768KHz */
}

static void hikey960_ufs_init(void)
{
	ufs_params_t ufs_params;

	memset(&ufs_params, 0, sizeof(ufs_params));
	ufs_params.reg_base = UFS_REG_BASE;
	ufs_params.desc_base = HIKEY960_UFS_DESC_BASE;
	ufs_params.desc_size = HIKEY960_UFS_DESC_SIZE;
	ufs_params.flags = UFS_FLAGS_SKIPINIT;
	ufs_init(NULL, &ufs_params);
}

/*
 * Function which will perform any remaining platform-specific setup that can
 * occur after the MMU and data cache have been enabled.
 */
void bl1_platform_setup(void)
{
	hikey960_clk_init();
	hikey960_pmu_init();
	hikey960_timer_init();
	hikey960_ufs_init();
	hikey960_io_setup();
}

/*
 * The following function checks if Firmware update is needed,
 * by checking if TOC in FIP image is valid or not.
 */
unsigned int bl1_plat_get_next_image_id(void)
{
	unsigned int mode, ret;

	mode = mmio_read_32(SCTRL_REG_BASE + SCTRL_BAK_DATA0_OFFSET);
	switch (mode & BOOT_MODE_MASK) {
	case BOOT_MODE_RECOVERY:
		ret = BL2U_IMAGE_ID;
		break;
	case BOOT_MODE_NORMAL:
		ret = BL2_IMAGE_ID;
		break;
	default:
		WARN("Invalid boot mode is found:%d\n", mode);
		panic();
	}
	return ret;
}

image_desc_t *bl1_plat_get_image_desc(unsigned int image_id)
{
	unsigned int index = 0;

	while (bl1_tbbr_image_descs[index].image_id != INVALID_IMAGE_ID) {
		if (bl1_tbbr_image_descs[index].image_id == image_id) {
			return &bl1_tbbr_image_descs[index];
		}
		index++;
	}

	return NULL;
}

void bl1_plat_set_ep_info(unsigned int image_id,
		entry_point_info_t *ep_info)
{
}