//Kierowa³em siê wskazówkami zawartymi na stronie www.lazyfoo.net
#define _CRT_SECURE_NO_WARNINGS

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"headers.h"


int main(int argc, char *argv[]){
	double frameTicks;
	GlobalGameData globalData;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	TTF_Font *roundFont = NULL;
	Texture backgroundTexture, frameTexture, arcanoidTexture, lifeTexture;
	Texture presentScoreTexture, bestScoreTexture;
	Dot dot[MAX_NUMBER_OF_DOTS];
	Paddle paddle;
	Timer capTimer;

	if (!init(&window, &renderer)){
		printf("Failed to initialize!\n");
	}
	else{
		if ((!loadMainImages(renderer, backgroundTexture, frameTexture, arcanoidTexture, lifeTexture)) || 
			(!dot[0].loadTexture(renderer)) || (!dot[1].loadTexture(renderer)) || (!dot[2].loadTexture(renderer)) || 
			(!paddle.loadTexture(renderer)) || (!loadFont(&roundFont)) ||
			(!loadTextTextures(renderer, roundFont, presentScoreTexture, bestScoreTexture))){
			printf("Failed to load media!\n");
		}
		else{
			bool quit = false;
			SDL_Event event;
			BlockBoard blockBoard(renderer);
			BonusTable bonuses(renderer);
			SDL_Rect bonusPosition;
			initGlobalData(globalData);
			dot[0].onScreen = true;

			while (!quit){
				capTimer.start();
				while (SDL_PollEvent(&event) != 0){
					if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE){
						quit = true;
					}
					dot[0].handleEvent(event);
					paddle.handleEvent(event);

				}
				for (int i = 0; i < MAX_NUMBER_OF_DOTS; i++){
					if (dot[i].onScreen){
						dot[i].move(paddle.getCollider(), paddle.outsideData);
						bonusPosition = dot[i].move(blockBoard.getBlocks(), globalData);
					}
				}
				if (paddle.outsideData.tripleDot && globalData.numberOfDots==1){
					createTripleDots(dot);
					globalData.numberOfDots = MAX_NUMBER_OF_DOTS;
				}
				paddle.move(dot[0]);
				loseLife(dot, globalData, paddle, bonuses);
				if (bonusPosition.x != 0){
					bonuses.createBonus(bonusPosition, paddle.outsideData);
				}
				paddle.bonusCollision(bonuses.getBonusTable(), globalData);

				SDL_SetRenderDrawColor(renderer, 0, 0, 120, 255);
				SDL_RenderClear(renderer);
				drawMainImages(renderer, backgroundTexture, frameTexture, arcanoidTexture);
				drawTextTextures(renderer, presentScoreTexture, bestScoreTexture);
				drawScore(renderer, roundFont, globalData.score, globalData.scoreTable[0].points);
				bonuses.drawBonuses(renderer);
				drawLives(renderer, lifeTexture, globalData.numberOfLives);
				for (int i = 0; i < MAX_NUMBER_OF_DOTS; i++){
					if (dot[i].onScreen)
						dot[i].render(renderer);
				}
				paddle.render(renderer);
				blockBoard.drawBoard(renderer);
				SDL_RenderPresent(renderer);

				if (checkEndGame(renderer, roundFont, globalData, paddle, dot, blockBoard, bonuses))
					quit = true;

				frameTicks = capTimer.getTicks();
				if (frameTicks < SCREEN_TICK_PER_FRAME){
					SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
				}
			}
		}
	}
	close(window, renderer);

	return 0;
}


void loseLife(Dot dotTable[], GlobalGameData &data, Paddle &paddle, BonusTable &bonusTable){
	for (int i = 0; i < MAX_NUMBER_OF_DOTS; i++){
		if (dotTable[i].onScreen){
			if (dotTable[i].posY >= SCREEN_HEIGHT - DOT_HEIGHT){
				data.numberOfDots--;
				dotTable[i].onScreen = false;
				if (data.numberOfDots == 0){
					data.numberOfLives--;
					bonusTable.reset();
					dotTable[i].resetDot(paddle.getCollider());
				}
				for (int j = 0; j < MAX_NUMBER_OF_DOTS - 1; j++){

				}
				if (data.numberOfLives > 0){
					data.numberOfDots = 1;
					dotTable[0].onScreen = true;
				}
			}
		}
	}
}


