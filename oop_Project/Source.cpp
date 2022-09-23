#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <math.h>
#include <algorithm>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
// Using truetype font
#include "SDL_ttf.h"
using namespace std;

const int WIDTH = 800;
const int HEIGHT = 575;
const int MAIN_HP = 100;
int MAP_LENGTH;

const int SOLID = 100;
const int SHADED = 101;
const int BLENDED = 102;

// Start Button constants
const int BUTTON_X = 480;
const int BUTTON_Y = 70;
const int BUTTON_WIDTH = 250;
const int BUTTON_HEIGHT = 75;

// frame num of mc
const int mc_FRAME = 4;

//stage1 setting
// frame number of enemy1
const int s1_e1_walk_f = 4;
const int s1_e1_hit_f = 3;

//stage demo setting
// frame number of enemy1
const int e1_walk_f = 4;
const int e1_hit_f = 6;

// frame number of enemy2
const int e2_walk_f = 4;

int mc_attack_i, e2_attack_i;

// Flip type
SDL_RendererFlip no = SDL_FLIP_NONE;
SDL_RendererFlip ho = SDL_FLIP_HORIZONTAL;
SDL_RendererFlip ve = SDL_FLIP_VERTICAL;
SDL_RendererFlip hove = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);

struct ImageData
{
	char path[100];
	SDL_Texture *texture;
	int width;
	int height;
};

struct TextData
{
	char path[100];
	SDL_Texture *texture;
	int width;
	int height;
};

enum MouseState
{
	NONE = 0,
	OUT = 1, // Mouse out
	IN_LB_SC = 2,  // Inside, Left Button, Single Click
	IN_RB_SC = 3,  // Inside, RIGHT Button, Single Click
	IN_LB_DC = 4,  // Inside, Left Button, Double Click
	IN_RB_DC = 5,  // Inside, RIGHT Button, Double Click
	IN_LB_PR = 6,  // Inside, Left Button, Press
	IN_RB_PR = 7,  // Inside, Left Button, Press
	IN_WU = 8,  // Inside, Wheel UP
	IN_WD = 9,  // Inside, Wheel DOWN
	HOVER = 10 // Mouse hover
};
struct Character
{
	int x_pos;
	int x_pos_d;
	int y_pos;
	int x_velo;
	int y_velo;
	int velocity;
	int health;
	int o_health;
	int damage;
	int way = 0;  //0 for right, 1 for left
	int action_flag[10] = { 0 }; // 0:stand, 1:run 2:a, 3:s 4:¡ô+s 5:damage
	int w, h;
};
Character main_ch;
Character *enemy_list;
int enemy_index;

// 0:up 1:down 2:left 3:right 4:a 5:s
int keyRecord[6] = { 0 };

int initSDL(); // Starts up SDL and creates window
void closeSDL(); // Frees media and shuts down SDL
bool loadAudio();
ImageData loadTexture(char *path, bool ckEnable, Uint8 r, Uint8 g, Uint8 b);
void imgRender(SDL_Renderer *renderer, ImageData img, int posX, int posY, int cx, int cy ,\
	double angle, SDL_RendererFlip flip);
void imgRender_size(SDL_Renderer *renderer, ImageData img, int posX, int posY, int wid, int hei,\
	int cx, int cy, double angle, SDL_RendererFlip flip);
