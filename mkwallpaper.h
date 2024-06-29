#ifndef __MKWALLPAPER_H
#define __MKWALLPAPER_H

#define PROG "mkwallpaper"
#define THIS_VERSION "0.1.8"

struct { 
	/* allows an icon */
	cairo_surface_t *image;
	/* allows a background - png only */
	cairo_surface_t *background;
}glob;

void usage(void);
static const char *get_user_out_file(char *destination);
static void paint_img (const char *label,
						const char *name,
						const char *font,
						int pflag,
						int wdth,
						int hght,
						const char *fp_color,
						int f_size,
						double offset,
						int angle,
						int kfont,
						int gradl,
						int gradr,
						char *qposi,
						char *jposi,
						int fontcol,
						char *dest,
						char *style,
						char *eicon,
						char *wall,
						int trans,
						int centred,
						const char *sec_color,
						int effect) {

#endif /*__MKWALLPAPER_H*/
