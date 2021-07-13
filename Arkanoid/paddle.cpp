#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<stdio.h>
#include"headers.h"
/*
class Paddle{
public:
Paddle();

void handleEvent(SDL_Event &event);
void move();
void render(SDL_Renderer *renderer, Texture paddleTexture);
bool loadTexture(SDL_Renderer *renderer);

private:
int posX, posY;
int velocX, velocY;

Texture paddleTexture;
SDL_Rect collider;
};
*/

Paddle::Paddle(){
	posX = FIELD_WIDTH / 2 - PADDLE_WIDTH / 2;
	posY = FIELD_BOTTOM;

	collider.y = FIELD_BOTTOM;
	collider.w = PADDLE_WIDTH;
	collider.h = PADDLE_HEIGHT;

	velocX = 0;
	velocY = 0;

	outsideData.stickyPaddle = false;
	outsideData.touchingWall = false;
	outsideData.slowMotion = false;
	outsideData.tripleDot = false;
	outsideData.widePaddle = false;
	loadedLongPaddle = false;
}


void Paddle::reset(SDL_Renderer *renderer){
	posX = FIELD_WIDTH / 2 - PADDLE_WIDTH / 2;
	posY = FIELD_BOTTOM;

	collider.x = posX;

	velocX = 0;
	velocY = 0;

	paddleTexture.loadFromFile(renderer, "Grafika/paddle.png");
	collider.w = paddleTexture.getWidth();

	outsideData.stickyPaddle = false;
	outsideData.touchingWall = false;
	outsideData.slowMotion = false;
	outsideData.tripleDot = false;
	outsideData.widePaddle = false;
	loadedLongPaddle = false;
	bonusTimer.stop();
}


void Paddle::handleEvent(SDL_Event &event){
	if (event.type == SDL_KEYDOWN && event.key.repeat == 0){
		switch (event.key.keysym.sym){
		case SDLK_LEFT:
			velocX -= PADDLE_VELOCITY;
			break;
		case SDLK_RIGHT:
			velocX += PADDLE_VELOCITY;
			break;
		}
	}
	else if (event.type == SDL_KEYUP && event.key.repeat == 0){
		switch (event.key.keysym.sym){
		case SDLK_LEFT:
			velocX += PADDLE_VELOCITY;
			break;
		case SDLK_RIGHT:
			velocX -= PADDLE_VELOCITY;
			break;
		}
	}
}


void Paddle::move(Dot &dot){
	posX += velocX;
	collider.x = (int)posX;

	if (collider.x<FIELD_LEFTX || collider.x>(FIELD_RIGHTX - collider.w)){
		posX -= velocX;
		collider.x = (int)posX;
		outsideData.touchingWall = true;
	}
	else{
		outsideData.touchingWall = false;
		if (!dot.isStarted()){
			dot.posX += velocX;
			dot.collider.x = (int)dot.posX;
		}
	}
}


void Paddle::render(SDL_Renderer *renderer){
	if (outsideData.widePaddle){
		if (!loadedLongPaddle){
			paddleTexture.loadFromFile(renderer, "Grafika/longPaddle.png");
			bonusTimer.start();
			loadedLongPaddle = true;
		}
		collider.w = paddleTexture.getWidth();
		if ((bonusTimer.getTicks()) >= 20 || outsideData.stickyPaddle){
			outsideData.widePaddle = false;
			paddleTexture.loadFromFile(renderer, "Grafika/paddle.png");
			collider.w = paddleTexture.getWidth();
			bonusTimer.stop();
			loadedLongPaddle = false;
		}
	}
	paddleTexture.render(renderer, (int)posX, (int)posY);
}


bool Paddle::loadTexture(SDL_Renderer *renderer){
	if (!paddleTexture.loadFromFile(renderer, "Grafika/paddle.png")){
		printf("Failed to load paddle texture!\n");
		return false;
	}
	return true;
}


SDL_Rect Paddle::getCollider(){
	return collider;
}


void Paddle::bonusCollision(Bonus table[], GlobalGameData &data){
	for (int i = 0; i < MAX_BONUS_TABLE_SIZE; i++){
		if (table[i].onScreen){
			int topBonus = table[i].collider.y;
			int bottomBonus = table[i].collider.y + table[i].collider.h;
			int leftBonus = table[i].collider.x;
			int rightBonus = table[i].collider.x + table[i].collider.w;
			if (((rightBonus>collider.x && leftBonus<collider.x + collider.w)) && 
				topBonus < collider.y + collider.h && bottomBonus > collider.y){
				if (i == STICKY_PADDLE){
					outsideData.stickyPaddle = true;
					data.score += (STICKY_PADDLE + 1)*BONUS_POINTS;
				}
				else if (i == ADDITIONAL_LIFE){
					if (data.numberOfLives < MAX_LIVES){
						data.numberOfLives++;
					}
					data.score += ADDITIONAL_LIFE*BONUS_POINTS;
				}
				else if (i == CHANGE_VELOCITY){
					outsideData.slowMotion = true;
					data.score += CHANGE_VELOCITY*BONUS_POINTS;
				}
				else if (i == TRIPLE_DOT){
					outsideData.tripleDot = true;
					outsideData.stickyPaddle = false;
					data.score += TRIPLE_DOT*BONUS_POINTS;
				}
				else if (i == WIDE_PADDLE){
					outsideData.widePaddle = true;
					outsideData.stickyPaddle = false;
					data.score += WIDE_PADDLE*BONUS_POINTS;
				}
				table[i].onScreen = false;
			}
		}
	}
}
