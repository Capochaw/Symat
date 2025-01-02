#include "inc.h"

Buffer * setBuffer(){
	Buffer * ret;
	ret = malloc(sizeof(Buffer));
	ret->parsetext = malloc(sizeof(uint8_t));
	ret->posx = 0;
	ret->posy = 0;
	ret->spanx = 0;
	ret->spany = 0;
	ret->sizex = 0;
	ret->sizey = 0;
	ret->tokensize = 0;
	ret->tokens = NULL;
	ret->subtext = NULL;
	ret->suptext = NULL;
	ret->next = NULL;
	return ret;
}

Buffer initBuffer(){
	Buffer ret;
	ret.parsetext = malloc(sizeof(uint8_t));
	ret.posx = 0;
	ret.posy = 0;
	ret.spanx = 0;
	ret.spany = 0;
	ret.sizex = 0;
	ret.sizey = 0;
	ret.tokensize = 0;
	ret.tokens = NULL;
	ret.subtext = NULL;
	ret.suptext = NULL;
	ret.next = NULL;
	return ret;
}

Token getTokenFlags(char * s,int * pos){
	Token ret;
	bool close = false;
	int i = 0;
	ret.flags = 0;
	while(s[i] != ')'){
		i++;
	}
	while(s[i] != '('){
		switch(s[i]){
			case 'B':
				ret.flags += ISBIGOP;
				break;
			case 'P':
				ret.flags += ISPARENTHESIS;
				if(close){
					ret.flags += CLOSEPAR;
				}
				close = true;
				break;
			case 'F':
				ret.flags += ISFRAC;
				break;
			case 'U':
				ret.flags += MAKESUP;
				break;
			case 'u':
				ret.flags += MAKESUB;
				break;
		}
		i++;
	}
	*pos = i+2;
	return ret;
}

uint8_t * cutParse(uint8_t * source, int i){
	//int set = strlen(source) - (i+1);
	uint8_t * ret = malloc(sizeof(uint8_t) * (strlen(source) -(i)));
	if(ret == NULL)
		return NULL;
	int p = i;
	while(source[p] != '\0'){
		ret[p-i] = source[p];
		p++;
	}
	ret[p-i] = '\0';
	return ret;
}

int getNumTokens(char * config){
	FILE * file = fopen(config,"r");
	char c;
	int numtokens = 0;
	while(!feof(file)){
		c = fgetc(file);
		if(c == '\n'){
			numtokens++;
			continue;
		}
	}
	fclose(file);
	return numtokens;
}
void freeBuffers(Buffer *buffer) {
    if (buffer == NULL) {
        return; // Evita procesar un puntero nulo.
    }

    // Libera los nodos dependientes primero.
    if (buffer->next != NULL) {
        freeBuffers(buffer->next);
    }
    if (buffer->subtext != NULL) {
        freeBuffers(buffer->subtext);
    }
    if (buffer->suptext != NULL) {
        freeBuffers(buffer->suptext);
    }

    // Libera los recursos propios del nodo actual.
    if (buffer->parsetext != NULL) {
        free(buffer->parsetext);
		buffer->parsetext = NULL;
    }
    if (buffer->tokens != NULL) {
        free(buffer->tokens);
		buffer->parsetext = NULL;
    }

    // Finalmente, libera el nodo actual.
    free(buffer);
}

//FIXME: No interpreta bien los cambios de dimensiones
uint8_t * getBbparse(Symat * symat,uint8_t * source, bool issup,int *retpos){
	//si issup, termina cuando encuentra __
	//si ~issup, termina cuando encuentra ^^
	uint8_t * ret = malloc(sizeof(uint8_t));
	bool broke = false;
	int size = 1, move ,dim = 0, cant = 0;
	if(retpos != NULL){
		move = *retpos+1;
	} else {
		move = 1;
	}
	if(issup){
		dim += 1;
		while(source[move+size-1] != '\0' && broke == false){
			ret = realloc(ret,sizeof(uint8_t)*(size+1));
			ret[size-1] = source[move+size-1];
			ret[size] = '\0';
			if(ret[size-1] == '^' && ret[size-1] != ret[size-2]){
				dim +=1;
			}
			if(ret[size-1] == '_' && ret[size-1] == ret[size-2]){
				dim -=1;
				cant++;
			}
			if(ret[size-1] == ret[size-2] && ret[size-1] == '_' && size >= 1 && dim == 0){
				broke = true;
				if(cant%2 == 0){
					ret[size-1] = '\0';
				} else {
					ret[size-2] = '\0';
				}
				if(retpos != NULL)
					*retpos += size+1;
			}
			size += 1;
		}
	} else {
		dim -= 1;
		while(source[move+size-1] != '\0' && broke == false){
			ret = realloc(ret,sizeof(uint8_t)*size);
			ret[size-1] = source[move+size-1];
			if(ret[size-1] == '^' && ret[size-1] == ret[size-2]){
				dim +=1;
				cant++;
			}
			if(ret[size-1] == '_' && ret[size-1] != ret[size-2]){
				dim -=1;
			}
			if(ret[size-1] == ret[size-2] && ret[size-1] == '^' && size >= 1 && dim == 0){
				broke = true;
				if(cant%2 == 0){
					ret[size-1] = '\0';
				} else {
					ret[size-2] = '\0';
				}
				if(retpos != NULL)
					*retpos += size+1;
			}
			size += 1;
		}
	}
	if(cant%2 == 0){
		ret[size-1] = '\0';
	} else {
		ret[size-2] = '\0';
	}
	if(retpos != NULL && !broke)
		*retpos += size+1;
	return ret;
}

