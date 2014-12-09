/*
 * Copyright (c) 2014, Hisilicon Ltd.
 * Copyright (c) 2014, Linaro Ltd.
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
#include <platform.h>
#include <platform_def.h>
#include <string.h>
#include "lcb_def.h"
#include "lcb_private.h"

/* Data structure which holds the extents of the trusted RAM for BL2 */
static meminfo_t bl2_tzram_layout
__attribute__ ((aligned(PLATFORM_CACHE_LINE_SIZE),
		section("tzfw_coherent_mem")));

/*******************************************************************************
 * Structure which holds the arguments which need to be passed to BL3-1
 ******************************************************************************/
static bl2_to_bl31_params_mem_t bl31_params_mem;

meminfo_t *bl2_plat_sec_mem_layout(void)
{
	return &bl2_tzram_layout;
}

/*******************************************************************************
 * This function assigns a pointer to the memory that the platform has kept
 * aside to pass platform specific and trusted firmware related information
 * to BL31. This memory is allocated by allocating memory to
 * bl2_to_bl31_params_mem_t structure which is a superset of all the
 * structure whose information is passed to BL31
 * NOTE: This function should be called only once and should be done
 * before generating params to BL31
 ******************************************************************************/
bl31_params_t *bl2_plat_get_bl31_params(void)
{
	bl31_params_t *bl2_to_bl31_params;

	/*
	 * Initialise the memory for all the arguments that needs to
	 * be passed to BL3-1
	 */
	memset(&bl31_params_mem, 0, sizeof(bl2_to_bl31_params_mem_t));

	/* Assign memory for TF related information */
	bl2_to_bl31_params = &bl31_params_mem.bl31_params;
	return bl2_to_bl31_params;
}

/*******************************************************************************
 * This function returns a pointer to the shared memory that the platform
 * has kept to point to entry point information of BL31 to BL2
 ******************************************************************************/
struct entry_point_info *bl2_plat_get_bl31_ep_info(void)
{
	return &bl31_params_mem.bl31_ep_info;
}

/*******************************************************************************
 * BL1 has passed the extents of the trusted RAM that should be visible to BL2
 * in x0. This memory layout is sitting at the base of the free trusted RAM.
 * Copy it to a safe loaction before its reclaimed by later BL2 functionality.
 ******************************************************************************/
void bl2_early_platform_setup(meminfo_t *mem_layout)
{
	/* Initialize the console to provide early debug support */
	console_init(PL011_UART0_BASE, PL011_UART0_CLK_IN_HZ, PL011_BAUDRATE);
}

/*******************************************************************************
 * Perform platform specific setup, i.e. initialize the IO layer, load BL3-0
 * image and initialise the memory location to use for passing arguments to
 * BL3-1.
 ******************************************************************************/
void bl2_platform_setup(void)
{
}

/* Flush the TF params and the TF plat params */
void bl2_plat_flush_bl31_params(void)
{
}

/*******************************************************************************
 * Perform the very early platform specific architectural setup here. At the
 * moment this is only intializes the mmu in a quick and dirty way.
 ******************************************************************************/
void bl2_plat_arch_setup(void)
{
}

/*******************************************************************************
 * Populate the extents of memory available for loading BL3-0, i.e. anywhere
 * in trusted RAM as long as it doesn't overwrite BL2.
 ******************************************************************************/
void bl2_plat_get_bl30_meminfo(meminfo_t *bl30_meminfo)
{
}

/*******************************************************************************
 * Transfer BL3-0 from Trusted RAM using the SCP Download protocol.
 * Return 0 on success, -1 otherwise.
 ******************************************************************************/
int bl2_plat_handle_bl30(image_info_t *bl30_image_info)
{
	return 0;
}

/*******************************************************************************
 * Before calling this function BL31 is loaded in memory and its entrypoint
 * is set by load_image. This is a placeholder for the platform to change
 * the entrypoint of BL31 and set SPSR and security state.
 * On Juno we are only setting the security state, entrypoint
 ******************************************************************************/
void bl2_plat_set_bl31_ep_info(image_info_t *bl31_image_info,
			       entry_point_info_t *bl31_ep_info)
{
}

/*******************************************************************************
 * Before calling this function BL32 is loaded in memory and its entrypoint
 * is set by load_image. This is a placeholder for the platform to change
 * the entrypoint of BL32 and set SPSR and security state.
 * On Juno we are only setting the security state, entrypoint
 ******************************************************************************/
void bl2_plat_set_bl32_ep_info(image_info_t *bl32_image_info,
			       entry_point_info_t *bl32_ep_info)
{
}

/*******************************************************************************
 * Before calling this function BL33 is loaded in memory and its entrypoint
 * is set by load_image. This is a placeholder for the platform to change
 * the entrypoint of BL33 and set SPSR and security state.
 * On Juno we are only setting the security state, entrypoint
 ******************************************************************************/
void bl2_plat_set_bl33_ep_info(image_info_t *image,
				       entry_point_info_t *bl33_ep_info)
{
}

/*******************************************************************************
 * Populate the extents of memory available for loading BL3-2
 ******************************************************************************/
void bl2_plat_get_bl32_meminfo(meminfo_t *bl32_meminfo)
{
}

/*******************************************************************************
 * Populate the extents of memory available for loading BL3-3
 ******************************************************************************/
void bl2_plat_get_bl33_meminfo(meminfo_t *bl33_meminfo)
{
}