void loadImageSet(ImageData *picture_array, char *path, int frame);
void bg_imgRender(SDL_Renderer *renderer, ImageData *img, int posX, int posY);
void show_map(SDL_Renderer *renderer, ImageData *map);
void mc_imgRender(SDL_Renderer *renderer, ImageData img, int posX, int posY, SDL_RendererFlip flip);
void e_imgRender(SDL_Renderer *renderer, ImageData img, Character e, SDL_RendererFlip flip, int w, int h);
void handleEvent(SDL_Event& e);
void enemy_move_p(Character *e_list, int e_index);
void mc_display_pos();
void mc_move();
void e_move1(Character &e);
void e_move2(Character &e, Character &mc);
void drawRectangleBound(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,\
	Sint16 bound, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void showstatus(SDL_Renderer * renderer, ImageData mc_pro, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, \
	Sint16 bound, Uint8 r, Uint8 g, Uint8 b, Uint8 a, Character ch);
int checkaction(Character ch, int n);
int checkcollision1(Character *enemies, int enemy_ind, SDL_RendererFlip flip);
int checkcollision2(Character *enemies, int i, SDL_RendererFlip flip);
void gotDamage(Character &attacker, Character &opponent);
void knockup(Character &ch, int hit_index, SDL_RendererFlip flip_flag);
//function for mc
Uint32  mc_action_stand(Uint32 interval, void *param);
Uint32  mc_action_walk(Uint32 interval, void *param);
Uint32  mc_action_run(Uint32 interval, void *param);
Uint32  mc_attack_A(Uint32 interval, void *param);
Uint32  mc_attack_S(Uint32 interval, void *param);
Uint32  mc_action_hit(Uint32 interval, void *param);

//function for e1
Uint32  e1_action_walk(Uint32 interval, void *param);
Uint32  e1_1_action_hit(Uint32 interval, void *param);
Uint32  e1_2_action_hit(Uint32 interval, void *param);
Uint32  e1_3_action_hit(Uint32 interval, void *param);
Uint32  e1_4_action_hit(Uint32 interval, void *param);
Uint32  e1_5_action_hit(Uint32 interval, void *param);
Uint32  e1_1_action_down(Uint32 interval, void *param);
Uint32  e1_2_action_down(Uint32 interval, void *param);
Uint32  e1_3_action_down(Uint32 interval, void *param);
Uint32  e1_4_action_down(Uint32 interval, void *param);
Uint32  e1_5_action_down(Uint32 interval, void *param);


//function for e2
Uint32  e2_action_walk(Uint32 interval, void *param);
Uint32  e2_action_hit(Uint32 interval, void *param);
Uint32  e2_action_attack(Uint32 interval, void *param);
Uint32  e2_action_stand(Uint32 interval, void *param);
Uint32  e2_action_down(Uint32 interval, void *param);

SDL_Window* window = NULL; // The window we'll be rendering to
SDL_Renderer *renderer = NULL; // The renderer contained by the window
Mix_Music *bg_music = NULL;
Mix_Chunk *punch_effect = NULL;

SDL_Texture *bgTexture = NULL; // The background texture
// SDL_Texture *pikaTexture = NULL;
// SDL_Texture *spTexture = NULL;

int initSDL()
{
	// Initialize SDL	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
	{
		// Error Handling		
		printf("SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	// Create window	
	// SDL_WINDOWPOS_UNDEFINED: Used to indicate that you don't care what the window position is.
	window = SDL_CreateWindow("OOP SDL project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 2;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return 5;
	}

	// Create renderer	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		SDL_DestroyWindow(window);
		printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 3;
	}

	return 0;
}

void closeSDL()
{
	SDL_DestroyTexture(bgTexture);
	// Destroy renderer	
	// Destroy window	
	// Quit SDL subsystems
	Mix_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool loadAudio()
{
	// Load music
	// https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_55.html
	// This can load WAVE, MOD, MIDI, OGG, MP3, FLAC, and any file that you use a command to play with.
	bg_music = Mix_LoadMUS("../audio/bg_music.mp3");
	if (bg_music == NULL)
	{
		printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	// Load sound effects
	// https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_19.html
	// This can load WAVE, AIFF, RIFF, OGG, and VOC files.
	punch_effect = Mix_LoadWAV("../audio/punch.wav");
	if (punch_effect == NULL)
	{
		printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}
}

TextData loadTextTexture(const char *str, const char *fontPath, int fontSize, Uint8 fr, Uint8 fg, Uint8 fb, int textType, Uint8 br, Uint8 bg, Uint8 bb)
{
	TextData text;

	// TTF Font sturct
	// https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_frame.html
	TTF_Font *ttfFont = NULL;

	// Open the font
	// https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_frame.html
	ttfFont = TTF_OpenFont(fontPath, fontSize); //("../fonts/lazy.ttf", 28);
	if (ttfFont == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
	}

	// A structure that represents a color.
	// https://wiki.libsdl.org/SDL_Color
	SDL_Color textFgColor = { fr, fg, fb }, textBgColor = { br, bg, bb };

	// Render text surface
	// The way SDL_ttf works is that you create a new image from a font and color. 
	// We're going to be loading our image from text rendered by SDL_ttf instead of a file.
	// https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_frame.html#SEC42		

	SDL_Surface *textSurface = NULL;

	// Creates a solid/shaded/blended color surface from the font, text, and color given.
	// https://www.libsdl.org/projects/docs/SDL_ttf/SDL_ttf_35.html
	if (textType == SOLID)
	{
		// Quick and Dirty
		textSurface = TTF_RenderText_Solid(ttfFont, str, textFgColor);
	}
	else if (textType == SHADED)
	{
		// Slow and Nice, but with a Solid Box
		textSurface = TTF_RenderText_Shaded(ttfFont, str, textFgColor, textBgColor);
	}
	else if (textType == BLENDED)
	{
		// Slow Slow Slow, but Ultra Nice over another image
		textSurface = TTF_RenderText_Blended(ttfFont, str, textFgColor);
	}

	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		// Create texture from surface pixels
		text.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (text.texture == NULL)
		{
			printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
		}

		//Get text dimensions and information
		text.width = textSurface->w;
		text.height = textSurface->h;

		// Get rid of old loaded surface
		SDL_FreeSurface(textSurface);
	}

	// Free font
	// https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_frame.html
	TTF_CloseFont(ttfFont);

	//return font data
	return text;
}

void textRender(SDL_Renderer *renderer, TextData text, int posX, int posY, int cx, int cy, double angle, SDL_RendererFlip flip)
{
	SDL_Rect r;
	r.x = posX;
	r.y = posY;
	r.w = text.width;
	r.h = text.height;

	SDL_Point center = { cx, cy };

	SDL_RenderCopyEx(renderer, text.texture, NULL, &r, angle, &center, flip);
}

bool setTextureAlpha(SDL_Texture *texture, Uint8 alpha)
{
	// Set and enable standard alpha blending mode for a texture	
	if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) == -1)
	{
		printf("SDL_SetTextureBlendMode failed: %s\n", SDL_GetError());
		return -1;
	}

	// Modulate texture alpha	
	if (SDL_SetTextureAlphaMod(texture, alpha) == -1)
	{
		printf("SDL_SetTextureAlphaMod failed: %s\n", SDL_GetError());
		return -1;
	}
}

void drawRectangleBound(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,\
	Sint16 bound, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	if (((x2 - x1) >= bound) && ((y2 - y1) >= bound))
		for (int i = 0; i < bound; i++)
			rectangleRGBA(renderer, x1 + i, y1 + i, x2 - i, y2 - i, r, g, b, a);
}

ImageData loadTexture(char *path, bool ckEnable, Uint8 r, Uint8 g, Uint8 b)
{
	ImageData img;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("IMG_Load failed: %s\n", IMG_GetError());
	}
	else
	{
		// Set the color key (transparent pixel) in a surface.
		// https://wiki.libsdl.org/SDL_SetColorKey
		// The color key defines a pixel value that will be treated as transparent in a blit. 
		// It is a pixel of the format used by the surface, as generated by SDL_MapRGB().
		// Use SDL_MapRGB() to map an RGB triple to an opaque pixel value for a given pixel format.
		// https://wiki.libsdl.org/SDL_MapRGB
		SDL_SetColorKey(loadedSurface, ckEnable, SDL_MapRGB(loadedSurface->format, r, g, b));

		// Create texture from surface pixels
		img.texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (img.texture == NULL)
		{
			printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
		}

		//Get image dimensions
		img.width = loadedSurface->w;
		img.height = loadedSurface->h;

		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//return newTexture;
	return img;
}

void imgRender(SDL_Renderer *renderer, ImageData img, int posX, int posY, int cx, int cy , \
	double angle, SDL_RendererFlip flip)
{
	SDL_Rect r;
	r.x = posX;
	r.y = posY;
	r.w = img.width;
	r.h = img.height;

	SDL_Point center = { cx, cy };
	SDL_RenderCopyEx(renderer, img.texture, NULL, &r, angle, &center, flip);
}

void imgRender_size(SDL_Renderer *renderer, ImageData img, int posX, int posY, int wid, int hei, \
	int cx = 0, int cy = 0, double angle = 0, SDL_RendererFlip flip = SDL_FLIP_NONE)
{
	SDL_Rect r;
	r.x = posX;
	r.y = posY;
	r.w = wid;
	r.h = hei;

	SDL_Point center = { cx, cy };
	SDL_RenderCopyEx(renderer, img.texture, NULL, &r, angle, &center, flip);
}

void loadImageSet(ImageData *picture_array, char *path, int frame)
{
	for (int i = 0; i < frame; i++)
	{
		char str[100];
		sprintf_s(str, 100, "%s%02d.png", path, i + 1);
		picture_array[i] = loadTexture(str, false, 0xFF, 0xFF, 0xFF);
	}
}

void bg_imgRender(SDL_Renderer *renderer, ImageData *img, int posX, int posY)
{	
	SDL_Rect r;
	r.x = posX;
	r.y = posY;
	r.w = img->width;
	r.h = img->height;
	SDL_RenderCopy(renderer, img->texture, NULL, &r);
}

void show_map(SDL_Renderer *renderer, ImageData *map) {
	if (main_ch.x_pos < (WIDTH / 2)) {
		bg_imgRender(renderer, map, 0, 0);
		//cout << "map1" << endl;
	}
	else if (main_ch.x_pos >= (WIDTH / 2) && main_ch.x_pos <= MAP_LENGTH - (WIDTH / 2)) {
		bg_imgRender(renderer, map, (WIDTH) / 2 - main_ch.x_pos, 0);
		//bg_imgRender(renderer, map, 0, 0);
		//cout << "map2" << endl;
	}
	else if (main_ch.x_pos > MAP_LENGTH - (WIDTH / 2)) {
		bg_imgRender(renderer, map, (WIDTH) - MAP_LENGTH, 0);
		//bg_imgRender(renderer, map, 0, 0);
		//cout << "map3" << endl;
	}
	
	
}

void mc_imgRender(SDL_Renderer *renderer, ImageData img, int posX, int posY, SDL_RendererFlip flip)
{
	SDL_Rect r;
	r.x = posX;
	r.y = posY;
	if (main_ch.action_flag[2] || main_ch.action_flag[3]) // attack
	{
		r.w = 210;
		r.h = 200;
	}
	else
	{
		r.w = 170;
		r.h = 200;
	}
	SDL_RenderCopyEx(renderer, img.texture, NULL, &r, 0, NULL, flip);
}

void e_imgRender(SDL_Renderer *renderer, ImageData img, Character e, SDL_RendererFlip flip, int w, int h)
{
	SDL_Rect r;
	r.x = e.x_pos_d;
	r.y = e.y_pos;
	r.w = w;
	r.h = h;
	
	SDL_RenderCopyEx(renderer, img.texture, NULL, &r, 0, NULL, flip);
}


void mouseHandleEvent(SDL_Event* e, MouseState *mouseState, int *x, int *y)
{
	// If mouse event happened
	// https://wiki.libsdl.org/SDL_Event
	// https://wiki.libsdl.org/SDL_MouseMotionEvent
	// https://wiki.libsdl.org/SDL_MouseButtonEvent
	// https://wiki.libsdl.org/SDL_MouseWheelEvent
	if (true)
	{
		// Get mouse position
		// https://wiki.libsdl.org/SDL_GetMouseState		
		SDL_GetMouseState(x, y);
		printf("(%d, %d)\n", *x, *y);

		// Check if mouse is in button
		bool inside = true;

		//Mouse is left of the button
		if (*x < BUTTON_X)
		{
			inside = false;
		}
		//Mouse is right of the button
		else if (*x > BUTTON_X + BUTTON_WIDTH)
		{
			inside = false;
		}
		//Mouse above the button
		else if (*y < BUTTON_Y)
		{
			inside = false;
		}
		//Mouse below the button
		else if (*y > BUTTON_Y + BUTTON_HEIGHT)
		{
			inside = false;
		}


		// Mouse is outside button
		if (!inside)
		{
			*mouseState = OUT;
		}
		//Mouse is inside button
		else
		{
			//static int lasttime = SDL_GetTicks();
			//static int curtime = SDL_GetTicks();
			//int timediv;

			//lasttime = curtime;
			//curtime = SDL_GetTicks();
			//timediv = curtime - lasttime;

			switch (e->type)
			{
			case SDL_MOUSEBUTTONDOWN:
				break;

			case SDL_MOUSEBUTTONUP:
				//*mouseState = NONE;

				/*
				if (e->button.button == SDL_BUTTON_LEFT && timediv > 800)
				{
					*mouseState = IN_LB_PR;
				}
				else if (e->button.button == SDL_BUTTON_RIGHT && timediv > 800)
				{
					*mouseState = IN_RB_PR;
				}
				else */
				/*if (e->button.button == SDL_BUTTON_LEFT && e->button.clicks == 2 && timediv < 150)
				{
					*mouseState = IN_LB_DC;
				}
				else if (e->button.button == SDL_BUTTON_RIGHT && e->button.clicks == 2 && timediv < 150)
				{
					*mouseState = IN_RB_DC;
				}*/
				if (e->button.button == SDL_BUTTON_LEFT/* && e->button.clicks == 1*/)
				{
					*mouseState = IN_LB_SC;
				}
				else if (e->button.button == SDL_BUTTON_RIGHT/* && e->button.clicks == 1*/)
				{
					*mouseState = IN_RB_SC;
				}

				break;

			case SDL_MOUSEWHEEL:
				if (e->wheel.y > 0) // scroll up
				{
					// Put code for handling "scroll up" here!
					*y = e->wheel.y;
					*mouseState = IN_WU;
				}
				else if (e->wheel.y < 0) // scroll down
				{
					// Put code for handling "scroll down" here!
					*y = e->wheel.y;
					*mouseState = IN_WD;
				}
				break;

			case SDL_MOUSEMOTION:
				*mouseState = HOVER;
				break;
			}
		}
	}
}


void handleEvent(SDL_Event& e)
{
	/* The method for "Debunce" */
	double ratio;
	// If a key was pressed
	// repeat: non-zero if this is a key repeat
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		static int lasttime = SDL_GetTicks();
		static int curtime = SDL_GetTicks();
		int timediv[5];
		int time_flag;

		lasttime = curtime;
		curtime = SDL_GetTicks();
		timediv[0] = curtime - lasttime;
		if (timediv[0] < 300) // judge if double click
			time_flag = 1;
		else
		{
			time_flag = 0;
			for (int i = 0; i < 6; i++)
				keyRecord[i] = 0;
		}
		if (time_flag)
			ratio = 1.5; // for running
		else
			ratio = 1; // for walk
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_a:
			if (checkaction(main_ch, 2) && main_ch.action_flag[2] < 2)
			//if (main_ch.action_flag[2] < 2)
			{
				main_ch.action_flag[2]++;
				keyRecord[4]++;
			}
			break;
		case SDLK_s:
			if (checkaction(main_ch, 3) && main_ch.action_flag[3] < 2)
			// if (main_ch.action_flag[3] < 2)
			{
				main_ch.action_flag[3]++;
				keyRecord[5]++;
			}
			break;
		case SDLK_UP: 
			if (time_flag && keyRecord[0] == 1)
			{
				if (checkaction(main_ch, 1))
					main_ch.action_flag[1] = 1;
					// main_ch.action_flag[0]++;
			}
			else
				main_ch.action_flag[0]++;
			main_ch.y_velo -= (int)main_ch.velocity * ratio;
			keyRecord[0] = 1;
			break;
		case SDLK_DOWN: 
			if (time_flag && keyRecord[0] == 2)
			{
				if (checkaction(main_ch, 1))
					main_ch.action_flag[1] = 1;
				// main_ch.action_flag[0]++;
			}
			else
				main_ch.action_flag[0]++;
			main_ch.y_velo += (int)main_ch.velocity * ratio;
			keyRecord[0] = 2;
			break;
		case SDLK_LEFT:
			if (time_flag && keyRecord[0] == 3)
			{
				if (checkaction(main_ch, 1))
					main_ch.action_flag[1] = 1;
				// main_ch.action_flag[0]++;
			}
			else
				main_ch.action_flag[0]++;
			main_ch.x_velo -= (int)main_ch.velocity * ratio;
			main_ch.way = 1;
			keyRecord[0] = 3;
			break;
		case SDLK_RIGHT: 
			if (time_flag && keyRecord[0] == 4)
			{
				if (checkaction(main_ch, 1))
					main_ch.action_flag[1] = 1;
				// main_ch.action_flag[0]++;
			}
			else
				main_ch.action_flag[0]++;
			main_ch.x_velo += (int)main_ch.velocity * ratio;
			main_ch.way = 0;
			keyRecord[0] = 4;
			break;
		default:
			break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: 
			main_ch.y_velo = 0;
			if (main_ch.action_flag[0] > 0)
				main_ch.action_flag[0]--;
			main_ch.action_flag[1] = 0;
			break;
		case SDLK_DOWN: 
			main_ch.y_velo = 0;
			if (main_ch.action_flag[0] > 0)
				main_ch.action_flag[0]--;
			main_ch.action_flag[1] = 0;
			break;
		case SDLK_LEFT: 
			main_ch.x_velo = 0;
			if (main_ch.action_flag[0] > 0)
				main_ch.action_flag[0]--;
			main_ch.action_flag[1] = 0;
			break;
		case SDLK_RIGHT: 
			main_ch.x_velo = 0;
			if (main_ch.action_flag[0] > 0)
				main_ch.action_flag[0]--;
			main_ch.action_flag[1] = 0;
			break;
		default:
			break;
		}
	}
}