// FIXME: Agregar parametros sobre los caracteres.
Token * getTokens(char * config,int numtokens){
	FILE * file = fopen(config,"r");
	uint8_t token[16] = "";
	uint8_t chara[16] = "";
	char num[16] = "";
	int * pos = malloc(sizeof(int)), save = 0;
	char actline[256] = "";
	//int numtokens = 0;
	bool isfirstelement = true, issecondelement = false;
	*pos = 0;

	Token * ret = malloc(sizeof(Token) * numtokens);
	for(int i = 0; i < numtokens; i++){
		fgets(actline,sizeof(actline),file);
		ret[i] = getTokenFlags(actline,pos);
		// ret[i].token[0] = actline[4];
		for(int p = *pos; p < strlen(actline); p++){
			if(isfirstelement){
				if(actline[p] != '"'){
					chara[p-*pos] = actline[p];
					continue;
				} else {
					isfirstelement = false;
					issecondelement = true;
					chara[p-*pos] = '\0';
					p += 2;
					*pos = p+1;
					continue;
				}
			} else if (issecondelement){
				if(actline[p] != '"'){
					token[p-*pos] = actline[p];
					continue;
				} else {
					token[p-*pos] = '\0';
					p += 2;
					*pos = p+1;
					issecondelement = false;
					break;
				}
			} else {
				num[p-*pos] = actline[p];
				save = p - *pos;
			}
		}
		num[save+1] = '\0';
		isfirstelement = true;
		ret[i].chara = malloc(sizeof(chara));
		ret[i].token = malloc(sizeof(token));
		for(int p = 0; p < sizeof(token);p++){
			ret[i].token[p] = token[p];
		}
		for(int p = 0; p < sizeof(chara);p++){
			ret[i].chara[p] = chara[p];
		}
	}
	fclose(file);
	free(pos);
	return ret;
}

bool existToken(uint8_t * set, uint8_t * reset, Token * tokens, int numtokens){
	bool ret = false;
	uint8_t *val = malloc(sizeof(char)*(strlen(set)+strlen(reset)));
	strcpy(val,set);
	strcat(val,reset);
	for(int i = 0; i < numtokens;i++){
		if(strstr(val,tokens[i].chara) != NULL){
			ret = true;
		}
	}
	return ret;
}

Token auxToken(uint8_t * text){
	Token ret;
	ret.flags = 0;
	ret.token = malloc(sizeof(char) * strlen(text));
	ret.chara = malloc(sizeof(char) * strlen(text));
	strcpy(ret.token,text);
	strcpy(ret.chara,text);
	return ret;
}

uint8_t * copyUntilPointer(uint8_t * tocopy, uint8_t * pointer){
	uint8_t * ret = malloc(sizeof(uint8_t) * strlen(tocopy));
	int i = 0;
	while(tocopy[i] != '\0' && (&tocopy[i] != pointer)){
		ret[i] = tocopy[i];
	}
	ret[i] = '\0';
	return ret;
}

//TODO: OPTIMIZAR
int isToken(uint8_t * prev, uint8_t * comp, Token * tokens, int numtokens){
	int ret = -1;
	bool nobreak = true;
	uint8_t * move = malloc(sizeof(uint8_t) * (strlen(comp)+1));
	for(int i = 0 ; i < strlen(comp) && nobreak; i++){
		for(int p = i; p < strlen(comp); p++){
			move[p-i] = comp[p];
		}
		move[strlen(comp)-i] = '\0';
		for(int p = 0; p < numtokens; p++){
			if(strcmp(move,tokens[p].chara) == 0){
				ret = p;
				for(int q = 0; q < strlen(comp)-strlen(move);q++){
					prev[q] = comp[q];
				}
				strcpy(comp,move);
				nobreak = false;
				break;
			}
		}
	}
	free(move);
	return ret;
}

void removeLastCharacter(uint8_t *set) {
    if (set == NULL || *set == '\0') {
        return; // Validación de entrada
    }

    // Obtener la longitud en caracteres UTF-8
    size_t len = u8_strlen(set);
    if (len == 0) {
        return; // Si la cadena está vacía, no hacemos nada
    }

    // Convertir la posición del último carácter a índice de bytes
    uint8_t *start = set;
    uint8_t *last = NULL;
    for (size_t i = 0; i < len; i++) {
        last = start;
        start += u8_mblen(start, strlen((char *)start));
    }

    // Eliminar el último carácter ajustando el terminador nulo
    *last = '\0';
}

