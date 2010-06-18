/*
 * kernel/fs/fatfs/fatfs.c
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
#include <types.h>
#include <stdarg.h>
#include <vsprintf.h>
#include <malloc.h>
#include <error.h>
#include <bitpos.h>
#include <time/xtime.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/scank.h>
#include <xboot/chrdev.h>
#include <xboot/blkdev.h>
#include <xboot/device.h>
#include <fs/vfs/vfs.h>
#include <fs/fs.h>

/*
 * boot sector
 */
struct fat_boot_sector {
	/*
	 * jump instruction and oem name
	 */
	x_u8	jmp_instruction[3];
	x_u8	oem_name[8];

	/*
	 * the common part of the fat12, fat16 and fat32 bios parameter block
	 */
	x_u8	bytes_per_sector[2];
	x_u8	sectors_per_cluster;
	x_u8	reserved_sectors[2];
	x_u8	num_of_fats;
	x_u8	root_entries[2];
	x_u8	total_sectors[2];
	x_u8	media_descriptor;
	x_u8	sectors_per_fat[2];
	x_u8	sectors_per_track[2];
	x_u8	heads[2];
	x_u8	hidden_sectors[2];

	/*
	 *
	 */
	union {
		x_u8 bootstrap[480];

	} code;

	/*
	 * the signature 0x55, 0xaa
	 */
	x_u8	signature[2];
} __attribute__ ((packed));

/*
 * fat directory entry
 */
struct fat_dirent {
	x_u8	name[11];
	x_u8	attr;
	x_u8	reserve[10];
	x_u8	time[2];
	x_u8	date[2];
	x_u8	cluster[2];
	x_u8	size[4];
} __attribute__ ((packed));

/*
 * fat filesystem type
 */
enum fat_type {
	FAT_TYPE_FAT12,
	FAT_TYPE_FAT16,
	FAT_TYPE_FAT32
};

/*
 * fatfs mount data
 */
struct fatfs_mount_data {
	/* fat type */
	enum fat_type type;

	/* start sector for root directory */
	x_u32 root_start;

	/* start sector for fat entries */
	x_u32 fat_start;

	/* start sector for data */
	x_u32 data_start;

	/* id of end cluster */
	x_u32 fat_eof;

	/* sectors per cluster */
	x_u32 sec_per_cl;

	/* cluster size */
	x_u32 cluster_size;

	/* last cluser */
	x_u32 last_cluster;

	/* mask for cluster */
	x_u32 fat_mask;

	/* start cluster to free search */
	x_u32 free_scan;

	/* vnode for root */
	struct vnode * root_vnode;

	/* local data buffer */
	char * io_buf;

	/* buffer for fat entry */
	char * fat_buf;

	/* buffer for directory entry */
	char * dir_buf;

	/* mounted block device */
	struct blkdev * blk;
};











/*
 * filesystem operations
 */
static x_s32 fatfs_mount(struct mount * m, char * dev, x_s32 flag)
{
	struct blkdev * blk;
	struct fat_boot_sector fbs;
	x_u32 sector_size;
	x_u32 tmp;

	if(dev == NULL)
		return EINVAL;

	blk = (struct blkdev *)m->m_dev;
	if(!blk || !blk->info)
		return ENODEV;

	if(get_blkdev_total_size(blk) <= sizeof(struct fat_boot_sector))
		return EINTR;

	if(bio_read(blk, (x_u8 *)(&fbs), 0, sizeof(struct fat_boot_sector)) != sizeof(struct fat_boot_sector))
		return EIO;

	/*
	 * check both signature (0x55, 0xaa)
	 */
	if((fbs.signature[0] != 0x55) || fbs.signature[1] != 0xaa)
		return EINVAL;

	/* the number of fats (byte 16) is nonzero */
	if(fbs.num_of_fats == 0x00)
		return EINVAL;

	/* the cluster size (byte 13) is a power of two */
	if(! is_power_of_2(fbs.sectors_per_cluster))
		return EINVAL;

	/* the logical sector size (bytes 11-12) is a power of two, at least 512 */
	sector_size = (fbs.bytes_per_sector[1] << 8) | fbs.bytes_per_sector[0];
	if( (sector_size < 512) || (!is_power_of_2(sector_size)) )
		return EINVAL;

	/* the number of root directory entries (bytes 17-18) must be sector aligned */
	tmp = (fbs.root_entries[1] << 8) | fbs.root_entries[0];
	if(tmp % (sector_size / sizeof(struct fat_dirent)) != 0)
		return EINVAL;

	/* the number of reserved sectors (bytes 14-15) is nonzero */
	tmp = (fbs.reserved_sectors[1] << 8) | fbs.reserved_sectors[0];
	if(tmp == 0)
		return EINVAL;

	return 0;
}

