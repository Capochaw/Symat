#include "inc.h"

Buffer * setBuffer(){
	Buffer * ret;
	ret = malloc(sizeof(Buffer));
	ret->parsetext = malloc(sizeof(uint8_t));
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
	uint8_t * ret = malloc(sizeof(strlen(source)-(i+1)));
	for(int p = i; p < strlen(source);p++){
		ret[p-i] = source[p];
	}
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

//FIXME: No interpreta bien los cambios de dimensiones
uint8_t * getBbparse(Symat * symat,uint8_t * source, bool issup,int *retpos){
	//si issup, termina cuando encuentra __
	//si ~issup, termina cuando encuentra ^^
	uint8_t * ret = malloc(sizeof(uint8_t));
	bool broke = false;
	int size = 1, move = *retpos+1,dim = 0;
	if(issup){
		dim += 1;
		while(source[move] != '\0' && broke == false){
			ret = realloc(ret,sizeof(uint8_t)*size);
			ret[size-1] = source[move+size-1];
			if(ret[size-1] == '^' && ret[size-1] != ret[size-2]){
				dim +=1;
			}
			if(ret[size-1] == '_' && ret[size-1] != ret[size-2]){
				dim -=1;
			}
			if(ret[size-1] == ret[size-2] && ret[size-1] == '_' && size >= 1 && dim == 0){
				broke = true;
				ret[size-2] = '\0';
				*retpos += size+1;
			}
			size += 1;
		}
	} else {
		dim -= 1;
		while(source[move] != '\0' && broke == false){
			ret = realloc(ret,sizeof(uint8_t)*size);
			ret[size-1] = source[move+size-1];
			if(ret[size-1] == '^' && ret[size-1] != ret[size-2]){
				dim +=1;
			}
			if(ret[size-1] == '_' && ret[size-1] != ret[size-2]){
				dim -=1;
			}
			if(ret[size-1] == ret[size-2] && ret[size-1] == '^' && size >= 1 && dim == 0){
				broke = true;
				ret[size-2] = '\0';
				*retpos += size+1;
			}
			size += 1;
		}
		printf("%s\n",ret);
	}
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

int isToken(const uint8_t * set, const uint8_t * reset, Token * tokens, int numtokens,bool * isonlycomp){
	int pos = -1, step = 0, red = 0;
	uint8_t * val = malloc(sizeof(char) * strlen(set) + sizeof(char) * strlen(reset));
	*isonlycomp = true;
	if(strcmp(set,"") != 0){
		strcpy(val,set);
		strcat(val,reset);
		*isonlycomp = false;
	} else {
		strcpy(val,reset);
	}
	for(int i = 0; i < strlen(val); i++){
		if(val[i] != ' '){
			val[step] = val[i];
			step++;
		} else {
			red -=1;
		}
	}
	val[step + red] = '\0';
	
	for(int i = 0; i < numtokens; i++){
		if(strcmp(val,tokens[i].chara) == 0){
			pos = i;
			break;
		}
	}
	return pos;
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
	uint8_t * source = dest->parsetext;
	int p = 0,s = 0,pos = 0;
	bool isonlycomp = true;
	uint8_t comp[64] = "";
	uint8_t prev[64] = "";
	int tokenpos = 0;
	for(int i = 0; source[i] != '\0'; i++){
		comp[p] = source[i];
		comp[p+1] = '\0';
		tokenpos = isToken(prev,comp,symat->tokens,symat->numtokens,&isonlycomp);
		if(existToken(prev,comp,symat->tokens,symat->numtokens) == true && tokenpos != -1){
			if((symat->tokens[tokenpos].flags & MAKESUP) == MAKESUP){
				dest->suptext = setBuffer();
				dest->suptext->parsetext = getBbparse(symat,source,true,&i);
				readTokens(symat,dest->suptext);
				dest->next = setBuffer();
				dest->next->parsetext = cutParse(source,i);
				//FIXME: Eliminar mallocs hechos
				readTokens(symat,dest->next);
				break;
			}
			if((symat->tokens[tokenpos].flags & MAKESUB) == MAKESUB){
				dest->subtext = setBuffer();
				dest->subtext->parsetext = getBbparse(symat,source,false,&i);
				readTokens(symat,dest->subtext);
				dest->next = setBuffer();
				dest->next->parsetext = cutParse(source,i);
				//FIXME: Eliminar mallocs hechos
				readTokens(symat,dest->next);
				break;
			}
			//FIXME:: No hace bien las asignaciones a tokensize
			if(dest->tokens == NULL){
				dest->tokens = malloc(sizeof(Token));
				dest->tokens[0] = symat->tokens[tokenpos];
				pos+=1;
				dest->tokensize = pos;
				p = -1;
				comp[0] = '\0';
			} else {
				pos+=1;
				dest->tokens = realloc(dest->tokens,sizeof(Token)*(pos+1));
				dest->tokens[pos-1] = symat->tokens[tokenpos];
				dest->tokensize = pos;
				p = -1;
				comp[0] = '\0';
			}
		} else if (strstr(comp, "  ") != NULL){
			if(dest->tokens == NULL){
				dest->tokens = malloc(sizeof(Token));
				removeLastCharacter(comp);
				dest->tokens[0] = auxToken(comp);
				strcpy(comp,"");
				strcpy(prev,"");
				dest->tokensize = pos;
				pos+=1;
				p = -1;
			} else {
				dest->tokens = realloc(dest->tokens,sizeof(Token)*(pos+1));
				pos+=1;
				removeLastCharacter(comp);
				dest->tokens[pos-1] = auxToken(comp);
				dest->tokensize = pos;
				strcpy(comp,"");
				strcpy(prev,"");
				p = -1;
			}
		} else if (strstr(comp, "  ") == NULL){
			if(dest->tokens == NULL){
				dest->tokens = malloc(sizeof(Token));
				dest->tokens[0] = auxToken(comp);
			} else {
				//pos+=1;
				dest->tokens = realloc(dest->tokens,sizeof(Token)*(pos+1));
				dest->tokens[pos] = auxToken(comp);
				dest->tokensize = pos;
			}
		}
		p++;
	}
}

void updateBuffer(Token *dest,int destlen, Buffer *origin, Token * tokens){
	bool istoken = false;
	while(!istoken){
	}
	// *dest es un array de punteros a tokens que existen. Si no existe el token, entonces crear uno con
	// el texto normal. Cuando se reemplaza ese valor de ese token, liberar la memoria y hacer el nuevo token.
}

			/*
			if(isonlycomp){
				printf("%d\n",tokenpos);
				dest->tokens[s] = symat->tokens[tokenpos];
				strcpy(prev,comp);
				strcpy(comp,"");
				s++;
				p = 0;
				continue;
			} else {
				printf("%d\n",tokenpos);
				s -= 1;
				dest->tokens[s] = symat->tokens[tokenpos];
				strcpy(prev,comp);
				strcpy(comp,"");
				s++;
				p = 0;
				continue;
			}
		} else
			removeLastCharacter(comp);
			dest->tokens[s] = auxToken(comp);
			s++;
			strcpy(comp,"");
			strcpy(prev,"");
			p=0;
			continue;
			// strcpy(comp,"");
			// p = 0;
		} else if (strstr(comp,"  ") == NULL) {
			strcpy(prev,"");
			dest->tokens[s] = auxToken(comp);
		*/
