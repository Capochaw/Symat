#include "inc.h"

TTF_Font * mainfont;
TTF_Font * bigopfont;
SDL_Color color = {0,0,0,255};

bool GetErrors(Symat * symat,char * config){
	initSymat(symat,config);

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("ErrorSDL: %s\n",SDL_GetError());
		return true;
	}
	symat->window = SDL_CreateWindow("Symat",1200,
			40, 700,300,SDL_WINDOW_SHOWN);
			// SCREEN_WIDTH,
			// SCREEN_HEIGHT,
			// SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	symat->renderer = SDL_CreateRenderer(symat->window,-1,SDL_RENDERER_ACCELERATED);
	if(symat->window == NULL || symat->renderer == NULL || TTF_Init() == -1){
		printf("ErrorSDL: %s\n",SDL_GetError());
		printf("ErrorSDL: %s\n",SDL_GetError());
		return true;
	} else {
		mainfont = TTF_OpenFont("font.otf",FONTSIZE);
		bigopfont = TTF_OpenFont("font.otf",BIGOPFONTSIZE);
		if(mainfont == NULL || bigopfont == NULL){
			printf("error, no hay fuente\n");
			exit(1);
		}
		symat->screenSurface = SDL_GetWindowSurface(symat->window);
		SDL_GetWindowSize(symat->window, symat->screenx, symat->screeny);
		SDL_FillRect(symat->screenSurface,NULL,
			SDL_MapRGB(symat->screenSurface->format,255,255,255));
		SDL_UpdateWindowSurface(symat->window);
	}
	return false;
}

void initSymat(Symat * symat, char * config){
	symat->quit = false;
	symat->movex = malloc(sizeof(int));
	symat->prevhmax = malloc(sizeof(int));
	*symat->movex = 0;
	*symat->prevhmax = 0;
	symat->updatewin = true;
	symat->buffer = malloc(sizeof(Buffer) * 16);
	for(int i = 0 ; i < 16; i++){
		 symat->buffer[i] = initBuffer();
	}
	symat->conf = malloc(sizeof(Conf));
	symat->conf->bufferpos = 0;
	symat->conf->posx = 0;
	symat->conf->posy = 0;
	symat->numtokens = getNumTokens(config);
	symat->tokens = getTokens(config,symat->numtokens);

}

// El span de y tiene que ser en funcion del ultimo token que este tiene.
void drawBuffer(Symat * symat, Buffer * buffer,float factor,int spanx,int spany,int * movex,int *prevhmax,bool firsttime){
	float dimfactor = (3*factor)/5;
	int charx, chary;
	int *supx = malloc(sizeof(int));
	int *subx = malloc(sizeof(int));
	*supx = 0;
	*subx = 0;
	SDL_Surface * chartest = TTF_RenderUTF8_Solid(mainfont,buffer->tokens[buffer->tokensize-1].token,color);
	charx = chartest->w;
	chary = chartest->h;
	SDL_FreeSurface(chartest);
	drawLine(symat,buffer,buffer->tokens,buffer->tokensize,factor,spanx,spany,buffer->sizey,movex,prevhmax);
	printf("spanx + buffer->sizex = %d\n",spanx + buffer->sizex);
	if(buffer->suptext != NULL)
		drawBuffer(symat,buffer->suptext,dimfactor,spanx+buffer->sizex,spany+((-1)*chary)/6,supx,NULL,false);
	if(buffer->subtext != NULL)
		drawBuffer(symat,buffer->subtext,dimfactor,spanx+buffer->sizex,spany+(5*chary)/6,subx,NULL,false);
	if(*supx > *subx){
		*movex += *supx;
	} else {
		*movex += *subx;
	}
	if(buffer->next != NULL)
		drawBuffer(symat,buffer->next,factor,spanx+*movex,spany,movex,prevhmax,false);
	free(subx);
	free(supx);
}

void drawLine(Symat * symat, Buffer * buffer, Token * tokens, int maxtokens,float factor,int spanx,int spany,int medy,int * movex,int *prevhmax){
	SDL_Texture * dummyTexture;
	SDL_Rect * rect = malloc(sizeof(SDL_Rect));
	int hmax = 0;
	if(prevhmax != NULL)
		hmax = *prevhmax;
	rect->y = spany;
	rect->x = spanx;
	for(int i = 0; i < maxtokens; i++){
		// TODO: Interpretacion de flags
		if(tokens[i].token != NULL){
			if((tokens[i].flags & ISBIGOP) == ISBIGOP){
				SDL_Surface * dummySurface = TTF_RenderUTF8_Solid(bigopfont,tokens[i].token,color);
				rect->w = dummySurface->w * factor;
				rect->y = spany;
				rect->h = dummySurface->h * factor;
				if(rect->h > hmax){
					hmax = rect->h;
				}
				dummyTexture = SDL_CreateTextureFromSurface(symat->renderer,dummySurface);
				SDL_RenderCopy(symat->renderer,dummyTexture,0,rect);
				rect->x = rect->x + dummySurface->w * factor;
				SDL_DestroyTexture(dummyTexture);
				SDL_FreeSurface(dummySurface);
			} else {
				SDL_Surface * dummySurface = TTF_RenderUTF8_Solid(mainfont,tokens[i].token,color);
				if(dummySurface == NULL){
					rect->w = 0;
					rect->y = spany;
					rect->h = 0;
				} else {
					rect->w = dummySurface->w * factor;
					rect->h = dummySurface->h * factor;
					if(hmax > 0){
					rect->y = spany + (hmax-rect->h)/2;
					} else {
					rect->y = spany;
					}
				}
				dummyTexture = SDL_CreateTextureFromSurface(symat->renderer,dummySurface);
				SDL_RenderCopy(symat->renderer,dummyTexture,0,rect);
				if(dummySurface != NULL){
					rect->x = rect->x + dummySurface->w * factor;
				}
				SDL_DestroyTexture(dummyTexture);
				SDL_FreeSurface(dummySurface);
			}
		}
	}
	buffer->sizex = rect->x-spanx;
	if(prevhmax != NULL)
		*prevhmax = hmax;
	if(movex != NULL)
		*movex = rect->x-spanx;
	buffer->sizey = hmax-spany;
	free(rect);
}