static x_s32 fatfs_unmount(struct mount * m)
{
	return 0;
}

static x_s32 fatfs_sync(struct mount * m)
{
	return 0;
}

static x_s32 fatfs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static x_s32 fatfs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static x_s32 fatfs_open(struct vnode * node, x_s32 flag)
{
	return -1;
}

static x_s32 fatfs_close(struct vnode * node, struct file * fp)
{
	return -1;
}

static x_s32 fatfs_read(struct vnode * node, struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 fatfs_write(struct vnode * node , struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 fatfs_seek(struct vnode * node, struct file * fp, x_off off1, x_off off2)
{
	return -1;
}

static x_s32 fatfs_ioctl(struct vnode * node, struct file * fp, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 fatfs_fsync(struct vnode * node, struct file * fp)
{
	return -1;
}

static x_s32 fatfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	return -1;
}

static x_s32 fatfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
	return -1;
}

static x_s32 fatfs_create(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 fatfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 fatfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static x_s32 fatfs_mkdir(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 fatfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 fatfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 fatfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 fatfs_inactive(struct vnode * node)
{
	return -1;
}

static x_s32 fatfs_truncate(struct vnode * node, x_off length)
{
	return -1;
}

/*
 * fatfs vnode operations
 */
static struct vnops fatfs_vnops = {
	.vop_open 		= fatfs_open,
	.vop_close		= fatfs_close,
	.vop_read		= fatfs_read,
	.vop_write		= fatfs_write,
	.vop_seek		= fatfs_seek,
	.vop_ioctl		= fatfs_ioctl,
	.vop_fsync		= fatfs_fsync,
	.vop_readdir	= fatfs_readdir,
	.vop_lookup		= fatfs_lookup,
	.vop_create		= fatfs_create,
	.vop_remove		= fatfs_remove,
	.vop_rename		= fatfs_rename,
	.vop_mkdir		= fatfs_mkdir,
	.vop_rmdir		= fatfs_rmdir,
	.vop_getattr	= fatfs_getattr,
	.vop_setattr	= fatfs_setattr,
	.vop_inactive	= fatfs_inactive,
	.vop_truncate	= fatfs_truncate,
};

/*
 * file system operations
 */
static struct vfsops fatfs_vfsops = {
	.vfs_mount		= fatfs_mount,
	.vfs_unmount	= fatfs_unmount,
	.vfs_sync		= fatfs_sync,
	.vfs_vget		= fatfs_vget,
	.vfs_statfs		= fatfs_statfs,
	.vfs_vnops		= &fatfs_vnops,
};

/*
 * fatfs filesystem
 */
static struct filesystem fatfs = {
	.name		= "fatfs",
	.vfsops		= &fatfs_vfsops,
};

static __init void filesystem_fatfs_init(void)
{
	if(!filesystem_register(&fatfs))
		LOG_E("register 'fatfs' filesystem fail");
}

static __exit void filesystem_fatfs_exit(void)
{
	if(!filesystem_unregister(&fatfs))
		LOG_E("unregister 'fatfs' filesystem fail");
}

module_init(filesystem_fatfs_init, LEVEL_POSTCORE);
module_exit(filesystem_fatfs_exit, LEVEL_POSTCORE);