void orderDotTable(Dot dotTable[]){
	for (int i = 0; i < MAX_NUMBER_OF_DOTS; i++){
		if (!dotTable[i].onScreen){
			Dot temp;
			for (int j = i; j < MAX_NUMBER_OF_DOTS-1; j++){
				temp = dotTable[j];
				dotTable[j] = dotTable[j + 1];
				dotTable[j + 1] = temp;
			}
		}
	}
}


void createTripleDots(Dot dotTable[]){
	for (int i = 1; i < MAX_NUMBER_OF_DOTS; i++){
		dotTable[i].onScreen = true;
		dotTable[i].posX = dotTable[0].posX;
		dotTable[i].posY = dotTable[0].posY;
		dotTable[i].collider.x = dotTable[i].posX;
		dotTable[i].collider.y = dotTable[i].posY;
		dotTable[i].velocX = dotTable[0].velocX;
		dotTable[i].velocY = dotTable[0].velocY;
		dotTable[i].started = true;
	}
	dotTable[1].velocX *= -1;
	dotTable[2].velocY *= -1;

}


bool init(SDL_Window **window, SDL_Renderer **renderer){
	int rc;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
		printf("SDL_Init error: %s\n", SDL_GetError());
		return false;
	}
	else{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")){
			printf("Warning: Linear texture filtering not enabled!\n");
		}
//		rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN, &window, &renderer);
		rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, window, renderer);
		if (rc != 0){
			printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		}
		else{
			SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
			SDL_SetWindowTitle(*window, "Arkanoid");
			SDL_ShowCursor(SDL_DISABLE);

			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags)){
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				return false;
			}
			if (TTF_Init() == -1){
				printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
				return false;
			}
		}
	}
	return true;
}


void drawScore(SDL_Renderer *renderer, TTF_Font *font, long int score, long int best){
	Texture text;
	char buffer[32];
	int posX;
	SDL_Color whiteColor = { 255, 255, 255, 0 };
	_itoa_s(score, buffer, 10);
	text.loadFromRenderedText(renderer, font, buffer, whiteColor);
	posX = SCORE_POSX + SCORE_WIDTH / 2 - text.getWidth() / 2;;
	text.render(renderer, posX, SCORE_POSY+26);

	if (score > best)
		_itoa_s(score, buffer, 10);
	else
		_itoa_s(best, buffer, 10);
	text.loadFromRenderedText(renderer, font, buffer, whiteColor);
	posX = HIGH_SCORE_POSX + HIGH_SCOR_WIDTH / 2 - text.getWidth() / 2;
	text.render(renderer, posX, HIGH_SCORE_POSY + 26);

}


void drawTextTextures(SDL_Renderer *renderer, Texture &presentScoreTexture, Texture &bestScoreTexture){
	presentScoreTexture.render(renderer, SCORE_POSX, SCORE_POSY);
	bestScoreTexture.render(renderer, HIGH_SCORE_POSX, HIGH_SCORE_POSY);
}


bool loadTextTextures(SDL_Renderer *renderer, TTF_Font *roundFont, Texture &presentScoreTexture, Texture &bestScoreTexture){
	bool success = true;
	SDL_Color redColor = {200, 0, 0, 1};
	if (!presentScoreTexture.loadFromRenderedText(renderer, roundFont, "SCORE", redColor)){
		printf("Failed to load presentScore texture!\n");
		success = false;
	}
	if (!bestScoreTexture.loadFromRenderedText(renderer, roundFont, "HIGH SCORE", redColor)){
		printf("Failed to load bestScore texture!\n");
		success = false;
	}
	return success;
}