float getfactor(char * num){
	float ret = 0;
	for(int i = strlen(num)-1; i >=0 ; i++){
		ret = ret + ((num[i]-'a')*pow(10,i));
	}
	return ret;
}

//TODO: Implementar fracciones
//FIXME: Genera mal parseo
//1 espacio es un token
//2 espacios es un separador de token auxlilar???? (Quitar)
//En una palabra se analiza de adelante a atras. Si encuentra un token, separa la parte del token
//y genera el token auxiliar atras de el token correcto.
void readTokens(Symat * symat, Buffer * dest){
	if(dest != NULL){
		free(dest->tokens);
		dest->tokens = NULL;
		dest->sizex = 0;
		dest->sizey = 0;
		dest->tokensize = 0;
		if(dest->next != NULL)
			freeBuffers(dest->next);
			dest->next = NULL;
		if(dest->subtext != NULL)
			freeBuffers(dest->subtext);
			dest->subtext = NULL;
		if(dest->suptext != NULL)
			freeBuffers(dest->suptext);
			dest->subtext = NULL;
	}
	uint8_t * source = dest->parsetext;
	int p = 0,s = 0,pos = 0;
	bool hastokenprev = false,isaux = false;
	uint8_t prev[64] = "";
	uint8_t comp[64] = "";
	int tokenpos = 0;
	for(int i = 0; source[i] != '\0'; i++){
		comp[p] = source[i];
		comp[p+1] = '\0';
		p++;
		tokenpos = isToken(prev,comp,symat->tokens,symat->numtokens);
		if(tokenpos != -1){
			if((symat->tokens[tokenpos].flags & MAKESUB) == MAKESUB || (symat->tokens[tokenpos].flags & MAKESUP) == MAKESUP){
				if((symat->tokens[tokenpos].flags & MAKESUB) == MAKESUB){
					dest->subtext = setBuffer();
					dest->subtext->parsetext = getBbparse(symat,source,false,&i);
					readTokens(symat,dest->subtext);
					strcpy(prev,"");
					comp[0] = source[i];
					comp[1] = '\0';
					int newtokenpos = isToken(prev,comp,symat->tokens,symat->numtokens);
					if((symat->tokens[newtokenpos].flags & MAKESUP) == MAKESUP){
						dest->suptext = setBuffer();
						dest->suptext->parsetext = getBbparse(symat,source,true,&i);
						if(dest->suptext->parsetext != NULL)
							readTokens(symat,dest->suptext);
					}
				}
				if((symat->tokens[tokenpos].flags & MAKESUP) == MAKESUP){
					dest->subtext = setBuffer();
					dest->subtext->parsetext = getBbparse(symat,source,true,&i);
					readTokens(symat,dest->subtext);
					strcpy(prev,"");
					comp[0] = source[i];
					comp[1] = '\0';
					int newtokenpos = isToken(prev,comp,symat->tokens,symat->numtokens);
					if((symat->tokens[newtokenpos].flags & MAKESUB) == MAKESUB){
						dest->subtext = setBuffer();
						dest->subtext->parsetext = getBbparse(symat,source,false,&i);
						if(dest->subtext->parsetext != NULL)
							readTokens(symat,dest->subtext);
					}
						//FIXME: Eliminar mallocs hechos
				}
				dest->next = setBuffer();
				dest->next->parsetext = cutParse(source,i);
				if(dest->next->parsetext != NULL)
					readTokens(symat,dest->next);
				break;
			}
			if(dest->tokens == NULL){
				dest->tokens = malloc(sizeof(Token));
				dest->tokens[0] = symat->tokens[tokenpos];
				pos+=1;
				dest->tokensize = pos;
				p = 0;
				comp[0] = '\0';
			} else {
				if(strlen(comp) <= 1){
					pos+=1;
					dest->tokens = realloc(dest->tokens,sizeof(Token) * (pos));
				}
				dest->tokens[pos-1] = symat->tokens[tokenpos];
				p = 0;
				comp[0] = '\0';
			}
			hastokenprev = true;
		} else {
			if(dest->tokens == NULL){
				dest->tokens = malloc(sizeof(Token));
				dest->tokens[0] = auxToken(comp);
				pos+=1;
			} else {
				if(hastokenprev){
					pos+=1;
					dest->tokens = realloc(dest->tokens,sizeof(Token) * (pos));
					dest->tokens[pos-1] = auxToken(comp);
					hastokenprev = false;
				} else {
					dest->tokens[pos-1] = auxToken(comp);
				}
				isaux = true;
			}
		}
	dest->tokensize = pos;
	}
}
