/*
 * SDL_SavePNG -- libpng-based SDL_Surface writer.
 *
 * This code is free software, available under zlib/libpng license.
 * http://www.libpng.org/pub/png/src/libpng-LICENSE.txt
 */

#ifndef SDLVER
#define SDLVER 1
#endif

#if SDLVER == 1
#include <SDL/SDL.h>
#define SDL23(x,y) x
#endif
#if SDLVER == 2
#include <SDL2/SDL.h>
#define SDL23(x,y) x
#endif
#if SDLVER == 3
#include <SDL3/SDL.h>
#define SDL23(x,y) y
#endif

#include <cstdlib>
#include <png.h>

#define SUCCESS 0
#define ERROR -1

#define USE_ROW_POINTERS

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define rmask 0xFF000000
#define gmask 0x00FF0000
#define bmask 0x0000FF00
#define amask 0x000000FF
#else
#define rmask 0x000000FF
#define gmask 0x0000FF00
#define bmask 0x00FF0000
#define amask 0xFF000000
#endif

#if SDLVER >= 3
struct pixformat { int BitsPerPixel; int BytesPerPixel; Uint32 Rmask, Gmask, Bmask, Amask; };

pixformat get_format(SDL_PixelFormat val) { pixformat pf; SDL_GetMasksForPixelFormat(val, &pf.BitsPerPixel, &pf.Rmask, &pf.Gmask, &pf.Bmask, &pf.Amask); pf.BytesPerPixel = (pf.BitsPerPixel+7) / 8; return pf; }

#else

SDL_PixelFormat get_format(SDL_PixelFormat *val) { return *val; }


#endif

/* libpng callbacks */ 
static void png_error_SDL(png_structp ctx, png_const_charp str)
{
	SDL_SetError("libpng: %s\n", str);
}
static void png_write_SDL(png_structp png_ptr, png_bytep data, png_size_t length)
{
	#if SDLVER >= 3
        SDL_IOStream *rw = (SDL_IOStream*)png_get_io_ptr(png_ptr);
	SDL_WriteIO(rw, data, length);
	#else
        SDL_RWops *rw = (SDL_RWops*)png_get_io_ptr(png_ptr);
	SDL_RWwrite(rw, data, sizeof(png_byte), length);
	#endif
}

#ifdef __cplusplus
extern "C"
#endif
SDL_Surface *SDL_PNGFormatAlpha(SDL_Surface *src) 
{
	SDL_Surface *surf;
	SDL_Rect rect = { 0, 0, 0, 0 };

	auto pf = get_format(src->format);

	/* NO-OP for images < 32bpp and 32bpp images that already have Alpha channel */ 
	if (pf.BitsPerPixel <= 24 || pf.Amask) {
		src->refcount++;
		return src;
	}

	/* Convert 32bpp alpha-less image to 24bpp alpha-less image */
	rect.w = src->w;
	rect.h = src->h;
	#if SDLVER >= 3
	surf = SDL_CreateSurface(src->w, src->h, SDL_GetPixelFormatForMasks(24, pf.Rmask, pf.Gmask, pf.Bmask, 0));
	SDL_BlitSurfaceUnchecked(src, &rect, surf, &rect);
	#else
	surf = SDL_CreateRGBSurface(src->flags, src->w, src->h, 24, pf.Rmask, pf.Gmask, pf.Bmask, 0);
	SDL_LowerBlit(src, &rect, surf, &rect);
	#endif

	return surf;
}

#ifdef __cplusplus
extern "C"

#define ERR SDL23(if (freedst) SDL_RWclose(dst); return (ERROR), if (freedst) SDL_CloseIO(dst); return (ERROR))

#endif
int SDL_SavePNG_RW(SDL_Surface *surface, SDL23(SDL_RWops, SDL_IOStream) *dst, int freedst)
{
	png_structp png_ptr;
	png_infop info_ptr;
	#if SDLVER <= 2
	png_colorp pal_ptr;
	SDL_Palette *pal;
	#endif
	int i, colortype;
#ifdef USE_ROW_POINTERS
	png_bytep *row_pointers;
#endif
	/* Initialize and do basic error checking */
	if (!dst)
	{
		SDL_SetError("Argument 2 to SDL_SavePNG_RW can't be NULL, expecting SDL_RWops*\n");
		return (ERROR);
	}
	if (!surface)
	{
		SDL_SetError("Argument 1 to SDL_SavePNG_RW can't be NULL, expecting SDL_Surface*\n");
		ERR;
	}
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_error_SDL, NULL); /* err_ptr, err_fn, warn_fn */
	if (!png_ptr) 
	{
		SDL_SetError("Unable to png_create_write_struct on %s\n", PNG_LIBPNG_VER_STRING);
		ERR;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		SDL_SetError("Unable to png_create_info_struct\n");
		png_destroy_write_struct(&png_ptr, NULL);
		ERR;
	}
	if (setjmp(png_jmpbuf(png_ptr)))	/* All other errors, see also "png_error_SDL" */
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		ERR;
	}

	/* Setup our RWops writer */
	png_set_write_fn(png_ptr, dst, png_write_SDL, NULL); /* w_ptr, write_fn, flush_fn */

	/* Prepare chunks */
	colortype = PNG_COLOR_MASK_COLOR;
	auto pf = get_format(surface->format);

	#if SDLVER <= 2
        if (pf.BytesPerPixel > 0 && pf.BytesPerPixel <= 8 && (pal = surface->format->palette))
	{
		colortype |= PNG_COLOR_MASK_PALETTE;
		pal_ptr = (png_colorp)malloc(pal->ncolors * sizeof(png_color));
		for (i = 0; i < pal->ncolors; i++) {
			pal_ptr[i].red   = pal->colors[i].r;
			pal_ptr[i].green = pal->colors[i].g;
			pal_ptr[i].blue  = pal->colors[i].b;
		}
		png_set_PLTE(png_ptr, info_ptr, pal_ptr, pal->ncolors);
		free(pal_ptr);
	}
	#else
	if(false) ;
	#endif
	else if (pf.BytesPerPixel > 3 || pf.Amask)
		colortype |= PNG_COLOR_MASK_ALPHA;

	png_set_IHDR(png_ptr, info_ptr, surface->w, surface->h, 8, colortype,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

//	png_set_packing(png_ptr);

	/* Allow BGR surfaces */
	if (pf.Rmask == bmask
	&& pf.Gmask == gmask
	&& pf.Bmask == rmask)
		png_set_bgr(png_ptr);

	/* Write everything */
	png_write_info(png_ptr, info_ptr);
#ifdef USE_ROW_POINTERS
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep)*surface->h);
	for (i = 0; i < surface->h; i++)
		row_pointers[i] = (png_bytep)(Uint8*)surface->pixels + i * surface->pitch;
	png_write_image(png_ptr, row_pointers);
	free(row_pointers);
#else
	for (i = 0; i < surface->h; i++)
		png_write_row(png_ptr, (png_bytep)(Uint8*)surface->pixels + i * surface->pitch);
#endif
	png_write_end(png_ptr, info_ptr);

	/* Done */
	png_destroy_write_struct(&png_ptr, &info_ptr);
	if (freedst) SDL23(SDL_RWclose, SDL_CloseIO)(dst);
	return (SUCCESS);
}
