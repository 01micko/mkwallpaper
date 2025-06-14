/*
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * (c)2015 - 2024 Michael Amadio. Gold Coast QLD, Australia 01micko@gmx.com
 */
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>

#include <cairo-svg.h>
#include <pango/pangocairo.h>

#define PROG "mkwallpaper"
#define THIS_VERSION "0.19"

void usage(){
	printf("%s-%s\n\n", PROG , THIS_VERSION);
	printf("\tGenerate SVG or PNG Linux wallpapers. SVG is default.\n\n");
	printf("Usage :\n");
	printf("%s [-l,-n,-f,-p,-s,-x,-y,-r,-d,-c,-j,-k,-g,-z,-o,-a,-b,-h,-e,-w]\n", PROG);
	printf("\t-n [string] image file name\n");
	printf("\t-l [string] label for an image, up to 36 chars, quoted\t(optional)\n");
	printf("\t-f [string] a TTF font family; quoted if it has spaces\n");
	printf("\t-z [\"float float float\"] floating point RGB, quoted,\n"
					"\tspace delimited values for colour\n"
					"\teg: -z \"0.1 0.2 0.3\"\n");
	printf("\t-i [\"float float float\"] floating point RGB, quoted,\n"
					"\tspace delimited values for colour\n"
					"\tthis is for a second colour\n"
					"\teg: -i \"0.1 0.2 0.3\"\n"
					"\t must be used with \"-z\"");
	printf("\tOR [\"float float float float\"] with the last arg for transparency\n"
			"\tApplies to either \"-z\" or \"-i\"\n");
	printf("\t-p save file as \"png\". \"svg\" format is the default\n");
	printf("\t-x [int] width of image in pixels\n");
	printf("\t-y [int] height of image in pixels\n");
	printf("\t-s [int] font size in pixels\n");
	printf("\t-k add embossed effect on font\n");
	printf("\t-g turn on background linear gradient (default is off)\n");
	printf("\t-r turn on background radial gradient (default is off)\n");
	printf("\t-t add extra linear gradient effect or offset centre of radial gradient.(default is off)\n");
	printf("\t-q [x y] : radial gradient position in px; requires \"-r\"\n");
	printf("\t-e [string] : '/path/to/icon.png x y' - embed a png image at position\n\t\"x y\"(optional) or use \"-c\" option to centre\n");
	printf("\t-w [string] : '/path/to/background.png' - embed a png wall at pos 0,0\n\t(optional). Requires");
	printf(" original dimensions of incoming png image for\n\toptions '-x' and '-y'; (overrides \"-z\")\n");
	printf("\t-c : centre font and/or icon on image\n");
	printf("\t-j [x y] : font position in px; overrides \"-c\"\n");
	printf("\t-o [float] offset: floating point value from 0.0 to 1.0\n"
								"\tfor the gradient offset\n");
	printf("\t-a [int] angle: integer value from 0 to 20 for the linear gradient angle\n"
			"\tit also applies the radius in regard to radial gradients\n");
	printf("\t-d [/path/to/directory] destination directory: (default: $HOME)\n");
	printf("\t-b [string] font-style: accepted values are \"n\" (normal) [the default],\n"
					"\t\"b\" (bold), \"i\" (italic), \"o\" (bold-italic).\n");
	printf("\t-u use transparent background (overrides \"-z\"; not good for wallpapers)\n");
	printf("\t-h : show this help and exit.\n");
	printf("\t-v : show version and exit.\n");
	exit (EXIT_SUCCESS);
}

struct { 
	/* allows an icon */
	cairo_surface_t *image;
	/* allows a background - png only */
	cairo_surface_t *background;
}glob;