void enemy_move_p(Character *e_list, int e_index) {
	for (int i = 0; i < e_index; i++) {
		if (main_ch.x_pos < (WIDTH / 2)) {
			e_list[i].x_pos_d = e_list[i].x_pos;
			//cout << "mc1" << endl;
		}
		else if (main_ch.x_pos >= (WIDTH / 2) && main_ch.x_pos <= MAP_LENGTH - (WIDTH / 2)) {
			e_list[i].x_pos_d = main_ch.x_pos_d + (e_list[i].x_pos - main_ch.x_pos);
			//cout << "mc2" << endl;
		}
		else if (main_ch.x_pos > MAP_LENGTH - (WIDTH / 2)) {
			e_list[i].x_pos_d = main_ch.x_pos_d + (e_list[i].x_pos - main_ch.x_pos);
			//main_ch.x_pos_d = (WIDTH)-MAP_LENGTH + main_ch.x_pos;
			//cout << "mc3" << endl;
		}
		else {
			cout << "error" << endl;
		}
	}
}

void mc_display_pos() {
	if (main_ch.x_pos < (WIDTH / 2)) {
		main_ch.x_pos_d = main_ch.x_pos;
		// cout << "mc1" << endl;
	}
	else if (main_ch.x_pos >= (WIDTH / 2) && main_ch.x_pos <= MAP_LENGTH - (WIDTH / 2)) {
		main_ch.x_pos_d = WIDTH / 2;
		// cout << "mc2" << endl;
	}
	else if (main_ch.x_pos > MAP_LENGTH - (WIDTH / 2)) {
		main_ch.x_pos_d = (WIDTH)-MAP_LENGTH + main_ch.x_pos;
		cout << "mc3" << endl;
	}
	else {
		cout << "error" << endl;
	}
}

void mc_move()
{
	//Move the dot left or right
	main_ch.x_pos += main_ch.x_velo;

	//If the dot went too far to the left or right
	if ((main_ch.x_pos <= 0) || (main_ch.x_pos >= MAP_LENGTH - 150))
	{
		//Move back
		main_ch.x_pos -= main_ch.x_velo;

	}

	//Move the dot up or down
	main_ch.y_pos += main_ch.y_velo;

	//If the dot went too far up or down
	if ((main_ch.y_pos <= 0) || (main_ch.y_pos >= HEIGHT -200))
	{
		//Move back
		main_ch.y_pos -= main_ch.y_velo;
	}
}

void e_move1(Character &e)
{
	if (!e.action_flag[5] && !e.action_flag[7])
	{
		e.x_pos += e.x_velo;

		//If the dot went too far to the left or right
		if ((e.x_pos <= 0) || (e.x_pos >= MAP_LENGTH - e.w + 50))
		{
			//Move back
			e.x_pos -= e.x_velo;
			e.x_velo = -1 * e.x_velo;
			e.way = (e.way + 1) % 2;
		}

		e.y_pos += e.y_velo;

		//If the dot went too far up or down
		if ((e.y_pos <= 0) || (e.y_pos >= HEIGHT - e.h ))
		{
			//Move back
			e.y_pos -= e.y_velo;
			e.y_velo = -1 * e.y_velo;
		}
	}
}

void e_move2(Character &e, Character &mc) {
	int x_stop = 0, y_stop = 0; // flag for enemy is next to main or not
	if (!e.action_flag[5] && !e.action_flag[2] && !e.action_flag[6])
	{
		int mc_way_x = e.x_pos - mc.x_pos;
		int mc_way_y = e.y_pos - mc.y_pos;

		if (mc_way_x > 120) {
			e.x_pos -= e.x_velo;
			e.way = 0;
		}
		else if (mc_way_x < -120) {
			e.x_pos += e.x_velo;
			e.way = 1;
		}
		else {
			x_stop = 1;
		}

		//If the dot went too far to the left or right
		if ((e.x_pos <= 0) || (e.x_pos >= MAP_LENGTH - 150))
		{
			//Move back
			e.x_pos -= e.x_velo;
			e.x_velo = -1 * e.x_velo;
			e.way = (e.way + 1) % 2;
		}

		if (mc_way_y > 0) {
			e.y_pos -= e.y_velo;
		}
		else if (mc_way_y < 0) {
			e.y_pos += e.y_velo;
		}
		else {
			y_stop = 1;
		}

		//If the dot went too far up or down
		if ((e.y_pos <= 0) || (e.y_pos >= HEIGHT - 200))
		{
			//Move back
			e.y_pos -= e.y_velo;
			e.y_velo = -1 * e.y_velo;
		}

		if (x_stop && y_stop && !main_ch.action_flag[5])
		{
			e.action_flag[6] = 1;
			e.action_flag[2] = 1;
		}
	}
}

void showstatus(SDL_Renderer * renderer, ImageData mc_pro, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,\
	Sint16 bound, Uint8 r, Uint8 g, Uint8 b, Uint8 a, Character ch)
{
	drawRectangleBound(renderer, x1, y1, x2, y2, bound, 0x00, 0x00, 0x00, 0xFF);
	imgRender_size(renderer, mc_pro, x1 + 2, y1 + 2, 86, 86);

	drawRectangleBound(renderer, x1, y1 + 95, y2, y2 + 25, bound, 0x00, 0x00, 0x00, 0xFF);
	roundedBoxRGBA(renderer, x1 + bound, y1 + 95 + bound, x2 - bound, y2 + 25 - bound, 0, 69, 69, 69, 255);

	drawRectangleBound(renderer, x1, y1 + 120, x2, y2 + 50, bound, 0x00, 0x00, 0x00, 0xFF);
	double hp_ratio;
	if (ch.health <= 0) {
		hp_ratio = 0;
	}
	else {
		hp_ratio = 1.0 * ch.health / ch.o_health;
	}

	// roundedBoxRGBA(renderer, x1 + bound, y1 + 120 + bound, x1 + 90 * (hp_ratio) - bound, y2 + 50 - bound, 0, 255, 255, 0, 255);
	if (hp_ratio < 0.0001) {

	}
	else
		roundedBoxRGBA(renderer, x1 + bound, y1 + 120 + bound, (double)x2 - 1.0 * 84 * (1 - hp_ratio) - bound, y2 + 50 - bound, 0, 255, 255, 0, 255);
}

int checkaction(Character ch, int n)
{
	for (int i = 0; i < 9; i++)
		if (ch.action_flag[i] && i != n)
			return 0;
	return 1;
}

int checkcollision1(Character *enemies, int enemy_ind,  SDL_RendererFlip flip)
{
	int m[4]; // x1, y1, w, h
	int e_pos[4]; 
	if (flip == no)
	{
		m[0] = main_ch.x_pos + 150;
		m[1] = main_ch.y_pos + 50;
		m[2] = 60;
		m[3] = 50;
	}
	else
	{
		m[0] = main_ch.x_pos;
		m[1] = main_ch.y_pos + 50;
		m[2] = 60;
		m[3] = 50;
	}
	for (int i = 0; i < enemy_ind; i++)
	{
		e_pos[0] = enemies[i].x_pos;
		e_pos[1] = enemies[i].y_pos;
		e_pos[2] = enemies[i].w;
		e_pos[3] = enemies[i].h;
		if (m[0] + m[2] > e_pos[0] &&
			e_pos[0] + e_pos[2] > m[0] &&
			m[1] + m[3] > e_pos[1] &&
			e_pos[1] + e_pos[3] > m[1]
			)
			return i;
	}
	return -1;
}

int checkcollision2(Character *enemies, int i, SDL_RendererFlip flip)
{
	int m[4]; // x1, y1, w, h
	int e_pos[4]; 
	if (flip == no)
	{
		m[0] = enemies[i].x_pos + 150;
		m[1] = enemies[i].y_pos + 50;
		m[2] = 30;
		m[3] = 60;
	}
	else
	{
		m[0] = enemies[i].x_pos;
		m[1] = enemies[i].y_pos + 50;
		m[2] = 30;
		m[3] = 60;
	}
	e_pos[0] = main_ch.x_pos;
	e_pos[1] = main_ch.y_pos;
	e_pos[2] = main_ch.w;
	e_pos[3] = main_ch.h;
	if (m[0] + m[2] > e_pos[0] &&
		e_pos[0] + e_pos[2] > m[0] &&
		m[1] + m[3] > e_pos[1] &&
		e_pos[1] + e_pos[3] > m[1]
		)
		return 1;
	return 0;
}

void gotDamage(Character &attacker, Character &opponent)
{
	if (opponent.health >= 0) {
		opponent.health -= attacker.damage;
		opponent.action_flag[5]++;
		if (opponent.action_flag[5] >= 4)
		{
			opponent.action_flag[7] = 1;
			opponent.action_flag[5] = 0;
		}
	}
	if (opponent.health <= 0)
		opponent.action_flag[7] = 1;
	// printf("%d\n", opponent.health);
}

