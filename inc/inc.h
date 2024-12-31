#ifndef LIBS
#define LIBS

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <unistr.h>

#endif

#ifndef CONSTS
#define CONSTS

#define ISBIGOP			0b00000001
#define ISPARENTHESIS	0b00000010
#define ISFRAC			0b00000100
#define CLOSEPAR		0b00001000
#define MAKESUP			0b00010000
#define MAKESUB			0b00100000

#define FONTSIZE 64
#define BIGOPFONTSIZE 96

#endif

#ifndef ASTRUCTS
#define ASTRUCTS

typedef struct {
	int bufferpos;
	int posx;
	int posy;
} Conf;

typedef struct {
	uint8_t flags;
	uint8_t * token;
	uint8_t * chara;
} Token;

typedef struct Buffer{ // Textos a ser parseados
	int spanx;
	int spany;
	int sizex;
	int sizey;
	int posx;
	int posy;
	int tokensize;
	uint8_t * parsetext;
	Token * tokens;
	struct Buffer *subtext;
	struct Buffer *suptext;
	struct Buffer *next;
} Buffer;

typedef struct {
	bool quit;
	bool updatewin;
	int numtokens;
	int * screenx;
	int * screeny;
	int * movex;
	int * prevhmax;
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Surface * screenSurface;
	Buffer * buffer;
	Token * tokens;
	Conf * conf;
} Symat;

#endif

#ifndef FUNCS
#define FUNCS

bool GetErrors(Symat * symat,char * config);
void initSymat(Symat * symat, char * config);
int getNumTokens(char * config);
Token * getTokens(char * config,int numtokens);
void readTokens(Symat * symat, Buffer * dest);
void updateBuffer(Token *dest,int destlen, Buffer *origin, Token * tokens);
void drawLine(Symat * symat, Buffer * buffer, Token * tokens, int maxtokens,float factor,int spanx,int spany,int medy,int * movex,int *prevhmax);
void drawBuffer(Symat * symat, Buffer * buffer,float factor,int spanx,int spany,int * movex,int *prevhmax,bool firsttime);
float getfactor(char * num);
Buffer initBuffer();
Buffer * setBuffer();

#endif