bool checkEndGame(SDL_Renderer *renderer, TTF_Font *roundFont, GlobalGameData &globalData, Paddle &paddle, Dot dotTable[], 
	BlockBoard &blockBoard, BonusTable &bonuses){
	bool success = false;
	if (globalData.numberOfBlocks == -1){
		if (gameWonLost(renderer, roundFont, globalData.level, "GAME WON!\0")){
			globalData.score += (globalData.level)*ROUND_WON_SCORE*2;
			success = true;
		}
		else{
			globalData.score += (globalData.level)*ROUND_WON_SCORE*4;
			drawScoreTable(renderer, roundFont, globalData.scoreTable, globalData.score);
			paddle.reset(renderer);
			dotTable[0].resetDot(paddle.getCollider());
			dotTable[1].onScreen = false;
			dotTable[2].onScreen = false;
			bonuses.reset();
			resetGlobalData(globalData);
			globalData.numberOfBlocks = blockBoard.initBoard(globalData.level);
			if (globalData.level != 1)
				globalData.score += (globalData.level-1)*ROUND_WON_SCORE;
			displayRoundText(renderer, roundFont, globalData.level);
		}
	}
	else if (globalData.numberOfBlocks == 0){
		globalData.numberOfBlocks = blockBoard.initBoard(globalData.level);
		if (globalData.numberOfBlocks != -1){
			paddle.reset(renderer);
			globalData.numberOfDots = 1;
			dotTable[0].resetDot(paddle.getCollider());
			bonuses.reset();
			if(globalData.level != 1) 
				globalData.score += (globalData.level-1)*ROUND_WON_SCORE;
			displayRoundText(renderer, roundFont, globalData.level);
		}
	}
	if (globalData.numberOfLives == 0){
		if (gameWonLost(renderer, roundFont, globalData.level, "GAME LOST!\0")){
			success = true;
		}
		else{
			drawScoreTable(renderer, roundFont, globalData.scoreTable, globalData.score);
			resetGlobalData(globalData);
			paddle.reset(renderer);
			dotTable[0].resetDot(paddle.getCollider());
			dotTable[1].onScreen = false;
			dotTable[2].onScreen = false;
			bonuses.reset();
			globalData.numberOfBlocks = blockBoard.initBoard(globalData.level);
			displayRoundText(renderer, roundFont, globalData.level);
		}
	}
	dotTable[1].onScreen = dotTable[2].onScreen = false;
	if (success)
		drawScoreTable(renderer, roundFont, globalData.scoreTable, globalData.score);
	return success;
}


bool loadMainImages(SDL_Renderer *renderer, Texture &backgroungTexture, Texture &frameTexture, Texture &arcanoidTexture, 
	Texture &lifeTexture){
	bool success = true;
	if (!backgroungTexture.loadFromFile(renderer, "Grafika/background1.png")){
		printf("Failed to load background texture!\n");
		success = false;
	}

	if (!frameTexture.loadFromFile(renderer, "Grafika/frame.png")){
		printf("Failed to load frame texture!\n");
		success = false;
	}

	if (!arcanoidTexture.loadFromFile(renderer, "Grafika/arcanoid.png")){
		printf("Failed to load arcanoid texture!\n");
		success = false;
	}

	if (!lifeTexture.loadFromFile(renderer, "Grafika/lifeImage.png")){
		printf("Failed to load life texture!\n");
		success = false;
	}

	return success;
}


void close(SDL_Window *window, SDL_Renderer *renderer){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}


void displayRoundText(SDL_Renderer *renderer, TTF_Font *font, int &level){
	Texture roundTexture;
	SDL_Color whiteColor = {255, 255, 255, 0};
	char buffer[10];
	char roundText[32] = "ROUND ";
	_itoa_s(level, buffer, 10);
	strcat_s(roundText, buffer);
	roundTexture.loadFromRenderedText(renderer, font, roundText, whiteColor);
	if (level != 1) 
		SDL_Delay(WAIT_TIME);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	roundTexture.render(renderer, ROUND_TEXT_POSX, ROUND_TEXT_POSY);
	SDL_RenderPresent(renderer);
	SDL_Delay(WAIT_TIME);
	roundTexture.free();
	level++;
}