void knockup(Character &ch, int hit_index, SDL_RendererFlip flip_flag)
{
	if (hit_index < 4)
	{
		if (flip_flag == no)
		{
			if ((ch.x_pos + 5 <= MAP_LENGTH - ch.w))
				ch.x_pos += 3;
			printf("%d\n", ch.x_pos);
		}
		else if (flip_flag == ho)
		{
			if (ch.x_pos - 5 >= 0)
				ch.x_pos -= 3;
		}
	}
}

Uint32  mc_action_stand(Uint32 interval, void *param)
{
	int *index = (int *)param;
	(*index) = ((*index) + 1) % mc_FRAME;

	return interval;
}

Uint32  mc_action_walk(Uint32 interval, void *param)
{
	int *index = (int *)param;
	(*index) = ((*index) + 1) % mc_FRAME;

	return interval;
}

Uint32  mc_action_run(Uint32 interval, void *param)
{
	int *index = (int *)param;
	(*index) = ((*index) + 1) % mc_FRAME;

	return interval;
}

Uint32  mc_attack_A(Uint32 interval, void *param)
{
	int *index = (int *)param;
	
	if (main_ch.action_flag[2])
	{
		if ((*index) == 0 || (*index) == 4)
		{
			Mix_PlayChannel(-1, punch_effect, 0);
			if (mc_attack_i != -1)
			{
				gotDamage(main_ch, enemy_list[mc_attack_i]);
				// printf("%d\n", (*index));
			}
		}
		if ((*index) < 8)
			(*index) = ((*index) + 1);
	}
	else
		(*index) = 0;
	return interval;
}

Uint32  mc_attack_S(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (main_ch.action_flag[3])
	{
		if ((*index) == 0 || (*index) == 4)
		{
			Mix_PlayChannel(-1, punch_effect, 0);
			if (mc_attack_i != -1)
			{
				gotDamage(main_ch, enemy_list[mc_attack_i]);
				// printf("%d\n", (*index));
			}
		}
		if ((*index) < 8)
			(*index) = ((*index) + 1);
	}
	else
		(*index) = 0;
	return interval;
}

Uint32  mc_action_hit(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (main_ch.action_flag[5])
	{
		if ((*index) == 7)
			main_ch.action_flag[5] = 0;
		(*index) = ((*index) + 1) % 8;
	}
	else
	{
		(*index) == 0;
	}
	return interval;
}

Uint32  e1_action_walk(Uint32 interval, void *param)
{
	int *index = (int *)param;
	(*index) = ((*index) + 1) % e1_walk_f;

	return interval;
}

Uint32  e1_1_action_hit(Uint32 interval, void *param)
{
	static int counter = 0;
	static int last_index = 0;
	int *index = (int *)param;
	if (last_index != (*index))
	{
		counter = 0;
		last_index = (*index);
	}
	if (enemy_list[0].action_flag[5])
	{
		if (counter == 6)
		{
			enemy_list[0].action_flag[5] = 0;
			counter = 0;
			(*index) = 0;
		}
		counter++;
	}

	return interval;
}

Uint32  e1_2_action_hit(Uint32 interval, void *param)
{
	static int counter = 0;
	static int last_index = 0;
	int *index = (int *)param;
	if (last_index != (*index))
	{
		counter = 0;
		last_index = (*index);
	}
	if (enemy_list[1].action_flag[5])
	{
		if (counter == 6)
		{
			enemy_list[1].action_flag[5] = 0;
			counter = 0;
			(*index) = 0;
		}
		counter++;
	}

	return interval;
}
Uint32  e1_3_action_hit(Uint32 interval, void *param)
{
	static int counter = 0;
	static int last_index = 0;
	int *index = (int *)param;
	if (last_index != (*index))
	{
		counter = 0;
		last_index = (*index);
	}
	if (enemy_list[2].action_flag[5])
	{
		if (counter == 6)
		{
			enemy_list[2].action_flag[5] = 0;
			counter = 0;
			(*index) = 0;
		}
		counter++;
	}

	return interval;
}
Uint32  e1_4_action_hit(Uint32 interval, void *param)
{
	static int counter = 0;
	static int last_index = 0;
	int *index = (int *)param;
	if (last_index != (*index))
	{
		counter = 0;
		last_index = (*index);
	}
	if (enemy_list[3].action_flag[5])
	{
		if (counter == 6)
		{
			enemy_list[3].action_flag[5] = 0;
			counter = 0;
			(*index) = 0;
		}
		counter++;
	}

	return interval;
}
Uint32  e1_5_action_hit(Uint32 interval, void *param)
{
	static int counter = 0;
	static int last_index = 0;
	int *index = (int *)param;
	if (last_index != (*index))
	{
		counter = 0;
		last_index = (*index);
	}
	if (enemy_list[4].action_flag[5])
	{
		if (counter == 6)
		{
			enemy_list[4].action_flag[5] = 0;
			counter = 0;
			(*index) = 0;
		}
		counter++;
	}

	return interval;
}

Uint32  e1_1_action_down(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (enemy_list[0].action_flag[7])
	{
		if ((*index) < 3)
			(*index) = ((*index) + 1);
		else
		{
			enemy_list[0].action_flag[7] = 0;
			(*index) = 0;
			if (enemy_list[0].health <= 0)
				enemy_list[0].action_flag[8] = 1;
		}
	}
	else
		(*index) = 0;

	return interval;
}

Uint32  e1_2_action_down(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (enemy_list[1].action_flag[7])
	{
		if ((*index) < 3)
			(*index) = ((*index) + 1);
		else
		{
			enemy_list[1].action_flag[7] = 0;
			(*index) = 0;
			if (enemy_list[1].health <= 0)
				enemy_list[1].action_flag[8] = 1;
		}
	}
	else
		(*index) = 0;

	return interval;
}

Uint32  e1_3_action_down(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (enemy_list[2].action_flag[7])
	{
		if ((*index) < 3)
			(*index) = ((*index) + 1);
		else
		{
			enemy_list[2].action_flag[7] = 0;
			(*index) = 0;
			if (enemy_list[2].health <= 0)
				enemy_list[2].action_flag[8] = 1;
		}
	}
	else
		(*index) = 0;

	return interval;
}

Uint32  e1_4_action_down(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (enemy_list[3].action_flag[7])
	{
		if ((*index) < 3)
			(*index) = ((*index) + 1);
		else
		{
			enemy_list[3].action_flag[7] = 0;
			(*index) = 0;
			if (enemy_list[3].health <= 0)
				enemy_list[3].action_flag[8] = 1;
		}
	}
	else
		(*index) = 0;

	return interval;
}

Uint32  e1_5_action_down(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (enemy_list[4].action_flag[7])
	{
		if ((*index) < 3)
			(*index) = ((*index) + 1);
		else
		{
			enemy_list[4].action_flag[7] = 0;
			(*index) = 0;
			if (enemy_list[4].health <= 0)
				enemy_list[4].action_flag[8] = 1;
		}
	}
	else
		(*index) = 0;

	return interval;
}

Uint32  e2_action_walk(Uint32 interval, void *param)
{
	int *index = (int *)param;
	(*index) = ((*index) + 1) % e2_walk_f;

	return interval;
}

Uint32  e2_action_hit(Uint32 interval, void *param)
{
	static int counter = 0;
	static int last_index = 0;
	int *index = (int *)param;
	if (last_index != (*index))
	{
		counter = 0;
		last_index = (*index);
	}
	if (enemy_list[1].action_flag[5])
	{
		if (counter == 6)
		{
			enemy_list[1].action_flag[5] = 0;
			counter = 0;
			(*index) = 0;
		}
		counter++;
	}

	return interval;
}

Uint32  e2_action_attack(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (enemy_list[1].action_flag[2] && !enemy_list[1].action_flag[6])
	{
		if ((*index) < 2)
			(*index) = ((*index) + 1);
		else
		{
			enemy_list[1].action_flag[2] = 0;
			(*index) = 0;
		}
		if ((*index) == 2)
		{
			if (e2_attack_i)
			{
				gotDamage(enemy_list[1], main_ch);
				// printf("%d\n", (*index));
			}
		}
	}
	else
		(*index) = 0;

	return interval;
}

Uint32  e2_action_stand(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (enemy_list[1].action_flag[6])
	{
		if ((*index) < 2)
			(*index) = ((*index) + 1);
		else
		{
			enemy_list[1].action_flag[6] = 0;
			// enemy_list[1].action_flag[2] = 1;
			(*index) = 0;
		}
	}
	else
		(*index) = 0;

	return interval;
}

Uint32  e2_action_down(Uint32 interval, void *param)
{
	int *index = (int *)param;

	if (enemy_list[1].action_flag[7])
	{
		if ((*index) < 3)
			(*index) = ((*index) + 1);
		else
		{
			enemy_list[1].action_flag[7] = 0;
			enemy_list[1].action_flag[6] = 1;
			(*index) = 0;
			if (enemy_list[1].health <= 0)
				enemy_list[1].action_flag[8] = 1;
		}
	}
	else
		(*index) = 0;

	return interval;
}

int main(int argc, char* args[])
{
	SDL_RendererFlip no = SDL_FLIP_NONE;
	SDL_RendererFlip ho = SDL_FLIP_HORIZONTAL;
	SDL_RendererFlip ve = SDL_FLIP_VERTICAL;
	SDL_RendererFlip hove = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);

	// Start up SDL and create window
	if (initSDL())
	{
		printf("Failed to initialize SDL!\n");
		return -1;
	}

	loadAudio();