static const char *get_user_out_file(char *destination){
	static char out_file[PATH_MAX];
	if (destination != NULL) {
		snprintf(out_file, sizeof(out_file), "%s", destination);
	} else {
		fprintf(stderr, "Failed to recognise directory\n");
		exit (EXIT_FAILURE);
	}
	mkdir(out_file, 0755);
	if (access(out_file, W_OK) == -1) {
		fprintf(stderr, "Failed to access directory %s\n", out_file);
		exit (EXIT_FAILURE);
	}
	return out_file;
}

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
	
	char icon[PATH_MAX];
	char icon_pre[PATH_MAX];
	char posx[8];
	char posy[8];
	int icon_x, icon_y;
	/* icon */
	if (eicon != NULL) {
		int icon_res;
		if (centred  == 0) {	
			icon_res = sscanf(eicon, "%s %s %s", icon_pre, posx, posy);
			if (icon_res < 3) {
				fprintf(stderr,"ERROR: path, x and y positions are required or use \"-c\" option\n");
				exit (EXIT_FAILURE);
			}
		} else {
			icon_res = sscanf(eicon, "%s 0 0", icon_pre);
		}
		if (icon_res > 3) {
			fprintf(stderr,"ERROR: too many arguments\n");
			exit (EXIT_FAILURE);
		}
		snprintf(icon, sizeof(icon), "%s", icon_pre);
		if (access(icon, R_OK) == -1) {
			fprintf(stderr, "Failed to access icon %s\n", icon);
			exit (EXIT_FAILURE);
		}
			icon_x = atoi(posx);
			icon_y = atoi(posy);
	}

	/* background */
	if (wall != NULL) {
		if (access(wall, R_OK) == -1) {
			fprintf(stderr, "Failed to access image %s\n", wall);
			exit (EXIT_FAILURE);
		}
	}
	
	char destimg[PATH_MAX];
	
	/* error check angle/offset */
	if ((angle < 1) || (angle > 21)) {
		fprintf(stderr, "%d is out of range. Must be 0 - 21 inclusive\n", angle);
		exit(EXIT_FAILURE);
	}
	if ((offset < 0) || (offset > 1)) {
		fprintf(stderr, "%f is out of range. Must be 0.00 - 1.00 inclusive\n", offset);
		exit(EXIT_FAILURE);
	}

	double r, g , b, a;
	char red[8];
	char green[8];
	char blue[8];
	char alpha[8] = "1.0"; /* default opaque */
	int len = strlen(fp_color);
	if (len > 32 ) {
		fprintf(stderr,"ERROR: colour argument too long\n");
		exit (EXIT_FAILURE);
	}
	int result = sscanf(fp_color, "%s %s %s %s", red, green, blue, alpha);
	if (result < 3) {
		fprintf(stderr,"ERROR: less than 3 colour aguments!\n");
		exit (EXIT_FAILURE);
	}
	r = atof(red);
	g = atof(green);
	b = atof(blue);
	a = atof(alpha);
	if ((r > 1.0) || (g > 1.0) || (b > 1.0) || (a > 1.0) ||
	   (r < 0.0) || (g < 0.0) || (b <0.0) || (a <0.0))  {
		fprintf(stderr, "Color values are out of range\n");
		exit (EXIT_FAILURE);
	}
	/* gradient colours: linear / radial */
	double r1, g1, b1, r2, g2, b2;
	if ((gradl == 0) || (gradr == 0)) {
		r1 = r; g1 = g; b1 = b;
		if (sec_color) {
			if (!fp_color) {
				fprintf(stderr, "\"-i\" must be used with \"-z\"\n");
				exit (EXIT_FAILURE);
			}
			int leni = strlen(sec_color);
			if (leni > 32 ) {
				fprintf(stderr,"ERROR: colour argument too long\n");
				exit (EXIT_FAILURE);
			}
			int resulti = sscanf(sec_color, "%s %s %s", red, green, blue);
			if (resulti < 3) {
				fprintf(stderr,"ERROR: less than 3 colour aguments!\n");
				exit (EXIT_FAILURE);
			}
			r2 = atof(red);
			g2 = atof(green);
			b2 = atof(blue);
		} else {		
			if ((r > 0.701) || (g > 0.701) || (b > 0.701)) {
				r2 = r + 0.3;
				g2 = g + 0.3;
				b2 = b + 0.3;
			} else {
				r2 = r - 0.3;
				g2 = g - 0.3;
				b2 = b - 0.3;		
			}
		}
	} else {
		r1 = r; g1 = g; b1 = b; r2 = r; g2 = g; b2 = b;
	}
	
	cairo_surface_t *cs;

	if (pflag == 0) {
		snprintf(destimg, sizeof(destimg), "%s/%s.svg", get_user_out_file(dest), name);
		cs = cairo_svg_surface_create(destimg, wdth, hght);
	} else {
		cs = cairo_image_surface_create
							(CAIRO_FORMAT_ARGB32, wdth, hght);
		snprintf(destimg, sizeof(destimg), "%s/%s.png", get_user_out_file(dest), name);
	}
	cairo_t *c;
	c = cairo_create(cs);
	cairo_pattern_t *linear, *radial;
	
	/* background and position */
	if (wall != NULL) {
		cairo_rectangle(c, 0.0, 0.0, wdth, hght);
		glob.background = cairo_image_surface_create_from_png(wall);
		cairo_set_source_surface(c, glob.background, 0, 0);
		cairo_paint(c);
	} else if (trans == 1) {
		cairo_set_source_rgba(c, 0, 0, 0, 0);
		cairo_rectangle(c, 0.0, 0.0, wdth, hght);
		cairo_fill(c);
	} else if (gradl == 0) {
		cairo_rectangle(c, 0.0, 0.0, wdth, hght);
		if (angle > 20) { /* vertical gradient */
			linear = cairo_pattern_create_linear(0, hght / 2, wdth , hght / 2);
		} else {
			linear = cairo_pattern_create_linear(angle * wdth / 20, 0, wdth / 2, hght);
		}
		cairo_pattern_add_color_stop_rgba(linear, 0.1, r1, g1, b1, a);
		cairo_pattern_add_color_stop_rgba(linear, offset, r2, g2, b2, a);
		if (effect == 1) {
			cairo_pattern_add_color_stop_rgba(linear, 0.9, r1, g1, b1, a);
		}
		cairo_set_source(c, linear);
		cairo_fill(c);
		cairo_pattern_destroy(linear);
	} else if (gradr == 0) {
		double qxposi0 = wdth / 2;
		double qyposi0 = hght / 2;
		double qxposi1 = qxposi0;
		double qyposi1 = qyposi0;
		double radius0 = wdth / angle;
		double radius1 = 0.0;
		if (qposi != NULL) {	
			char prqx[8];
			char prqy[8];
			int rad_pos = sscanf(qposi, "%s %s", prqx, prqy);
			if (rad_pos < 2) {
				fprintf(stderr,"ERROR: x and y positions are required\n");
				exit (EXIT_FAILURE);
			}
			if (rad_pos > 2) {
				fprintf(stderr,"ERROR: too many args\n");
				exit (EXIT_FAILURE);
			}
			qxposi0 = atof(prqx);
			qyposi0 = atof(prqy);
			qxposi1 = qxposi0;
			qyposi1 = qyposi0;
			if ((qxposi0 < 0) || (qyposi0 < 0) || (qxposi0 > wdth) || (qyposi0 > hght) ||
				(qxposi1 < 0) || (qyposi1 < 0) || (qxposi1 > wdth) || (qyposi1 > hght)) {
				fprintf(stderr,"ERROR: x or y args out of range\n");
				exit (EXIT_FAILURE);
			}
		}
		if (effect == 1) {
			qxposi1 = qxposi0 + (50 * angle);
			qyposi1 = qyposi0 + (50 * angle);
			radius1 = radius1 + angle;
		}
		cairo_rectangle(c, 0.0, 0.0, wdth, hght);
		radial = cairo_pattern_create_radial(qxposi0, qyposi0, radius0, qxposi1, qyposi1, radius1);
		cairo_pattern_add_color_stop_rgba(radial, 1.0, r1, g1, b1, a);
		cairo_pattern_add_color_stop_rgba(radial, 0.0, r2, g2, b2, a);
		cairo_set_source(c, radial);
		cairo_fill(c);
		cairo_pattern_destroy(radial);
	} else {
		cairo_set_source_rgb(c, r, g, b);
		cairo_rectangle(c, 0.0, 0.0, wdth, hght);
		cairo_fill(c);
	}
	/* icon and position */
	if (eicon != NULL) {
		glob.image = cairo_image_surface_create_from_png(icon);
		int ww = cairo_image_surface_get_width(glob.image);
		int hh = cairo_image_surface_get_height(glob.image);
		if (((centred == 1) && (!label)) || ((centred == 1) && (jposi))) {
			icon_x = (wdth / 2) - (ww / 2);
			icon_y = (hght / 2) - (hh / 2);
		} else if ((centred == 1) && (label)){
			icon_x = (wdth / 2) - (ww / 2);
			icon_y = (hght / 2) - hh;			
		}
		cairo_set_source_surface(c, glob.image, icon_x, icon_y);
		cairo_paint(c);
	}
	
	if (label) {
		int msg_len = strlen(label);
		if (msg_len > 36) {
			fprintf(stderr,"\"%s\" is too long!\n", label);
			exit (EXIT_FAILURE);
		}
		/* font color */
		double rf;
		if ((r > 0.5) || (g > 0.5) || (b > 0.5))
			rf = 0.1;
		else
			rf = 0.9;
		
		/* offset font for effect option */	
		double or, og, ob;
		int fc = 0;
		if (kfont == 1) {
			or = (r1 + r2) / 2;
			og = (g1 + g2) /2 ;
			ob = (b1 + b2) / 2;
			fc = 1;
		}
		/*font style*/
		int boldness; int styleness;
		if (strncmp(style, "b", 1) == 0) { boldness = PANGO_WEIGHT_BOLD; styleness = PANGO_STYLE_NORMAL;}
		else if (strncmp(style, "i", 1) == 0) { boldness = PANGO_WEIGHT_NORMAL; styleness = PANGO_STYLE_OBLIQUE;}
		else if (strncmp(style, "o", 1) == 0) { boldness = PANGO_WEIGHT_BOLD; styleness = PANGO_STYLE_OBLIQUE;}
		else { boldness = PANGO_WEIGHT_NORMAL; styleness = PANGO_STYLE_NORMAL;} /*catch garbage and default to 'normal'*/
		/* font */
		PangoLayout *layout;
		PangoFontDescription *font_description;
		
		font_description = pango_font_description_new ();
		pango_font_description_set_family (font_description, font);
		pango_font_description_set_style (font_description, styleness ); /*PANGO_STYLE_NORMAL = 0, PANGO_STYLE_OBLIQUE = 1*/
		pango_font_description_set_weight (font_description, boldness); /*PANGO_WEIGHT_NORMAL = 400, PANGO_WEIGHT_BOLD = 700*/
		pango_font_description_set_absolute_size (font_description, f_size * PANGO_SCALE);
		layout = pango_cairo_create_layout (c);
		pango_layout_set_font_description (layout, font_description);
		pango_layout_set_width (layout, 9 * wdth / 20 * PANGO_SCALE);
		pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
		pango_layout_set_text (layout, label, -1);
		

		/* position of text */
		int xposi, yposi;
		if (jposi != NULL) {	
			char prex[8];
			char prey[8];
			int font_pos = sscanf(jposi, "%s %s", prex, prey);
			if (font_pos < 2) {
				fprintf(stderr,"ERROR: x and y positions are required\n");
				exit (EXIT_FAILURE);
			}
			if (font_pos > 2) {
				fprintf(stderr,"ERROR: too many args\n");
				exit (EXIT_FAILURE);
			}
			xposi = atoi(prex);
			yposi = atoi(prey);
		} else if (centred == 1) {
			int wrect, hrect;
			PangoRectangle rect = { 0 };
			pango_layout_get_extents(layout, NULL, &rect);
			pango_extents_to_pixels(&rect, NULL);
			wrect = rect.width;
			wrect = ((wrect) < (wdth)) ? (wrect) : (wdth);
			hrect = rect.height;
			pango_layout_set_width(layout, wrect * PANGO_SCALE);
			if (!eicon) {
				xposi = (wdth / 2) - (wrect / 2);
				yposi = (hght / 2) - (hrect / 2);
			} else {
				xposi = (wdth / 2) - (wrect / 2);
				yposi = (hght / 2) + 10;
			}
		} else { /* fallback */
			xposi = wdth / 2;
			yposi = 3 * hght / 7;
		}
		 /* font effect */
		cairo_move_to(c, xposi , 1 * yposi);
		if (fontcol == 1)
			cairo_set_source_rgba(c, rf, rf, rf, 0.6);
		else
			cairo_set_source_rgba(c, rf, rf, rf, 0.55);
			
		pango_cairo_show_layout (c, layout);

		if (fc == 1) {
			double fz = (double)f_size;
			fz = f_size / 30; /* 30 default font size*/
			cairo_move_to(c, xposi - (0.75 * fz) , (1 * yposi) - (0.6 * fz));
			cairo_set_source_rgba(c, or, og, ob, 0.65);
			pango_cairo_show_layout (c, layout);
			g_object_unref (layout);
			pango_font_description_free (font_description);
		}
	}

	cairo_status_t res = cairo_surface_status(cs);
	const char *ret;
	ret = cairo_status_to_string (res);
	if (res != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy(cs);
		cairo_destroy(c);
		fprintf(stderr, "Cairo : %s\n", ret);
		exit (EXIT_FAILURE);
	}
	/* cleanup */
	if (pflag == 1) {
		cairo_surface_write_to_png (cs, destimg);
	}
	if (eicon != NULL)
		cairo_surface_destroy(glob.image);
	if (wall != NULL)
		cairo_surface_destroy(glob.background);
	cairo_surface_destroy(cs);
	cairo_destroy(c);
	fprintf(stdout, "image saved to %s\n", destimg);
}

