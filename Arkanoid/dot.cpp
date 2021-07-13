#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<stdio.h>
#include<math.h>
#include"headers.h"

/*
class Dot{
public:
Dot();

void handleEvent(SDL_Event &event);
void move();
void render(SDL_Renderer *renderer, Texture dotTexture);
int getDirection();
bool isStarted();

private:
int posX, posY;
int velocX, velocY;
bool started;

Texture dotTexture;
SDL_Rect collider;
};
*/

Dot::Dot(){
	posX = FIELD_WIDTH / 2 - DOT_WIDTH / 2;
	posY = FIELD_BOTTOM - DOT_WIDTH;

	collider.w = DOT_WIDTH;
	collider.h = DOT_HEIGHT;

	tempVelocX = DOT_VELOCITY;
	tempVelocY = -DOT_VELOCITY;
	velocX = 0;
	velocY = 0;

	started = false;
	accelerating = false;
	onScreen = false;
}


void Dot::resetDot(SDL_Rect paddleCollider){
	posX = (float)(paddleCollider.x + paddleCollider.w / 2 - collider.w / 2);
	posY = FIELD_BOTTOM - DOT_WIDTH; 
	
	velocX = 0;
	velocY = 0;
	tempVelocX = DOT_VELOCITY;
	tempVelocY = -DOT_VELOCITY;

	started = false;
	bonusTimer.stop();
}


void Dot::handleEvent(SDL_Event &event){
	if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && !started){
		switch (event.key.keysym.sym){
		case SDLK_LEFT:
			velocX -= PADDLE_VELOCITY;
			break;
		case SDLK_RIGHT:
			velocX += PADDLE_VELOCITY;
			break;
		case SDLK_s:
			velocX = tempVelocX;
			velocY = tempVelocY;
			started = true;
			accelerating = true;
			break;
		}
	}
	else if (event.type == SDL_KEYUP && event.key.repeat == 0 && !started){
		switch (event.key.keysym.sym){
		case SDLK_LEFT:
			velocX += PADDLE_VELOCITY;
			break;
		case SDLK_RIGHT:
			velocX -= PADDLE_VELOCITY;
			break;
		case SDLK_s:
			break;
		}
	}
}


void Dot::accelerate(){
	if (accelerating){
		int sign = 1;
		if (velocY < 0){
			sign = -1;
			velocY *= sign;
		}
			velocY += ACCELERATION_DOT;
			if (velocY > MAX_DOT_VELOCITY)
				velocY -= ACCELERATION_DOT;
		velocY *= sign;
	}
}


void Dot::move(SDL_Rect paddleCollider, DotDataInPaddle &outsideData){
	if (started){
		makeMove();
		if (!frameCollision()){
			paddleCollision(paddleCollider, outsideData.stickyPaddle);
		}	
		if (!outsideData.slowMotion)
			accelerate();
		else{
			if (velocY > 0)
				velocY = DOT_VELOCITY / 1.5;
			else
				velocY = -(DOT_VELOCITY / 1.5);
			if (velocX > 0)
				velocX = DOT_VELOCITY / 1.5;
			else
				velocX = -(DOT_VELOCITY / 1.5);
			if (!bonusTimer.isStarted()){
				bonusTimer.start();
			}
			if (bonusTimer.getTicks() >= 20){
				outsideData.slowMotion = false;
				if (velocY > 0)
					velocY = DOT_VELOCITY;
				else
					velocY = -DOT_VELOCITY;
				if (velocX > 0)
					velocX = DOT_VELOCITY;
				else
					velocX = -DOT_VELOCITY;
				bonusTimer.stop();
			}
		}
	}
}


bool Dot::deleteBlock(Block &block){
	block.life--;
	if (block.life == 0){
		block.onScreen = false;
		return true;
	}
	return false;
}


SDL_Rect Dot::move(Block *blockBoard, GlobalGameData &data){
	int leftDot;
	int rightDot;
	int topDot;
	int bottomDot;
	bool top, bottom,left, right;
	SDL_Rect bonusPosition;
	bonusPosition.x = 0;

	leftDot = (int)posX;
	rightDot = (int)posX + collider.w;
	topDot = (int)posY;
	bottomDot = (int)posY + collider.w;
	if (started){
		for (int i = 0; i < MAX_NUM_BLOCKS; i++){
			if (blockBoard[i].onScreen){
				top = bottom = left = right = false;
				//top
				if (checkPointInBlock(leftDot + collider.w / 2, topDot, blockBoard[i].collider)){
					top = true;
				}
				//bottom
				if (checkPointInBlock(leftDot + collider.w / 2, bottomDot, blockBoard[i].collider)){
					bottom = true;
				}
				//right
				if (checkPointInBlock(rightDot, topDot + collider.h / 2, blockBoard[i].collider)){
					right = true;
				}
				//left
				if (checkPointInBlock(leftDot, topDot + collider.h / 2, blockBoard[i].collider)){
					left = true;
				}

				if (top || bottom || right || left){
					if (blockBoard[i].destructible){
						if (deleteBlock(blockBoard[i])){
							bonusPosition = blockBoard[i].collider;
							addPoints(data, blockBoard[i].blockKind);
							data.numberOfBlocks--;
						}
					}
					if (top){
						velocY = (abs(velocY));
						posY += velocY;
					}
					else if (bottom){
						velocY = -abs(velocY);
						posY += velocY;
					}
					if (right){
						velocX = -abs(velocX);
						posX += velocX;
					}
					else if (left){
						velocX = (abs(velocX));
						posX += velocX;
					}
					return bonusPosition;
				}
			}
		}
	}
	return bonusPosition;
}