//menu
	//bg
	char imgPath_menu[100] = "../images/menu.png";
	ImageData menu;
	menu = loadTexture(imgPath_menu, false, 0xFF, 0xFF, 0xFF);

	//bottom
	char imgPath_start1[100] = "../images/start1.png";
	ImageData start1;
	start1 = loadTexture(imgPath_start1, false, 0xFF, 0xFF, 0xFF);

	char imgPath_start2[100] = "../images/start2.png";
	ImageData start2;
	start2 = loadTexture(imgPath_start2, false, 0xFF, 0xFF, 0xFF);
	//TextData text_norm = loadTextTexture("START GAME!!", "../fonts/VP6y.ttf", 40, 123, 104, 238, SOLID, NULL, NULL, NULL);
	//TextData text_touch = loadTextTexture("START GAME!!!", "../fonts/VP6y.ttf", 45, 106, 90, 205, SOLID, NULL, NULL, NULL);

	bool quit = false;
	bool click = false;
	bool inside = false;
	MouseState mouseState;
	int mouseX, mouseY;
	SDL_Event e;

	//Mix_PlayMusic(bg_music, 0);
	while (!click && !quit)
	{
		
		// Handle events on queue		
		while (SDL_PollEvent(&e) != 0)
		{
			mouseState = NONE;

			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}

				mouseHandleEvent(&e, &mouseState, &mouseX, &mouseY);
			}

			switch (mouseState)
			{
			case NONE:
				break;

			case OUT: // Mouse out
				break;

			case IN_LB_SC:  // Inside, Left Button, Single Click
				click = true;
				printf("Inside, Left Button, Single Click: (%d, %d)\n", mouseX, mouseY);
				break;

			case IN_RB_SC:  // Inside, RIGHT Button, Single Click
				printf("Inside, Right Button, Single Click: (%d, %d)\n", mouseX, mouseY);
				break;

			case IN_LB_DC:  // Inside, Left Button, Double Click
				printf("Inside, Left Button, Double Click: (%d, %d)\n", mouseX, mouseY);
				break;

			case IN_RB_DC:  // Inside, RIGHT Button, Double Click
				printf("Inside, Right Button, Double Click: (%d, %d)\n", mouseX, mouseY);
				break;

			case IN_LB_PR:  // Inside, Left Button, Press
				printf("Inside, Left Button, Press: (%d, %d)\n", mouseX, mouseY);
				break;

			case IN_RB_PR:  // Inside, Left Button, Press
				printf("Inside, Right Button, Press: (%d, %d)\n", mouseX, mouseY);
				break;

			case IN_WU:  // Inside, Wheel UP
				printf("Inside, Wheel Up: y=%d\n", mouseY);
				break;

			case IN_WD:  // Inside, Wheel DOWN
				printf("Inside, Wheel Down: y=%d\n", mouseY);
				break;

			case HOVER: // Mouse hover
				printf("Inside, Hover: (%d, %d)\n", mouseX, mouseY);
				break;
			}
		}
		// Clear screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(renderer);


		bg_imgRender(renderer, &menu, 0, 0);
		if (((480 < mouseX) && (mouseX < 730)) && ((70 < mouseY) && (mouseY < 175))) {
			imgRender_size(renderer, start2, 480, 70, 250, 75);
		}
		else{ 
			imgRender_size(renderer, start1, 480, 70, 250, 75); 
		}
		
		//textRender(renderer, text_norm, 600, 100, NULL, NULL, NULL, no);
		//textRender(renderer, text_touch, 600, 100, NULL, NULL, NULL, no);

		SDL_RenderPresent(renderer);

	}

	if(quit)
		closeSDL();


	// main character setting
	// path of main character profile
	char mc_pro_Path[100] = "../images/mc_pro.png";
	// path for main character
	char mc_stand_Path[100] = "../images/purple/stand/";
	char mc_walk_Path[100] = "../images/purple/walk/";
	char mc_run_Path[100] = "../images/purple/run/";
	char mc_attack1_Path[100] = "../images/purple/attack1/";
	char mc_attack2_Path[100] = "../images/purple/attack2/";
	char mc_hit_Path[100] = "../images/purple/hit/";

	// index of mc anime
	int mc_stand_index = 0;
	int mc_walk_index = 0;
	int mc_run_index = 0;
	int mc_attack1_index = 0;
	int mc_attack2_index = 0;
	int mc_hit_index = 0;


// stage 1
	//Path & index setting
	// path for background
	char imgPath_bg1[100] = "../images/bg1.png";

	//enemy
	// enemy1
	// path for enemy1
	char s1_e1_pro_Path[100] = "../images/s1_e1/s1_e1_pro.png";
	char s1_e1_Path[3][100] = { "../images/s1_e1/walk/" , "../images/s1_e1/hit/", "../images/s1_e1/down/" };
	// enemy1 anime index
	int s1_e1_walk_index = 0;
	int s1_e1_hit_index = 0;
	int s1_e1_down_index = 0;

	// enemy2
	// path for enemy2
	char s1_e2_pro_Path[100] = "../images/s1_e1/s1_e1_pro.png";
	char s1_e2_Path[3][100] = { "../images/s1_e1/walk/" , "../images/s1_e1/hit/", "../images/s1_e1/down/" };
	// enemy1 anime index
	int s1_e2_walk_index = 0;
	int s1_e2_hit_index = 0;
	int s1_e2_down_index = 0;

	// enemy3
	// path for enemy3
	char s1_e3_pro_Path[100] = "../images/s1_e1/s1_e1_pro.png";
	char s1_e3_Path[3][100] = { "../images/s1_e1/walk/" , "../images/s1_e1/hit/", "../images/s1_e1/down/" };
	// enemy1 anime index
	int s1_e3_walk_index = 0;
	int s1_e3_hit_index = 0;
	int s1_e3_down_index = 0;
/*
	// enemy4
	// path for enemy4
	char s1_e4_pro_Path[100] = "../images/s1_e1/s1_e1_pro.png";
	char s1_e4_Path[3][100] = { "../images/s1_e1/walk/" , "../images/s1_e1/hit/", "../images/s1_e1/down/" };
	// enemy1 anime index
	int s1_e4_walk_index = 0;
	int s1_e4_hit_index = 0;
	int s1_e4_down_index = 0;

	// enemy5
	// path for enemy5
	char s1_e5_pro_Path[100] = "../images/s1_e1/s1_e1_pro.png";
	char s1_e5_Path[3][100] = { "../images/s1_e1/walk/" , "../images/s1_e1/hit/", "../images/s1_e1/down/" };
	// enemy1 anime index
	int s1_e5_walk_index = 0;
	int s1_e5_hit_index = 0;
	int s1_e5_down_index = 0;
*/
	ImageData bg1;
	ImageData mc_pro, s1_e1_pro, s1_e2_pro, s1_e3_pro;// , s1_e4_pro, s1_e5_pro;
	ImageData mc_stand[mc_FRAME], mc_walk[mc_FRAME], mc_run[mc_FRAME], mc_attack1[8], mc_attack2[8], mc_hit[8];
	ImageData s1_e1_walk[e1_walk_f], s1_e1_hit[e1_hit_f], s1_e1_down[4];
	ImageData s1_e2_walk[e1_walk_f], s1_e2_hit[e1_hit_f], s1_e2_down[4];
	ImageData s1_e3_walk[e1_walk_f], s1_e3_hit[e1_hit_f], s1_e3_down[4];
	ImageData s1_e4_walk[e1_walk_f], s1_e4_hit[e1_hit_f], s1_e4_down[4];
	ImageData s1_e5_walk[e1_walk_f], s1_e5_hit[e1_hit_f], s1_e5_down[4];

	SDL_RendererFlip mc_flip_flag, s1_e1_flip_flag, s1_e2_flip_flag, s1_e3_flip_flag, s1_e4_flip_flag, s1_e5_flip_flag;

	//load png
		//load png of bg
	bg1 = loadTexture(imgPath_bg1, false, 0xFF, 0xFF, 0xFF);
	MAP_LENGTH = bg1.width;
	
	//load png of mc
	//load profile picture of main character
	mc_pro = loadTexture(mc_pro_Path, false, 0xFF, 0xFF, 0xFF);
	//load png for main character standing
	loadImageSet(mc_stand, mc_stand_Path, mc_FRAME);
	//load png for main character walking
	loadImageSet(mc_walk, mc_walk_Path, mc_FRAME);
	//load png for main character running
	loadImageSet(mc_run, mc_run_Path, mc_FRAME);
	//load png for main character attack1
	loadImageSet(mc_attack1, mc_attack1_Path, 8);
	//load png for main character attack2
	loadImageSet(mc_attack2, mc_attack2_Path, 8);
	loadImageSet(mc_hit, mc_hit_Path, 8);

	//load png of enemy1
	//load png for s1_e1 walk
	s1_e1_pro = loadTexture(s1_e1_pro_Path, false, 0xFF, 0xFF, 0xFF);
	loadImageSet(s1_e1_walk, s1_e1_Path[0], s1_e1_walk_f);
	//load png for e1 hit
	loadImageSet(s1_e1_hit, s1_e1_Path[1], s1_e1_hit_f);
	loadImageSet(s1_e1_down, s1_e1_Path[2], 4);

	//load png of enemy2
	//load png for s1_e2 walk
	s1_e2_pro = loadTexture(s1_e2_pro_Path, false, 0xFF, 0xFF, 0xFF);
	loadImageSet(s1_e2_walk, s1_e2_Path[0], s1_e1_walk_f);
	//load png for e2 hit
	loadImageSet(s1_e2_hit, s1_e2_Path[1], s1_e1_hit_f);
	loadImageSet(s1_e2_down, s1_e2_Path[2], 4);

	//load png of enemy3
	//load png for s1_e3 walk
	s1_e3_pro = loadTexture(s1_e3_pro_Path, false, 0xFF, 0xFF, 0xFF);
	loadImageSet(s1_e3_walk, s1_e3_Path[0], s1_e1_walk_f);
	//load png for e3 hit
	loadImageSet(s1_e3_hit, s1_e3_Path[1], s1_e1_hit_f);
	loadImageSet(s1_e3_down, s1_e3_Path[2], 4);
