/*
 * arch/arm/mach-realview/realview-mmc.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include <configs.h>
#include <default.h>
#include <macros.h>
#include <types.h>
#include <div64.h>
#include <xboot/log.h>
#include <xboot/io.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <time/delay.h>
#include <mmc/mmc.h>
#include <mmc/mmc_host.h>
#include <realview/reg-mmc.h>

static x_bool mmc_send_cmd(x_u32 cmd, x_u32 arg, x_u32 * resp)
{
	x_u32 status;

	if(readl(REALVIEW_MCI_COMMAND) & REALVIEW_MCI_CMD_ENABLE)
	{
		writel(REALVIEW_MCI_CMD_ENABLE, 0);
		udelay(1);
	}

	cmd |= REALVIEW_MCI_CMD_ENABLE | (0x1<<7);

	writel(REALVIEW_MCI_ARGUMENT, arg);
	writel(REALVIEW_MCI_COMMAND, cmd);

	status = readl(REALVIEW_MCI_STATUS);
	//status &= readl(REALVIEW_MCI_MASK0);

	LOG_I("status = 0x%x, repocmd = 0x%x", status,readl(REALVIEW_MCI_RESPCMD));

	if(resp)
	{
		resp[0] = readl(REALVIEW_MCI_RESP0);
		resp[1] = readl(REALVIEW_MCI_RESP1);
		resp[2] = readl(REALVIEW_MCI_RESP2);
		resp[3] = readl(REALVIEW_MCI_RESP3);
	}

	return TRUE;
}

static void realview_mmc_init(void)
{
	/* power on mode */
	writel(REALVIEW_MCI_POWER, 0x3);
}

static void realview_mmc_exit(void)
{

}

x_bool realview_mmc_probe(struct mmc_card_info * info)
{
	x_u32 resp[4];
/*
	mmc_send_cmd(MMC_GO_IDLE_STATE, 0, NULL);

	mmc_send_cmd(41, 0, NULL);

	mmc_send_cmd(MMC_SEND_CID, 0, resp);

	LOG_I("0x%x,0x%x,0x%x,0x%x", resp[0],resp[1],resp[2],resp[3]);
*/
	return TRUE;
}

x_bool realview_mmc_read_sector(struct mmc_card * card, x_u32 sector, x_u8 * data)
{
	return FALSE;
}

x_bool realview_mmc_write_sector(struct mmc_card * nand, x_u32 sector, x_u8 * data)
{
	return FALSE;
}

static struct mmc_host realview_mmc_host_controller = {
	.name			= "realview-mmc",
	.init			= realview_mmc_init,
	.exit			= realview_mmc_exit,
	.probe			= realview_mmc_probe,
	.read_sector	= realview_mmc_read_sector,
	.write_sector	= realview_mmc_write_sector,
};

static __init void realview_mmc_host_controller_init(void)
{
	if(!register_mmc_host(&realview_mmc_host_controller))
		LOG_E("failed to register mmc host controller '%s'", realview_mmc_host_controller.name);
}

static __exit void realview_mmc_host_controller_exit(void)
{
	if(!unregister_mmc_host(&realview_mmc_host_controller))
		LOG_E("failed to unregister mmc host controller '%s'", realview_mmc_host_controller.name);
}

module_init(realview_mmc_host_controller_init, LEVEL_MACH_RES);
module_exit(realview_mmc_host_controller_exit, LEVEL_MACH_RES);