bool loadFont(TTF_Font **font){
	*font = TTF_OpenFont("Czcionka/DOSLIKE.ttf", 16);
	if (*font == NULL){
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
	TTF_SetFontStyle(*font, TTF_STYLE_BOLD);
	return true;
}


bool gameWonLost(SDL_Renderer *renderer, TTF_Font *font, int &level, char *text){
	bool success = false, quit = false;
	Texture gameWonTexture, repeatTexture;
	SDL_Event event;
	SDL_Color whiteColor = { 255, 255, 255, 0 };
	gameWonTexture.loadFromRenderedText(renderer, font, text, whiteColor);
	repeatTexture.loadFromRenderedText(renderer, font, "Want play again? Y/N", whiteColor);

	SDL_Delay(WAIT_TIME);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	gameWonTexture.render(renderer, ROUND_TEXT_POSX-20, ROUND_TEXT_POSY);
	repeatTexture.render(renderer, ROUND_TEXT_POSX-100, ROUND_TEXT_POSY+30);
	SDL_RenderPresent(renderer);

	while (!quit){
		while (SDL_PollEvent(&event) != 0){
			if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE){
				success = true;
				quit = true;
			}
			else if (event.type == SDL_KEYDOWN){
				switch (event.key.keysym.sym){
				case SDLK_y:
					quit = true;
					level = 1;
					break;
				case SDLK_n:
					quit = true;
					success = true;
					break;
				}
			}
		}
	}

	repeatTexture.free();
	gameWonTexture.free();
	return success;
}


void initGlobalData(GlobalGameData &data){
	data.level = 1;
	data.numberOfLives = BEGIN_LIVES;
	data.numberOfBlocks = 0;
	data.score = 0;
	loadTable(data.scoreTable);
	data.numberOfDots = 1;
}


void loadTable(ScoreTable scoreTable[]){
	FILE *file = fopen("scoreTable.txt", "r");
	if (file == NULL){
		printf("Couldn't open score table file!\n");
	}
	else{
		for (int i = 0; i < SCORE_TABLE_SIZE; i++){
			strncpy(scoreTable[i].name, "----", 32);
			scoreTable[i].points = 0;
		}
		for (int i = 0; i < SCORE_TABLE_SIZE && feof(file) == 0; i++){
			fscanf(file, "%s %d", &scoreTable[i].name, &scoreTable[i].points);
		}
	}
	fclose(file);
}


void resetGlobalData(GlobalGameData &data){
	data.level = 1;
	data.numberOfLives = BEGIN_LIVES;
	data.score = 0;
	data.numberOfDots = 1;
}


void drawLives(SDL_Renderer *renderer, Texture &lifeTexture, int lives){
	for (int i = 0; i < lives; i++){
		lifeTexture.render(renderer, (FIELD_LEFTX+(lifeTexture.getWidth()+5)*i)+5, (FIELD_BOTTOM+25));
	}
}


void drawMainImages(SDL_Renderer *renderer, Texture &backgroundTexture, Texture &frameTexture, Texture &arcanoidTexture){
	backgroundTexture.render(renderer, BCKGRNG_BEGINX, BCKGRND_BEGINY);
	frameTexture.render(renderer, FRAME_BEGINX, FRAME_BEGINY);
	arcanoidTexture.render(renderer, ARCNOID_BEGINX, ARCNOID_BEGINY);
}