/*
	//load png of enemy4
	//load png for s1_e4 walk
	s1_e4_pro = loadTexture(s1_e4_pro_Path, false, 0xFF, 0xFF, 0xFF);
	loadImageSet(s1_e4_walk, s1_e4_Path[0], s1_e1_walk_f);
	//load png for e3 hit
	loadImageSet(s1_e4_hit, s1_e4_Path[1], s1_e1_hit_f);
	loadImageSet(s1_e4_down, s1_e4_Path[2], 4);

	//load png of enemy5
	//load png for s1_e5 walk
	s1_e5_pro = loadTexture(s1_e5_pro_Path, false, 0xFF, 0xFF, 0xFF);
	loadImageSet(s1_e5_walk, s1_e5_Path[0], s1_e1_walk_f);
	//load png for e5 hit
	loadImageSet(s1_e5_hit, s1_e5_Path[1], s1_e1_hit_f);
	loadImageSet(s1_e5_down, s1_e5_Path[2], 4);
*/
	//Timer
	//timer for mc
	SDL_TimerID mc_timerID1_stand = SDL_AddTimer(150, mc_action_stand, &mc_stand_index);
	SDL_TimerID mc_timerID2_walk = SDL_AddTimer(150, mc_action_walk, &mc_walk_index);
	SDL_TimerID mc_timerID3_run = SDL_AddTimer(100, mc_action_run, &mc_run_index);
	SDL_TimerID mc_timerID4_attack1 = SDL_AddTimer(100, mc_attack_A, &mc_attack1_index);
	SDL_TimerID mc_timerID5_attack2 = SDL_AddTimer(150, mc_attack_S, &mc_attack2_index);
	SDL_TimerID mc_timerID6_hit = SDL_AddTimer(150, mc_action_hit, &mc_hit_index);

	//timer for s1_e1
	SDL_TimerID s1_e1_timerID1_walk = SDL_AddTimer(150, e1_action_walk, &s1_e1_walk_index);
	SDL_TimerID s1_e1_timerID2_hit = SDL_AddTimer(100, e1_1_action_hit, &s1_e1_hit_index);
	SDL_TimerID s1_e1_timerID3_down = SDL_AddTimer(200, e1_1_action_down, &s1_e1_down_index);

	//timer for s1_e2
	SDL_TimerID s1_e2_timerID1_walk = SDL_AddTimer(150, e1_action_walk, &s1_e2_walk_index);
	SDL_TimerID s1_e2_timerID2_hit = SDL_AddTimer(100, e1_2_action_hit, &s1_e2_hit_index);
	SDL_TimerID s1_e2_timerID3_down = SDL_AddTimer(200, e1_2_action_down, &s1_e2_down_index);

	//timer for s1_e3
	SDL_TimerID s1_e3_timerID1_walk = SDL_AddTimer(150, e1_action_walk, &s1_e3_walk_index);
	SDL_TimerID s1_e3_timerID2_hit = SDL_AddTimer(100, e1_3_action_hit, &s1_e3_hit_index);
	SDL_TimerID s1_e3_timerID3_down = SDL_AddTimer(200, e1_3_action_down, &s1_e3_down_index);
/*
	//timer for s1_e4
	SDL_TimerID s1_e4_timerID1_walk = SDL_AddTimer(150, e1_action_walk, &s1_e4_walk_index);
	SDL_TimerID s1_e4_timerID2_hit = SDL_AddTimer(100, e1_4_action_hit, &s1_e4_hit_index);
	SDL_TimerID s1_e4_timerID3_down = SDL_AddTimer(200, e1_4_action_down, &s1_e4_down_index);

	//timer for s1_e5
	SDL_TimerID s1_e5_timerID1_walk = SDL_AddTimer(150, e1_action_walk, &s1_e5_walk_index);
	SDL_TimerID s1_e5_timerID2_hit = SDL_AddTimer(100, e1_5_action_hit, &s1_e5_hit_index);
	SDL_TimerID s1_e5_timerID3_down = SDL_AddTimer(200, e1_5_action_down, &s1_e5_down_index);
*/


	//Initialize
	// initialize main character
	main_ch.x_pos = 10;
	main_ch.x_pos_d = main_ch.x_pos_d;
	main_ch.y_pos = HEIGHT / 2;
	main_ch.x_velo = 0;
	main_ch.y_velo = 0;
	main_ch.velocity = 3;
	main_ch.o_health = MAIN_HP;
	main_ch.health = MAIN_HP;
	main_ch.damage = 10;
	main_ch.w = 200;
	main_ch.h = 200;

	enemy_index = 3;
	enemy_list = new Character[enemy_index];

	// initialize s1 enemy1
	enemy_list[0].x_pos = 400;
	enemy_list[0].y_pos = 300;
	enemy_list[0].x_velo = 3;
	enemy_list[0].y_velo = 2;
	enemy_list[0].way = 1;
	enemy_list[0].o_health = 160;
	enemy_list[0].health = 160;
	enemy_list[0].h = 213;
	enemy_list[0].w = 155;

	// initialize s1 enemy2
	enemy_list[1].x_pos = 650;
	enemy_list[1].y_pos = 400;
	enemy_list[1].x_velo = 2;
	enemy_list[1].y_velo = 2;
	enemy_list[1].way = 1;
	enemy_list[1].o_health = 80;
	enemy_list[1].health = 80;
	enemy_list[1].h = 160;
	enemy_list[1].w = 116;

	// initialize s1 enemy3
	enemy_list[2].x_pos = 400;
	enemy_list[2].y_pos = 400;
	enemy_list[2].x_velo = -2;
	enemy_list[2].y_velo = 3;
	enemy_list[2].way = 0;
	enemy_list[2].o_health = 75;
	enemy_list[2].health = 75;
	enemy_list[2].h = 160;
	enemy_list[2].w = 116;
