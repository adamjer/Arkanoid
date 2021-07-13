#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<stdio.h>
#include"headers.h"

/*
class Texture{
public:
Texture();
~Texture();

bool loadFromFile(SDL_Renderer *renderer, char *text);
bool loadFromRenderedText(SDL_Renderer *renderer, TTF_Font *font, char *textureText, SDL_Color textColor);
void free();
void setColor(Uint8 red, Uint8 green, Uint8 blue);
void setBlendMode(SDL_BlendMode blending);
void setAlpha(Uint8 alpha);
void render(SDL_Renderer *renderer, int x, int y);
int getWidth();
int getHeight();

private:
SDL_Texture* texture;

int width;
int height;
};
*/

Texture::Texture(){
	texture = NULL;
	width = 0;
	height = 0;
}


Texture::~Texture(){
	free();
}


bool Texture::loadFromFile(SDL_Renderer *renderer, char *text){
	free();

	SDL_Surface* loadedSurface = IMG_Load(text);

	if (loadedSurface == NULL){
		printf("Unable to load image %s! SDL_image Error: %s\n", text, IMG_GetError());
	}
	else{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

		if (texture == NULL){
			printf("Unable to create texture from %s! SDL Error: %s\n", text, SDL_GetError());
		}
		else{
			width = loadedSurface->w;
			height = loadedSurface->h;
		}
		SDL_FreeSurface(loadedSurface);
	}

	return texture != NULL;
}


bool Texture::loadFromRenderedText(SDL_Renderer *renderer, TTF_Font *font, char *textureText, SDL_Color textColor){
	free();

	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText, textColor);
	if (textSurface != NULL){
		texture = SDL_CreateTextureFromSurface(renderer, textSurface);

		if (texture == NULL){
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else{
			width = textSurface->w;
			height = textSurface->h;
		}
		SDL_FreeSurface(textSurface);
	}
	else{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}

	return texture != NULL;
}


void Texture::free(){
	if (texture != NULL){
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
	}
}


void Texture::render(SDL_Renderer *renderer, int x, int y){
	SDL_Rect renderSpot;
	renderSpot.x = x;
	renderSpot.y = y;
	renderSpot.h = height;
	renderSpot.w = width;

	SDL_RenderCopy(renderer, texture, NULL, &renderSpot);
}



int Texture::getWidth(){
	return width;
}


int Texture::getHeight(){
	return height;
}