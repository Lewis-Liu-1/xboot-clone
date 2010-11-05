/*
 * drivers/console/curses.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <ctype.h>
#include <string.h>
#include <xboot/scank.h>
#include <xboot/printk.h>
#include <console/console.h>
#include <console/curses.h>

x_bool console_getwh(struct console * console, x_s32 * w, x_s32 * h)
{
	if(console && console->getwh)
		return console->getwh(console, w, h);
	return FALSE;
}

x_bool console_getxy(struct console * console, x_s32 * x, x_s32 * y)
{
	if(console && console->getxy)
		return console->getxy(console, x, y);
	return FALSE;
}

x_bool console_gotoxy(struct console * console, x_s32 x, x_s32 y)
{
	if(console && console->gotoxy)
		return console->gotoxy(console, x, y);
	return FALSE;
}

x_bool console_setcursor(struct console * console, x_bool on)
{
	if(console && console->setcursor)
		return console->setcursor(console, on);
	return FALSE;
}

x_bool console_setcolor(struct console * console, enum console_color f, enum console_color b)
{
	if(console && console->setcolor)
		return console->setcolor(console, f, b);
	return FALSE;
}

x_bool console_getcolor(struct console * console, enum console_color * f, enum console_color * b)
{
	if(console && console->getcolor)
		return console->getcolor(console, f, b);
	return FALSE;
}

x_bool console_cls(struct console * console)
{
	if(console && console->cls)
		return console->cls(console);
	return FALSE;
}

x_bool console_refresh(struct console * console)
{
	if(console && console->refresh)
		return console->refresh(console);
	return FALSE;
}

x_bool console_getchar(struct console * console, x_u32 * c)
{
	if(console && console->getchar)
		return console->getchar(console, c);
	return FALSE;
}

x_bool console_putchar(struct console * console, x_u32 c)
{
	if(console && console->putchar)
		return console->putchar(console, c);
	return FALSE;
}

x_bool console_draw_hline(struct console * console, x_u32 x0, x_u32 y0, x_u32 x)
{
	x_s32 width, height;
	x_s32 i, l;

	if(console && console->putchar)
	{
		if(!console_getwh(console, &width, &height))
			return FALSE;

		if(x0 >= width || y0 >= height)
			return FALSE;

		if(x0 + x >= width)
			l = width - x0;
		else
			l = x;

		if(!console_gotoxy(console, x0, y0))
			return FALSE;

		for(i = 0; i < l; i++)
		{
			console->putchar(console, UNICODE_HLINE);
		}

		return TRUE;
	}

	return FALSE;
}

x_bool console_draw_vline(struct console * console, x_u32 x0, x_u32 y0, x_u32 y)
{
	x_s32 width, height;
	x_s32 i, l;

	if(console && console->putchar)
	{
		if(!console_getwh(console, &width, &height))
			return FALSE;

		if(x0 >= width || y0 >= height)
			return FALSE;

		if(y0 + y >= height)
			l = width - y0;
		else
			l = y;

		for(i = 0; i < l; i++)
		{
			console->gotoxy(console, x0, y0 + i);
			console->putchar(console, UNICODE_VLINE);
		}
		return TRUE;
	}
	return FALSE;
}

x_bool console_draw_rect(struct console * console, x_u32 x0, x_u32 y0, x_u32 x1, x_u32 y1)
{
	x_s32 width, height;
	x_u32 x_min, y_min, x_max, y_max;

	if(!console_getwh(console, &width, &height))
		return FALSE;

	x_min = MIN(x0, x1);
	x_max = MAX(x0, x1);

	y_min = MIN(y0, y1);
	y_max = MAX(y0, y1);

	if(x_min < width && y_min < height)
	{
		console->gotoxy(console, x_min, y_min);
		console->putchar(console, UNICODE_LEFTUP);
		console_draw_hline(console, x_min+1, y_min, x_max - x_min - 1);
		console_draw_vline(console, x_min, y_min+1, y_max - y_min - 1);

		if(x_max < width)
		{
			console->gotoxy(console, x_max, y_min);
			console->putchar(console, UNICODE_RIGHTUP);
			console_draw_vline(console, x_max, y_min+1, y_max - y_min - 1);
		}

		if(y_max < height)
		{
			console->gotoxy(console, x_min, y_max);
			console->putchar(console, UNICODE_LEFTDOWN);
			console_draw_hline(console, x_min+1, y_max, x_max - x_min - 1);
		}

		if(x_max < width && y_max < height)
		{
			console->gotoxy(console, x_max, y_max);
			console->putchar(console, UNICODE_RIGHTDOWN);
		}

		return TRUE;
	}
	return FALSE;
}