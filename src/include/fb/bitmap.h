#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <configs.h>
#include <default.h>

/*
 * defined blit formats
 */
enum bitmap_format
{
	BITMAP_FORMAT_RGBA_GENERIC,
	BITMAP_FORMAT_RGBA_8888,
	BITMAP_FORMAT_BGRA_8888,

	BITMAP_FORMAT_RGB_GENERIC,
	BITMAP_FORMAT_RGB_888,
	BITMAP_FORMAT_BGR_888,
	BITMAP_FORMAT_RGB_565,
	BITMAP_FORMAT_BGR_565,
};

struct bitmap_info
{
	/* width of the bitmap */
	x_u32 width;

	/* height of the bitmap */
	x_u32 height;

	/* bits per pixel */
	x_u32 bpp;

	/* bytes per pixel */
	x_u32 bytes_per_pixel;

	/* pitch of one scan line, how many bytes there are for scan line */
	x_u32 pitch;

	/* how many bits are reserved for red color */
	x_u32 red_mask_size;

	/* what is location of red color bits */
	x_u32 red_field_pos;

	/* how many bits are reserved for green color */
	x_u32 green_mask_size;

	/* what is location of green color bits */
	x_u32 green_field_pos;

	/* how many bits are reserved for blue color */
	x_u32 blue_mask_size;

	/* what is location of blue color bits */
	x_u32 blue_field_pos;

	/* how many bits are alpha in color */
	x_u32 alpha_mask_size;

	/* what is location of alpha color bits */
	x_u32 alpha_field_pos;

	/* optimization hint how binary data is coded */
	enum bitmap_format fmt;
};

struct bitmap
{
	/* bitmap information description */
	struct bitmap_info info;

	/* bitmap's view port */
	struct {
		x_u32 x;
		x_u32 y;
		x_u32 w;
		x_u32 h;
	} viewport;

	/* pointer to bitmap data formatted according to info */
	void * data;
};

/*
 * the struct of bitmap reader
 */
struct bitmap_reader
{
	/* file extension for this bitmap type with dot */
	const char * extension;

	/* load bitmap function */
	x_bool (*load)(struct bitmap ** bitmap, const char * filename);
};

/*
 * the list of bitmap reader
 */
struct bitmap_reader_list
{
	struct bitmap_reader * reader;
	struct list_head entry;
};


x_bool register_bitmap_reader(struct bitmap_reader * reader);
x_bool unregister_bitmap_reader(struct bitmap_reader * reader);

enum bitmap_format get_bitmap_format(struct bitmap_info * info);
x_bool bitmap_create(struct bitmap ** bitmap, x_u32 width, x_u32 height, enum bitmap_format fmt);
x_bool bitmap_load(struct bitmap ** bitmap, const char * filename);
x_bool bitmap_destroy(struct bitmap * bitmap);

#endif /* __BITMAP_H__ */