int main(int argc, char **argv) {
	if (argc < 2) {
		usage();
		return 0;
	}
	int hflag = 0;
	int vflag = 0;
	char *lvalue = NULL; /* the cli string that appears in image */
	char *nvalue = "foo"; /* image name */
	char *fvalue = "Sans"; /* font */
	char *zvalue = ".1 .1 .1"; /* fp colour */
	char *ivalue = NULL; /* sec colour */
	char *evalue = NULL; /* embedded icon */
	char *wvalue = NULL; /* embedded background */
	char *jvalue = NULL;
	char *qvalue = NULL;
	int gvalue = 0;
	char *dvalue = getenv("HOME");
	double ovalue = 0.65;
	int avalue = 10;
	int width = 200; int height = 60;
	int font_size = 30;
	char *bvalue = "n";
	int uflag = 0; /* transparent bg */
	int cflag = 0; /* centred text */
	int pflag = 0; /* save as png */
	int kflag = 0; /* embossed effect */
	int grflag = 1; /* linear gradient */
	int rflag = 1; /* radial gradient */
	int tflag = 0; /* linear gradient effect */
	int c;
	while ((c = getopt (argc, argv, "l:n:f:x:y:d:z:o:a:q:i:j:s:b:e:w:ugrkpchvt")) != -1) {
		switch (c)
		{
			case 'l':
				lvalue = optarg;
				break;
			case 'n':
				nvalue = optarg;
				break;
			case 'f':
				fvalue = optarg;
				break;
			case 'x':
				width = atoi(optarg);
				break;
			case 'y':
				height = atoi(optarg);
				break;
			case 'z':
				zvalue = optarg;
				break;
			case 'i':
				ivalue = optarg;
				break;
			case 'o':
				ovalue = atof(optarg);
				break;
			case 'a':
				avalue = atoi(optarg);
				break;
			case 'd':
				dvalue = optarg;
				break;
			case 'p':
				pflag = 1;
				break;
			case 'k':
				kflag = 1;
				break;
			case 'j':
				jvalue = optarg;
				break;
			case 's':
				font_size = atoi(optarg);
				break;
			case 'b':
				bvalue = optarg;
				break;
			case 'e':
				evalue = optarg;
				break;
			case 'w':
				wvalue = optarg;
				gvalue = 1;
				break;
			case 'u':
				uflag = 1;
				break;
			case 'g':
				grflag = 0;
				break;
			case 'r':
				rflag = 0;
				break;
			case 't':
				tflag = 1;
				break;
			case 'q':
				qvalue = optarg;
				break;
			case 'c':
				cflag = 1;
				break;
			case 'h':
				hflag = 1;
				if (hflag == 1) usage();
				break;
			case 'v':
				vflag = 1;
				if (vflag == 1) fprintf(stdout, "%s\n", THIS_VERSION);
				return 0;
			default:
				usage();
		}
	}
	paint_img(lvalue, nvalue, fvalue, pflag,
					width, height, zvalue, font_size, ovalue, avalue,
					 kflag, grflag, rflag, qvalue, jvalue, gvalue, dvalue, bvalue, evalue, wvalue, uflag, cflag, ivalue, tflag);
	return 0;
} /* still can use m */

