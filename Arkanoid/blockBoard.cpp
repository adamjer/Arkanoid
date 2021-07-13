#define _CRT_SECURE_NO_WARNINGS

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"headers.h"

/*
struct Block{
	int posX, posY;
	int life, blockKind;
	SDL_Rect collider;
	bool destructible, onScreen;
};

class BlockBoard{
public:
	BlockBoard(SDL_Renderer *renderer);
	void drawBoard(SDL_Renderer *renderer);
	int initBoard(int level);
	bool loadTexture(SDL_Renderer *renderer);
	void freeBoard();

private:
	Block blocks[MAX_NUM_BLOCKS];
	Texture greenBlock, whiteBlock, blueBlock, purpleBlock, orangeBlock,
		yellowBlock, redBlock, silverBlock, goldBlock, aquaBlock;
};
*/

BlockBoard::BlockBoard(SDL_Renderer *renderer){
	greenBlock.loadFromFile(renderer, "Grafika/Bloczki/greenBlock.png");
	whiteBlock.loadFromFile(renderer, "Grafika/Bloczki/whiteBlock.png");
	blueBlock.loadFromFile(renderer, "Grafika/Bloczki/blueBlock.png");
	purpleBlock.loadFromFile(renderer, "Grafika/Bloczki/purpleBlock.png");
	orangeBlock.loadFromFile(renderer, "Grafika/Bloczki/orangeBlock.png");
	yellowBlock.loadFromFile(renderer, "Grafika/Bloczki/yellowBlock.png");
	redBlock.loadFromFile(renderer, "Grafika/Bloczki/redBlock.png");
	silverBlock.loadFromFile(renderer, "Grafika/Bloczki/silverBlock.png");
	goldBlock.loadFromFile(renderer, "Grafika/Bloczki/goldBlock.png");
	aquaBlock.loadFromFile(renderer, "Grafika/Bloczki/aquaBlock.png");
}


void BlockBoard::freeBoard(){
	for (int i = 0; i < MAX_NUM_BLOCKS; i++){
		blocks[i].onScreen = false;
	}
}


Block *BlockBoard::getBlocks(){
	return blocks;
}


void BlockBoard::drawBoard(SDL_Renderer *renderer){
	for (int i = 0; i < MAX_NUM_BLOCKS; i++){
		if (blocks[i].onScreen){
			switch (blocks[i].blockKind){
			case GREEN_BLOCK:
				greenBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			case WHITE_BLOCK:
				whiteBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			case ORANGE_BLOCK:
				orangeBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			case AQUA_BLOCK:
				aquaBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			case RED_BLOCK:
				redBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			case BLUE_BLOCK:
				blueBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			case YELLOW_BLOCK:
				yellowBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			case PURPLE_BLOCK:
				purpleBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			case SILVER_BLOCK:
				silverBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			case GOLD_BLOCK:
				goldBlock.render(renderer, blocks[i].posX, blocks[i].posY);
				break;
			}
		}
	}
}


int BlockBoard::initBoard(int level){
	FILE *file;
	char buffer[10];
	char directory[64] = "Level/";
	int counterAll = 0, counterHitableBlocks = 0;

	_itoa_s(level, buffer, 10);
	strcat(directory, buffer);
	strcat(directory, ".txt");
	file = fopen(directory, "rb");
	if (file == NULL){
//		printf("Unable to load board file!\n");
		return -1;
	}
	freeBoard();

	int counterX = 0, counterY = 0, beginPosX = FIELD_LEFTX, beginPosY = FIELD_TOP;
	char c;
	while (feof(file) == 0 && counterAll < MAX_NUM_BLOCKS){
		c = getc(file);
		if (counterX == FIELD_WIDTH){
			counterY += BLOCK_HEIGHT;;
			counterX = 0;
		}

		if (c >= '0' && c <= '9'){
			blocks[counterAll].onScreen = true;
			blocks[counterAll].blockKind = c - 48;
			blocks[counterAll].posX = beginPosX + counterX;
			blocks[counterAll].posY = beginPosY + counterY;
			blocks[counterAll].life = 1;
			blocks[counterAll].destructible = true;
			blocks[counterAll].collider.x = blocks[counterAll].posX;
			blocks[counterAll].collider.y = blocks[counterAll].posY;
			blocks[counterAll].collider.w = BLOCK_WIDTH;
			blocks[counterAll].collider.h = BLOCK_HEIGHT;
			if (c - 48 == SILVER_BLOCK){
				blocks[counterAll].life = 2;
			}
			else if (c - 48 == GOLD_BLOCK){
				blocks[counterAll].destructible = false;
				counterHitableBlocks--;
			}
			counterHitableBlocks++;
			counterAll++;
		}
		if (c != '\n' && c != '\r')
			counterX += BLOCK_WIDTH;
	}
	fclose(file);
	return counterHitableBlocks;
}