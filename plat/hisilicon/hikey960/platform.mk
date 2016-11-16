#
# Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# Neither the name of ARM nor the names of its contributors may be used
# to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

CONSOLE_BASE			:=	PL011_UART5_BASE
CRASH_CONSOLE_BASE		:=	PL011_UART5_BASE
COLD_BOOT_SINGLE_CPU		:=	1
PROGRAMMABLE_RESET_ADDRESS	:=	1
PLAT_PARTITION_BLOCK_SIZE 	:= 	4096

# Process flags
$(eval $(call add_define,CONSOLE_BASE))
$(eval $(call add_define,CRASH_CONSOLE_BASE))
$(eval $(call add_define,PLAT_PARTITION_BLOCK_SIZE))

ENABLE_PLAT_COMPAT	:=	0

USE_COHERENT_MEM	:=	1

PLAT_INCLUDES		:=	-Iinclude/common/tbbr			\
				-Iplat/hisilicon/common/include 	\
				-Iplat/hisilicon/hikey960/include

PLAT_BL_COMMON_SOURCES	:=	drivers/arm/pl011/pl011_console.S	\
				lib/aarch64/xlat_tables.c		\
				plat/common/aarch64/plat_common.c	\
				plat/hisilicon/hikey960/aarch64/hikey960_common.c

BL1_SOURCES		+=	bl1/tbbr/tbbr_img_desc.c		\
				drivers/arm/sp804/sp804_delay_timer.c	\
				drivers/delay_timer/delay_timer.c	\
				drivers/io/io_block.c			\
				drivers/io/io_fip.c			\
				drivers/io/io_storage.c			\
				drivers/ufs/ufs.c 			\
				lib/cpus/aarch64/cortex_a53.S		\
				plat/common/aarch64/platform_up_stack.S	\
				plat/hisilicon/hikey960/aarch64/hikey960_helpers.S \
				plat/hisilicon/hikey960/hikey960_bl1_setup.c 	\
				plat/hisilicon/hikey960/hikey960_io_storage.c

BL2_SOURCES		+=	drivers/arm/sp804/sp804_delay_timer.c 	\
				drivers/delay_timer/delay_timer.c	\
				drivers/io/io_block.c			\
				drivers/io/io_fip.c			\
				drivers/io/io_storage.c			\
				drivers/ufs/ufs.c			\
				plat/common/aarch64/platform_up_stack.S	\
				plat/hisilicon/hikey960/hikey960_bl2_setup.c \
				plat/hisilicon/hikey960/hikey960_io_storage.c

BL2U_SOURCES		+=	drivers/arm/sp804/sp804_delay_timer.c	\
				drivers/delay_timer/delay_timer.c	\
				drivers/io/io_block.c			\
				drivers/io/io_fip.c			\
				drivers/io/io_storage.c			\
				drivers/partition/gpt.c 		\
				drivers/partition/partition.c 		\
				drivers/synopsys/ufs/dw_ufs.c 		\
				drivers/ufs/ufs.c 			\
				plat/common/aarch64/platform_up_stack.S	\
				plat/hisilicon/common/mailbox/mailbox.c \
				plat/hisilicon/hikey960/aarch64/hikey960_helpers.S \
				plat/hisilicon/hikey960/hikey960_bl2u_setup.c \
				plat/hisilicon/hikey960/hikey960_io_storage.c \
				plat/hisilicon/hikey960/hi3660_mailbox.c