void Dot::addPoints(GlobalGameData &data, int blockKind){
	switch (blockKind){
	case GREEN_BLOCK:
		data.score += (GREEN_BLOCK + 1)*BLOCK_POINTS;
		break;
	case WHITE_BLOCK:
		data.score += (WHITE_BLOCK + 1)*BLOCK_POINTS;
		break;
	case BLUE_BLOCK:
		data.score += (BLUE_BLOCK + 1)*BLOCK_POINTS;
		break;
	case PURPLE_BLOCK:
		data.score += (PURPLE_BLOCK + 1)*BLOCK_POINTS;
		break;
	case ORANGE_BLOCK:
		data.score += (ORANGE_BLOCK + 1)*BLOCK_POINTS;
		break;
	case SILVER_BLOCK:
		data.score += (SILVER_BLOCK + 1)*BLOCK_POINTS;
		break;
	case AQUA_BLOCK:
		data.score += (AQUA_BLOCK + 1)*BLOCK_POINTS;
		break;
	case YELLOW_BLOCK:
		data.score += (YELLOW_BLOCK + 1)*BLOCK_POINTS;
		break;
	}
}


bool Dot::checkPointInBlock(int x, int y, SDL_Rect blockCollider){
	if ((x >= blockCollider.x) && (x <= blockCollider.x + blockCollider.w)
		&& (y >= blockCollider.y) && y <= blockCollider.y + blockCollider.h)
		return true;
	return false;
}


bool Dot::paddleCollision(SDL_Rect paddleCollider, bool &stickyPaddle){
	int leftDot, leftPaddle;
	int rightDot, rightPaddle;
	int topDot, topPaddle;
	int bottomDot, bottomPaddle;

	int dotCenter, paddleCenter;
	bool success = false;

	leftDot = (int)posX;
	rightDot = (int)posX + collider.w;
	topDot = (int)posY;
	bottomDot = (int)posY + collider.w;

	leftPaddle = paddleCollider.x;
	rightPaddle = paddleCollider.x + paddleCollider.w;
	topPaddle = paddleCollider.y;
	bottomPaddle = paddleCollider.y + paddleCollider.h;

	dotCenter = collider.x + collider.w / 2;
	paddleCenter = paddleCollider.x + paddleCollider.w / 2;

	if (bottomDot >= topPaddle && bottomDot <= bottomPaddle - paddleCollider.h / 2 && rightDot >= leftPaddle && leftDot <= rightPaddle){
		if (paddleCollider.w > PADDLE_WIDTH)
			velocX = ((dotCenter - paddleCenter)/2);
		else
			velocX = ((dotCenter - paddleCenter));
		setVeloc(paddleCollider, velocX);
		velocY = -(abs(velocY));

		if (stickyPaddle){
			tempVelocX = velocX;
			tempVelocY = velocY;
			velocX = 0;
			velocY = 0;
			started = false;
			stickyPaddle = false;
			posY = FIELD_BOTTOM - DOT_WIDTH;
		}
		success = true;
	}
	return success;
}


void Dot::loseLife(SDL_Rect paddleCollider, GlobalGameData &data, DotDataInPaddle &dataInPaddle, bool &onScreen){
	if (posY >= SCREEN_HEIGHT - DOT_HEIGHT){
		data.numberOfDots--;
		if (data.numberOfDots == 0){
			resetDot(paddleCollider);
			data.numberOfLives--;
			SDL_Delay(WAIT_TIME);
			data.numberOfDots = 1;
			dataInPaddle.tripleDot = false;
		}
	}
}


void Dot::makeMove(){
	if (started){
		posX += velocX;
		collider.x = (int)posX;
		posY += velocY;
		collider.y = (int)posY;
	}
}


void Dot::render(SDL_Renderer *renderer){
	dotTexture.render(renderer, (int)posX, (int)posY);
}


bool Dot::loadTexture(SDL_Renderer *renderer){
	if (!dotTexture.loadFromFile(renderer, "Grafika/dot.png")){
		printf("Failed to load dot texture!\n");
		return false;
	}
	return true;
}


bool Dot::isStarted(){
	return started;
}


void Dot::setVeloc(SDL_Rect paddleCollider, double &velocX){
	int sign = 1;
	if (velocX < 0){
		velocX *= -1;
		sign = -1;
	}
	double velocDiameter = paddleCollider.w / 16;
	if (velocX >= 7*velocDiameter)
		velocX = DOT_VELOCITY;
	else if (velocX >= 6*velocDiameter)
		velocX = DOT_VELOCITY/1.4;
	else if (velocX >= 5*velocDiameter)
		velocX = DOT_VELOCITY/1.6;
	else if (velocX >= 4*velocDiameter)
		velocX = DOT_VELOCITY/1.9;
	else if (velocX >= 3*velocDiameter)
		velocX = DOT_VELOCITY/2.2;
	else if (velocX >= 2*velocDiameter)
		velocX = DOT_VELOCITY/2.5;
	else if (velocX >= velocDiameter)
		velocX = DOT_VELOCITY/2.8;
	else if (velocX >= 0)
		velocX = DOT_VELOCITY/3.3;
	velocX *= sign;
}


bool Dot::frameCollision(){
	bool success = false;
	//collision with right wall
	if (collider.x>FIELD_RIGHTX - collider.w){
		velocX = -(abs(velocX));
		success = true;
	}
	//collision with left wall
	else if (collider.x<FIELD_LEFTX){
		velocX = abs(velocX);
		success = true;
	}
	//collision with ceiling
	if (collider.y<FIELD_TOP){
		velocY = abs(velocY);
		success = true;
	}
	return success;
}
