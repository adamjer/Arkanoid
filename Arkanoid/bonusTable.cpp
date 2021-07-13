#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"headers.h"


BonusTable::BonusTable(SDL_Renderer *renderer){
	for (int i = 0; i < MAX_BONUS_TABLE_SIZE; i++){
		bonusTable[i].onScreen = false;
		bonusTable[i].posX = 0;
		bonusTable[i].posY = 0;
		bonusTable[i].type = i;
		bonusTable[i].collider.x = bonusTable[i].posX;
		bonusTable[i].collider.y = bonusTable[i].posY;
		bonusTable[i].collider.w = BLOCK_WIDTH;
		bonusTable[i].collider.h = BLOCK_HEIGHT;
	}
	velocY = DOT_VELOCITY;
	stickyPaddleTexture.loadFromFile(renderer, "Grafika/Bonusy/stickyPaddle.png");
	tripleDotTexture.loadFromFile(renderer, "Grafika/Bonusy/tripleDot.png");
	additionalLifeTexture.loadFromFile(renderer, "Grafika/Bonusy/additionalLife.png");
	widePaddleTexture.loadFromFile(renderer, "Grafika/Bonusy/widePaddle.png");
	changeVelocityTexture.loadFromFile(renderer, "Grafika/Bonusy/changeVelocity.png");
}


void BonusTable::reset(){
	for (int i = 0; i < MAX_BONUS_TABLE_SIZE; i++){
		bonusTable[i].onScreen = false;
	}
}


void BonusTable::drawBonuses(SDL_Renderer *renderer){
	for (int i = 0; i < MAX_BONUS_TABLE_SIZE; i++){
		if (bonusTable[i].onScreen){
			if (i == STICKY_PADDLE)
				stickyPaddleTexture.render(renderer, bonusTable[i].posX, bonusTable[i].posY);
			else if (i == CHANGE_VELOCITY)
				changeVelocityTexture.render(renderer, bonusTable[i].posX, bonusTable[i].posY);
			else if (i == ADDITIONAL_LIFE)
				additionalLifeTexture.render(renderer, bonusTable[i].posX, bonusTable[i].posY);
			else if (i == WIDE_PADDLE)
				widePaddleTexture.render(renderer, bonusTable[i].posX, bonusTable[i].posY);
			else if (i == TRIPLE_DOT)
				tripleDotTexture.render(renderer, bonusTable[i].posX, bonusTable[i].posY);

			bonusTable[i].posY += velocY;
			bonusTable[i].collider.y = bonusTable[i].posY;
			if (bonusTable[i].posY > SCREEN_HEIGHT - DOT_WIDTH)
				bonusTable[i].onScreen = false;
		}
	}
}


void BonusTable::createBonus(SDL_Rect bonusPosition, DotDataInPaddle data){
	srand(time(NULL));
	int i = MAX_BONUS_TABLE_SIZE;
	if ((rand() % 100) < 50){
		while (i--){
			int index = rand() % (MAX_BONUS_TABLE_SIZE);
			if (!bonusTable[index].onScreen){
				if (index == STICKY_PADDLE){
					if (!data.stickyPaddle)
						createSingleBonus(bonusPosition, index);
				}
				else if (index == CHANGE_VELOCITY){
					if (!data.slowMotion)
						createSingleBonus(bonusPosition, index);
				}
				else if (index == WIDE_PADDLE){
					if (!data.widePaddle)
						createSingleBonus(bonusPosition, index);
				}
				else if (index == TRIPLE_DOT){
					if (!data.tripleDot && !data.stickyPaddle)
						createSingleBonus(bonusPosition, index);
				}
				else if (index == ADDITIONAL_LIFE){
					createSingleBonus(bonusPosition, index);
				}
				return;
			}
		}
	}
}


void BonusTable::createSingleBonus(SDL_Rect bonusPosition, int index){
	bonusTable[index].onScreen = true;
	bonusTable[index].posX = bonusPosition.x;
	bonusTable[index].posY = bonusPosition.y;
	bonusTable[index].type = index;
	bonusTable[index].collider.x = bonusTable[index].posX;
	bonusTable[index].collider.y = bonusTable[index].posY;
}


Bonus *BonusTable::getBonusTable(){
	return bonusTable;
}