void drawScoreTable(SDL_Renderer *renderer, TTF_Font *font, ScoreTable table[], long int score){
	Texture text;
	SDL_Color whiteColor = { 255, 255, 255, 0 };
	SDL_Color redColor = { 200, 0, 0, 1 };
	SDL_Color choosenColor;
	char str[32] = "RECORDS", tableText[64], buffer[32];
	int a = -1;
	if (score>table[SCORE_TABLE_SIZE - 1].points){
		a = addScoreToTable(renderer, font, table, score);
	}
	text.loadFromRenderedText(renderer,font, str, whiteColor);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	text.render(renderer, 260, SCORE_TABLE_POSY);
	for (int i = 0; i < SCORE_TABLE_SIZE; i++){
		if (a==i) choosenColor = redColor;
		else choosenColor = whiteColor;
		_itoa_s(i+1, tableText, 10);
		strcat(tableText, ". ");
		strcat(tableText, table[i].name);
		_itoa_s(table[i].points, buffer, 10);
		text.loadFromRenderedText(renderer, font, tableText, choosenColor);
		text.render(renderer, SCORE_TABLE_POSX, SCORE_TABLE_POSY + (i+1) * 25);
		text.loadFromRenderedText(renderer, font, buffer, choosenColor);
		text.render(renderer, POINTS_POSX, SCORE_TABLE_POSY + (i+1) * 25);
	}
	text.loadFromRenderedText(renderer, font, "PRESS ANY KEY...", whiteColor);
	text.render(renderer, 180, 400);
	SDL_RenderPresent(renderer);
	bool quit = false;
	SDL_Event event;
	while (!quit){
		while (SDL_PollEvent(&event) != 0){
			if (event.type == SDL_KEYDOWN){
				quit = true;
			}
		}
	}
	text.free();
	if (a != -1) saveTable(table);
}


int addScoreToTable(SDL_Renderer *renderer, TTF_Font *font, ScoreTable table[], long int score){
	int i, index = 0;
	bool quit = false;
	SDL_Event e;
	SDL_Color whiteColor = { 255, 255, 255, 0 };
	Texture text, newRecord, enterName;
	char string[32] = " ";
	SDL_StartTextInput();
	newRecord.loadFromRenderedText(renderer, font, "NEW RECORD!", whiteColor);
	enterName.loadFromRenderedText(renderer, font, "ENTER THE NAME", whiteColor);
	while (!quit){
		while (SDL_PollEvent(&e) != 0){
			if (e.type == SDL_TEXTINPUT && index < 17){
				if (index == 0) string[0] = '\0'; 
				strcat(string, e.text.text);
				index++;
			}
			else if (e.type == SDL_KEYDOWN){
				if (e.key.keysym.sym == SDLK_BACKSPACE && index > 0){
					string[--index] = '\0';
				}
				else if (e.key.keysym.sym == SDLK_RETURN && index > 0){
					quit = true;
				}
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);
			newRecord.render(renderer, 230, SCORE_TABLE_POSY);
			enterName.render(renderer, 230+newRecord.getWidth()/2-enterName.getWidth()/2, SCORE_TABLE_POSY+30);
			if (index > 0){
				text.loadFromRenderedText(renderer, font, string, whiteColor);
				text.render(renderer, 230 + newRecord.getWidth() / 2 - text.getWidth() / 2, 156);
			}
			SDL_RenderPresent(renderer);
		}
	}
	SDL_StopTextInput();

	for (i = SCORE_TABLE_SIZE-1; i >= 0; i--){
		if (i != 0){
			if (score > table[i].points && score <= table[i - 1].points){
				insertToTable(table, score, i, string);
				break;
			}
		}
		else{
			if (score > table[i].points){
				insertToTable(table, score, i, string);
				break;
			}
		}
	}
	newRecord.free();
	enterName.free();
	text.free();
	return i;
}


void insertToTable(ScoreTable table[], long int score, int index, char *text){
	for (int i = SCORE_TABLE_SIZE-1; i > index; i--){
		table[i] = table[i-1];
	}
	table[index].points = score;
	strncpy(table[index].name, text, 32);
}


void saveTable(ScoreTable table[]){
	FILE *file;
	file = fopen("scoreTable.txt", "w");
	if (file == NULL){
		printf("Couldn't open file!\n");
	}
	else{
		for (int i = 0; i < SCORE_TABLE_SIZE; i++){
			fprintf(file, "%s\t\t%d\n", table[i].name, table[i].points);
		}
	}
	fclose(file);
}