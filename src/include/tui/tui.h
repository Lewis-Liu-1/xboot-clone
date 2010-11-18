#ifndef __TUI_H__
#define __TUI_H__

#include <configs.h>
#include <default.h>
#include <console/console.h>

/*
 * forward declare
 */
struct tui_widget;

struct tui_widget_ops
{
	/* set bounds */
	x_bool (*setbounds)(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h);

	/* get bounds */
	x_bool (*getbounds)(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h);

	/* get minimal width and height */
	x_bool (*minsize)(struct tui_widget * widget, x_s32 * w, x_s32 * h);

	/* get a valid usable region */
	x_bool (*region)(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h);

	/* set widget property */
	x_bool (*setproperty)(struct tui_widget * widget, const x_s8 * name, const x_s8 * value);

	/* paint widget with area */
	x_bool (*paint)(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h);

	/* destory widget */
	x_bool (*destroy)(struct tui_widget * widget);
};

struct tui_widget
{
	/* widget's id */
	x_s8 * id;

	/* widget's bounds */
	x_s32 x, y;
	x_s32 w, h;

	/* bind console */
	struct console * console;

	/* container layout */
	struct tui_layout * layout;

	/* widget operations */
	struct tui_widget_ops * ops;

	/* the widget entry */
	struct list_head entry;

	/* the parent widget */
	struct tui_widget * parent;

	/* child widget list head */
	struct list_head child;

	/* priv data */
	void * priv;
};


struct tui_widget * find_tui_widget_by_id(struct tui_widget * widget, const x_s8 * id);
x_bool tui_widget_setparent(struct tui_widget * widget, struct tui_widget * parent);
struct tui_widget * tui_widget_getparent(struct tui_widget * widget);
x_bool tui_widget_addchild(struct tui_widget * widget, struct tui_widget * child);
x_bool tui_widget_removechild(struct tui_widget * widget, struct tui_widget * child);
x_bool tui_widget_setbounds(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h);
x_bool tui_widget_getbounds(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h);
x_bool tui_widget_minsize(struct tui_widget * widget, x_s32 * w, x_s32 * h);
x_bool tui_widget_region(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h);
x_bool tui_widget_setproperty(struct tui_widget * widget, const x_s8 * name, const x_s8 * value);
x_bool tui_widget_paint(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h);
x_bool tui_widget_destroy(struct tui_widget * widget);

#endif /* __TUI_H__ */