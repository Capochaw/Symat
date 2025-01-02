#include "inc.h"
#include <stdio.h>

Symat symat;

int main(int argc, char * argv[]){
	if(GetErrors(&symat,argv[1])){
		return 1;
	}
	Token * toprint = malloc(sizeof(Token) * 16);

    SDL_Event e;
    SDL_StartTextInput();

	while(!symat.quit){
        while(SDL_PollEvent(&e)){
            switch (e.type){
				case SDL_QUIT:
					symat.quit = true;
					break;
				case SDL_TEXTINPUT:
					strcat(symat.buffer[0].parsetext,e.text.text);
					break;
			}
			if(e.key.repeat == 1 || e.type == SDL_KEYDOWN){
				switch(e.key.keysym.sym){
					case SDLK_ESCAPE:
						strcpy(symat.buffer[0].parsetext,"");
						break;
					case SDLK_BACKSPACE:
						removeLastCharacter(symat.buffer[0].parsetext);
						break;
				}
			}
		}

		//strcpy(symat.buffer[0].parsetext,"sum_k = 0^^^n__ f(k)");
		printf("%s\n",symat.buffer[0].parsetext);
		readTokens(&symat,&symat.buffer[0]);
			SDL_SetRenderDrawColor(symat.renderer,255,255,255,255);
			SDL_RenderClear(symat.renderer);
			*symat.movex = 0;
			*symat.prevhmax = 0;
			drawBuffer(&symat,&symat.buffer[0],1,100,100,symat.movex,symat.prevhmax,true);
			SDL_RenderPresent(symat.renderer);
		SDL_Delay(60);
	}
	return 0;
}