/*
	// initialize s1 enemy4
	enemy_list[3].x_pos = 700;
	enemy_list[3].y_pos = 100;
	enemy_list[3].x_velo = -3;
	enemy_list[3].y_velo = 3;
	enemy_list[3].way = 0;
	enemy_list[3].health = 55;
	enemy_list[3].h = 160;
	enemy_list[3].w = 116;

	// initialize s1 enemy5
	enemy_list[4].x_pos = 600;
	enemy_list[4].y_pos = 350;
	enemy_list[4].x_velo = 4;
	enemy_list[4].y_velo = 2;
	enemy_list[4].way = 1;
	enemy_list[4].health = 60;
	enemy_list[4].h = 160;
	enemy_list[4].w = 116;
*/
	//Main loop flag
	quit = false;

	Mix_PlayMusic(bg_music, 0);
	while ((!quit))
	{
		if (((10000 < enemy_list[0].health || enemy_list[0].health <= 0) && (10000 < enemy_list[1].health || enemy_list[1].health <= 0) && (10000 < enemy_list[2].health || enemy_list[2].health <= 0))) {
			if (main_ch.x_pos >= MAP_LENGTH - 170)
				break;
		}
		// Handle events on queue		
		while (SDL_PollEvent(&e) != 0)
		{
			// User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			// Handle input for the dot
			handleEvent(e);
		}

		// character action
		if (!main_ch.action_flag[2] && !main_ch.action_flag[3] && !main_ch.action_flag[5])
			mc_move();
		e_move1(enemy_list[0]);
		e_move1(enemy_list[1]);
		e_move1(enemy_list[2]);
	//	e_move1(enemy_list[3]);
	//	e_move1(enemy_list[4]);
		

		// Clear screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(renderer);

		//Render handling
			//bg rendering
		show_map(renderer, &bg1);
		// cout << "x_pos : " << main_ch.x_pos << endl;
		// cout << "x_pos_d : " << main_ch.x_pos_d << endl;
		//bg_imgRender(renderer, bg, 0, 0);

		
		// printf("%d %d %d %d\n", main_ch.action_flag[0], main_ch.action_flag[1], main_ch.action_flag[2], main_ch.action_flag[3]); // display character status

		//enemy rendering
		enemy_move_p(enemy_list, enemy_index);

		if (!enemy_list[0].way)
			s1_e1_flip_flag = ho;
		else
			s1_e1_flip_flag = no;

		if (!enemy_list[1].way)
			s1_e2_flip_flag = ho;
		else
			s1_e2_flip_flag = no;

		if (!enemy_list[2].way)
			s1_e3_flip_flag = ho;
		else
			s1_e3_flip_flag = no;

		

		if (main_ch.way)
			mc_flip_flag = ho;
		else
			mc_flip_flag = no;

		//enemy1 rendering
		if (enemy_list[0].health > 0 && enemy_list[0].health < 1000)
		{
			if (enemy_list[0].action_flag[7]) {
				e_imgRender(renderer, s1_e1_down[s1_e1_down_index], enemy_list[0], s1_e1_flip_flag, 184, 213);
				if (s1_e1_down_index <= 2)
					knockup(enemy_list[0], 1, mc_flip_flag);
			}
			else if (enemy_list[0].action_flag[5]) {
				s1_e1_hit_index = (enemy_list[0].action_flag[5] - 1) % 3;
				e_imgRender(renderer, s1_e1_hit[s1_e1_hit_index], enemy_list[0], s1_e1_flip_flag, 194, 216);
			}
			else {
				e_imgRender(renderer, s1_e1_walk[s1_e1_walk_index], enemy_list[0], s1_e1_flip_flag, 155, 213);
			}
		}

		//enemy2 rendering
		if (enemy_list[1].health > 0 && enemy_list[1].health < 1000)
		{
			if (enemy_list[1].action_flag[7]) {
				e_imgRender(renderer, s1_e2_down[s1_e2_down_index], enemy_list[1], s1_e2_flip_flag, 146, 160);
				if (s1_e2_down_index <= 2)
					knockup(enemy_list[1], 1, mc_flip_flag);
			}
			else if (enemy_list[1].action_flag[5]) {
				s1_e2_hit_index = (enemy_list[1].action_flag[5] - 1) % 3;
				e_imgRender(renderer, s1_e2_hit[s1_e2_hit_index], enemy_list[1], s1_e2_flip_flag, 146, 162);
			}
			else {
				e_imgRender(renderer, s1_e2_walk[s1_e2_walk_index], enemy_list[1], s1_e2_flip_flag, 116, 160);
			}
		}

		//enemy3 rendering
		if (enemy_list[2].health > 0 && enemy_list[2].health < 1000)
		{
			if (enemy_list[2].action_flag[7]) {
				e_imgRender(renderer, s1_e3_down[s1_e3_down_index], enemy_list[2], s1_e3_flip_flag , 146, 160);
				if (s1_e3_down_index <= 2)
					knockup(enemy_list[2], 1, mc_flip_flag);
			}
			else if (enemy_list[2].action_flag[5]) {
				s1_e3_hit_index = (enemy_list[2].action_flag[5] - 1) % 3;
				e_imgRender(renderer, s1_e3_hit[s1_e3_hit_index], enemy_list[2], s1_e3_flip_flag , 146, 162);
			}
			else {
				e_imgRender(renderer, s1_e3_walk[s1_e3_walk_index], enemy_list[2], s1_e3_flip_flag , 116, 160);
			}
		}
/*
		//enemy4 rendering
		if (enemy_list[3].health > 0 && enemy_list[3].health < 1000)
		{
			if (enemy_list[3].action_flag[7]) {
				e_imgRender(renderer, s1_e4_down[s1_e4_down_index], enemy_list[3], s1_e4_flip_flag , 146, 160);
				if (s1_e4_down_index <= 2)
					knockup(enemy_list[3], 1, mc_flip_flag);
			}
			else if (enemy_list[4].action_flag[5]) {
				s1_e4_hit_index = (enemy_list[3].action_flag[5] - 1) % 3;
				e_imgRender(renderer, s1_e4_hit[s1_e4_hit_index], enemy_list[3], s1_e4_flip_flag , 146, 162);
			}
			else {
				e_imgRender(renderer, s1_e4_walk[s1_e4_walk_index], enemy_list[3], s1_e4_flip_flag , 116, 160);
			}
		}

		//enemy5 rendering
		if (enemy_list[4].health > 0 && enemy_list[4].health < 1000)
		{
			if (enemy_list[4].action_flag[7]) {
				e_imgRender(renderer, s1_e5_down[s1_e5_down_index], enemy_list[4], s1_e5_flip_flag, 146, 160);
				if (s1_e5_down_index <= 2)
					knockup(enemy_list[4], 1, mc_flip_flag);
			}
			else if (enemy_list[4].action_flag[5]) {
				s1_e5_hit_index = (enemy_list[4].action_flag[5] - 1) % 3;
				e_imgRender(renderer, s1_e5_hit[s1_e5_hit_index], enemy_list[4], s1_e5_flip_flag, 146, 162);
			}
			else {
				e_imgRender(renderer, s1_e5_walk[s1_e5_walk_index], enemy_list[4], s1_e5_flip_flag, 116, 160);
			}
		}
*/
		//mc rendering
		mc_display_pos();

		if (main_ch.action_flag[5]) {
			//knockup(main_ch, mc_hit_index, e2_flip_flag);
			if (mc_hit_index >= 2 && mc_hit_index <= 5)
				e_imgRender(renderer, mc_hit[mc_hit_index], main_ch, mc_flip_flag, main_ch.w + 50, main_ch.h);
			else
				mc_imgRender(renderer, mc_hit[mc_hit_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
		}
		else if (main_ch.action_flag[2]) {
			mc_imgRender(renderer, mc_attack1[mc_attack1_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
			mc_attack_i = checkcollision1(enemy_list, enemy_index, mc_flip_flag);
			if (mc_attack1_index == 3)
			{
				if (main_ch.action_flag[2] == 1)
				{
					main_ch.action_flag[2] = 0;
					mc_attack1_index = 0;
				}
			}
			if (mc_attack1_index == 7)
			{
				if (main_ch.action_flag[2] == 2)
				{
					main_ch.action_flag[2] = 0;
					mc_attack1_index = 0;
				}
			}
		}
		else if (main_ch.action_flag[3]) {
			mc_imgRender(renderer, mc_attack2[mc_attack2_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
			mc_attack_i = checkcollision1(enemy_list, enemy_index, mc_flip_flag);
			if (mc_attack2_index == 3)
			{
				if (main_ch.action_flag[3] == 1)
				{
					main_ch.action_flag[3] = 0;
					mc_attack2_index = 0;
				}
			}
			if (mc_attack2_index == 7)
			{
				if (main_ch.action_flag[3] == 2)
				{
					main_ch.action_flag[3] = 0;
					mc_attack2_index = 0;
				}
			}
		}
		else if (main_ch.action_flag[1]) {
			mc_imgRender(renderer, mc_run[mc_run_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
		}
		else if (main_ch.action_flag[0]) {
			mc_imgRender(renderer, mc_walk[mc_walk_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
		}
		else {
			mc_imgRender(renderer, mc_stand[mc_stand_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
		}


		

		//display health of main character
		//showstatus(renderer, mc_pro, 20, 20, 110, 110, 3, 0x00, 0x00, 0x00, 0xFF, main_ch);
		//showstatus(renderer, s1_e1_pro, 780, 20, 870, 110, 3, 0x00, 0x00, 0x00, 0xFF, enemy_list[0]);
		//showstatus(renderer, s1_e2_pro, 890, 20, 980, 110, 3, 0x00, 0x00, 0x00, 0xFF, enemy_list[1]);

		showstatus(renderer, mc_pro, 20, 20, 110, 110, 3, 0x00, 0x00, 0x00, 0xFF, main_ch);
		showstatus(renderer, s1_e1_pro, 580, 20, 670, 110, 3, 0x00, 0x00, 0x00, 0xFF, enemy_list[0]);
		showstatus(renderer, s1_e2_pro, 690, 20, 780, 110, 3, 0x00, 0x00, 0x00, 0xFF, enemy_list[1]);
		showstatus(renderer, s1_e3_pro, 470, 20, 560, 110, 3, 0x00, 0x00, 0x00, 0xFF, enemy_list[2]);
		SDL_RenderPresent(renderer);

		cout << enemy_list[0].health << " " << enemy_list[1].health << " " << enemy_list[2].health << endl;
		cout << main_ch.x_pos << MAP_LENGTH << endl;

	}

	delete[] enemy_list;
	SDL_RemoveTimer(s1_e1_timerID1_walk);
	SDL_RemoveTimer(s1_e1_timerID2_hit);
	SDL_RemoveTimer(s1_e1_timerID3_down);
	SDL_RemoveTimer(s1_e2_timerID1_walk);
	SDL_RemoveTimer(s1_e2_timerID2_hit);
	SDL_RemoveTimer(s1_e2_timerID3_down);
	SDL_RemoveTimer(s1_e3_timerID1_walk);
	SDL_RemoveTimer(s1_e3_timerID2_hit);
	SDL_RemoveTimer(s1_e3_timerID3_down);
/*	SDL_RemoveTimer(s1_e4_timerID1_walk);
	SDL_RemoveTimer(s1_e4_timerID2_hit);
	SDL_RemoveTimer(s1_e4_timerID3_down);
	SDL_RemoveTimer(s1_e5_timerID1_walk);
	SDL_RemoveTimer(s1_e5_timerID2_hit);
	SDL_RemoveTimer(s1_e5_timerID3_down);

	
*/
	if(quit == true)
		closeSDL();
// stage demo
	//Path & index setting
	// path for background
	char imgPath_bg[100] = "../images/bg.png";

	


	
	// enemy1
	// path for enemy1
	char e1_pro_Path[100] = "../images//e1/e1_pro.png";
	char e1_Path[3][100] = { "../images/e1/walk/" , "../images/e1/hit/", "../images/e1/down/" };
	// enemy1 anime index
	int e1_walk_index = 0;
	int e1_hit_index = 0;
	int e1_down_index = 0;

	// enemy2
	// path for enemy2
	char e2_pro_Path[100] = "../images//e2/e2_pro.png";
	char e2_Path[5][100] = { "../images/e2/walk/", "../images/e2/hit/", 
		"../images/e2/attack/", "../images/e2/stand/", "../images/e2/down/" };
	// enemy2 anime index
	int e2_walk_index = 0;
	int e2_hit_index = 0;
	int e2_attack_index = 0;
	int e2_stand_index = 0;
	int e2_down_index = 0;

	
	// generate images of each object
	ImageData bg;
	ImageData e1_pro, e2_pro;
//	ImageData mc_stand[mc_FRAME], mc_walk[mc_FRAME], mc_run[mc_FRAME], mc_attack1[8], mc_attack2[8], mc_hit[8];
	ImageData e1_walk[e1_walk_f], e1_hit[e1_hit_f], e1_down[4];
	ImageData e2_walk[e2_walk_f] , e2_hit[3], e2_attack[3], e2_stand[3], e2_down[5];

	//SDL_RendererFlip mc_flip_flag;
	SDL_RendererFlip e1_flip_flag, e2_flip_flag;

	

//	SDL_Event e;

//load png
	//load png of bg
	bg = loadTexture(imgPath_bg, false, 0xFF, 0xFF, 0xFF);
	MAP_LENGTH = bg.width;
	// cout << MAP_LENGTH << endl;
	//load png of mc
	//load profile picture of main character
	mc_pro = loadTexture(mc_pro_Path, false, 0xFF, 0xFF, 0xFF);
	//load png for main character standing
	loadImageSet(mc_stand, mc_stand_Path, mc_FRAME);
	//load png for main character walking
	loadImageSet(mc_walk, mc_walk_Path, mc_FRAME);
	//load png for main character running
	loadImageSet(mc_run, mc_run_Path, mc_FRAME);
	//load png for main character attack1
	loadImageSet(mc_attack1, mc_attack1_Path, 8);
	//load png for main character attack2
	loadImageSet(mc_attack2, mc_attack2_Path, 8);
	loadImageSet(mc_hit, mc_hit_Path, 8);

	//load png of enemy1
	//load png for e1 walk
	e1_pro = loadTexture(e1_pro_Path, false, 0xFF, 0xFF, 0xFF);
	loadImageSet(e1_walk, e1_Path[0], e1_walk_f);
	//load png for e1 hit
	loadImageSet(e1_hit, e1_Path[1], 3);
	loadImageSet(e1_down, e1_Path[2], 4);

	//load png of enemy2
	//load png for e2 walk
	e2_pro = loadTexture(e2_pro_Path, false, 0xFF, 0xFF, 0xFF);
	loadImageSet(e2_walk, e2_Path[0], e2_walk_f);
	loadImageSet(e2_hit, e2_Path[1], 3);
	loadImageSet(e2_attack, e2_Path[2], 3);
	loadImageSet(e2_stand, e2_Path[3], 3);
	loadImageSet(e2_down, e2_Path[4], 4);

//Timer
	//timer for mc
	/*SDL_TimerID mc_timerID1_stand = SDL_AddTimer(150, mc_action_stand, &mc_stand_index);
	SDL_TimerID mc_timerID2_walk = SDL_AddTimer(150, mc_action_walk, &mc_walk_index);
	SDL_TimerID mc_timerID3_run = SDL_AddTimer(100, mc_action_run, &mc_run_index);
	SDL_TimerID mc_timerID4_attack1 = SDL_AddTimer(100, mc_attack_A, &mc_attack1_index);
	SDL_TimerID mc_timerID5_attack2 = SDL_AddTimer(150, mc_attack_S, &mc_attack2_index);
	SDL_TimerID mc_timerID6_hit = SDL_AddTimer(150, mc_action_hit, &mc_hit_index);
	*/

	//timer for e1
	SDL_TimerID e1_timerID1_walk = SDL_AddTimer(150, e1_action_walk, &e1_walk_index);
	SDL_TimerID e1_timerID2_hit = SDL_AddTimer(100, e1_1_action_hit, &e1_hit_index);
	SDL_TimerID e1_timerID3_down = SDL_AddTimer(200, e1_1_action_down, &e1_down_index);

	//timer for e2
	SDL_TimerID e2_timerID1_walk = SDL_AddTimer(100, e2_action_walk, &e2_walk_index);
	SDL_TimerID e2_timerID2_hit = SDL_AddTimer(100, e2_action_hit, &e2_hit_index);
	SDL_TimerID e2_timerID3_attack = SDL_AddTimer(200, e2_action_attack, &e2_attack_index);
	SDL_TimerID e2_timerID4_stand = SDL_AddTimer(300, e2_action_stand, &e2_stand_index);
	SDL_TimerID e2_timerID5_down = SDL_AddTimer(200, e2_action_down, &e2_down_index);

//Initialize
	// initialize main character
	main_ch.x_pos = 10;
	main_ch.x_pos_d = main_ch.x_pos_d;
	main_ch.y_pos = HEIGHT / 2;
	main_ch.x_velo = 0;
	main_ch.y_velo = 0;
	main_ch.velocity = 3;
	main_ch.health = MAIN_HP;
	main_ch.damage = 10;
	main_ch.w = 200;
	main_ch.h = 200;

	enemy_index = 2;
	enemy_list = new Character[enemy_index];
	
	// initialize enemy1
	enemy_list[0].x_pos = 500;
	enemy_list[0].y_pos = HEIGHT / 2;
	enemy_list[0].x_velo = 2;
	enemy_list[0].y_velo = 1;
	enemy_list[0].way = 1;
	enemy_list[0].o_health = 70;
	enemy_list[0].health = 70;
	enemy_list[0].h= 180;
	enemy_list[0].w = 100;

	// initialize enemy2
	enemy_list[1].x_pos = 300;
	enemy_list[1].y_pos = 300;
	enemy_list[1].x_velo = 2;
	enemy_list[1].y_velo = 1;
	enemy_list[1].way = 1;
	enemy_list[1].o_health = 130;
	enemy_list[1].health = 130;
	enemy_list[1].h = 180;
	enemy_list[1].w = 150;
	enemy_list[1].damage = 5;

//Main loop flag
	 quit = false;

	Mix_PlayMusic(bg_music, 0);
	while ((!quit))
	{
		if (((10000 < enemy_list[0].health || enemy_list[0].health <= 0) && (10000 < enemy_list[1].health || enemy_list[1].health <= 0) )) {
			if (main_ch.x_pos >= MAP_LENGTH - 170)
				break;
		}
		// Handle events on queue		
		while (SDL_PollEvent(&e) != 0)
		{
			// User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true; 
			}

			// Handle input for the dot
			handleEvent(e);
		}

	// character action
		if (!main_ch.action_flag[2] && !main_ch.action_flag[3] && !main_ch.action_flag[5])
			mc_move();
		e_move1(enemy_list[0]);
		e_move2(enemy_list[1], main_ch);

	// Clear screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(renderer);

	//Render handling
		//bg rendering
		show_map(renderer, &bg);
		// cout << "x_pos : " << main_ch.x_pos << endl;
		// cout << "x_pos_d : " << main_ch.x_pos_d << endl;
		//bg_imgRender(renderer, bg, 0, 0);

		//mc rendering
		mc_display_pos();

		if (main_ch.way)
			mc_flip_flag = ho;
		else
			mc_flip_flag = no;

		if (!enemy_list[0].way)
			e1_flip_flag = ho;
		else
			e1_flip_flag = no;

		if (!enemy_list[1].way)
			e2_flip_flag = ho;
		else
			e2_flip_flag = no;


		if (main_ch.action_flag[5]) {
			knockup(main_ch, mc_hit_index, e2_flip_flag);
			if (mc_hit_index >= 2 && mc_hit_index <= 5)
				e_imgRender(renderer, mc_hit[mc_hit_index], main_ch, mc_flip_flag, main_ch.w + 50, main_ch.h );
			else
				mc_imgRender(renderer, mc_hit[mc_hit_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
		}
		else if (main_ch.action_flag[2]) {
			mc_imgRender(renderer, mc_attack1[mc_attack1_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
			mc_attack_i = checkcollision1(enemy_list, enemy_index, mc_flip_flag);
			if (mc_attack1_index == 3)
			{
				if (main_ch.action_flag[2] == 1)
				{
					main_ch.action_flag[2] = 0;
					mc_attack1_index = 0;
				}
			}
			if (mc_attack1_index == 7)
			{
				if (main_ch.action_flag[2] == 2)
				{
					main_ch.action_flag[2] = 0;
					mc_attack1_index = 0;
				}
			}
		}
		else if (main_ch.action_flag[3]) {
			mc_imgRender(renderer, mc_attack2[mc_attack2_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
			mc_attack_i = checkcollision1(enemy_list, enemy_index, mc_flip_flag);
			if (mc_attack2_index == 3)
			{
				if (main_ch.action_flag[3] == 1)
				{
					main_ch.action_flag[3] = 0;
					mc_attack2_index = 0;
				}
			}
			if (mc_attack2_index == 7)
			{
				if (main_ch.action_flag[3] == 2)
				{
					main_ch.action_flag[3] = 0;
					mc_attack2_index = 0;
				}
			}
		}
		else if (main_ch.action_flag[1]) {
			mc_imgRender(renderer, mc_run[mc_run_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
		}
		else if (main_ch.action_flag[0]) {
			mc_imgRender(renderer, mc_walk[mc_walk_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
		}
		else {
			mc_imgRender(renderer, mc_stand[mc_stand_index], main_ch.x_pos_d, main_ch.y_pos, mc_flip_flag);
		}

		
		// printf("%d %d %d %d\n", main_ch.action_flag[0], main_ch.action_flag[1], main_ch.action_flag[2], main_ch.action_flag[3]); // display character status

		//enemy rendering
		enemy_move_p(enemy_list, enemy_index);

		//enemy1 rendering
		if (enemy_list[0].health > 0 && enemy_list[0].health < 101)
		{
			if (enemy_list[0].action_flag[7]) {
				e_imgRender(renderer, e1_down[e1_down_index], enemy_list[0], e1_flip_flag, enemy_list[1].w, enemy_list[1].h);
				if (e2_down_index <= 2)
					knockup(enemy_list[0], 1, mc_flip_flag);
			}
			else if (enemy_list[0].action_flag[5]) {
				e1_hit_index = (enemy_list[0].action_flag[5] - 1) % 3;
				e_imgRender(renderer, e1_hit[e1_hit_index], enemy_list[0], e1_flip_flag, enemy_list[0].w + 70, enemy_list[0].h);
			}
			else {
				e_imgRender(renderer, e1_walk[e1_walk_index], enemy_list[0], e1_flip_flag, enemy_list[0].w, enemy_list[0].h);
			}
		}

		//enemy2 rendering
		if (enemy_list[1].health >= 0 && enemy_list[1].health < 10000)
		{
			printf("%d\n", enemy_list[1].action_flag[8]);
			if (enemy_list[1].action_flag[7]) {
				e_imgRender(renderer, e2_down[e2_down_index], enemy_list[1], e2_flip_flag, enemy_list[1].w, enemy_list[1].h);
				if (e2_down_index <= 2)
					knockup(enemy_list[1], 1, mc_flip_flag);
			}
			else if (enemy_list[1].action_flag[5]) {
				e2_hit_index = (enemy_list[1].action_flag[5] - 1) % 3;
				e_imgRender(renderer, e2_hit[e2_hit_index], enemy_list[1], e2_flip_flag, enemy_list[1].w, enemy_list[1].h);
			}
			else if (enemy_list[1].action_flag[6])
			{
				e_imgRender(renderer, e2_stand[e2_stand_index], enemy_list[1], e2_flip_flag, enemy_list[1].w, enemy_list[1].h);
			}
			else if (enemy_list[1].action_flag[2])
			{
				e2_attack_i = checkcollision2(enemy_list, 1, e2_flip_flag);
				e_imgRender(renderer, e2_attack[e2_attack_index], enemy_list[1], e2_flip_flag, enemy_list[1].w + 40, enemy_list[1].h);
			}
			else {
				e_imgRender(renderer, e2_walk[e2_walk_index], enemy_list[1], e2_flip_flag, enemy_list[1].w, enemy_list[1].h);
			}
		}

		//display health of main character
		showstatus(renderer, mc_pro, 20, 20, 110, 110, 3, 0x00, 0x00, 0x00, 0xFF, main_ch);
		showstatus(renderer, e1_pro, 580, 20, 670, 110, 3, 0x00, 0x00, 0x00, 0xFF, enemy_list[0]);
		showstatus(renderer, e2_pro, 690, 20, 780, 110, 3, 0x00, 0x00, 0x00, 0xFF, enemy_list[1]);

		SDL_RenderPresent(renderer);

	}
	if (quit == true)
		closeSDL();

	char imgPath_end[100] = "../images/end.png";
	ImageData end = loadTexture(imgPath_end, false, 0xFF, 0xFF, 0xFF);

	quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0)
		{
			// User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			// Handle input for the dot
			handleEvent(e);
		}

		// Clear screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(renderer);

		imgRender_size(renderer, end, 0, 0,  800, 575);
		SDL_RenderPresent(renderer);
	}

	
	//Free resources and close SDL
	closeSDL();

	return 0;
}