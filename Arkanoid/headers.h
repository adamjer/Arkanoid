#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define DOT_WIDTH		8
#define DOT_HEIGHT		8
#define DOT_VELOCITY	3
#define DOT_VELOC_MODIF	10
#define PADDLE_WIDTH	56
#define PADDLE_HEIGHT	15
#define PADDLE_VELOCITY 7
#define FRAME_BEGINX	0
#define FRAME_BEGINY	0
#define BCKGRNG_BEGINX	12
#define BCKGRND_BEGINY	15
#define FIELD_LEFTX		12
#define FIELD_RIGHTX	402
#define FIELD_TOP		15
#define FIELD_BOTTOM	440
#define FIELD_WIDTH		390
#define FIELD_HEIGHT	465
#define ARCNOID_WIDTH	200
#define ARCNOID_HEIGHT	64
#define ARCNOID_BEGINX  430
#define ARCNOID_BEGINY	0
#define HIGH_SCORE_POSX	438
#define HIGH_SCORE_POSY 200
#define SCORE_POSX		480
#define SCORE_POSY		120
#define SCORE_WIDTH		85
#define HIGH_SCOR_WIDTH	170
#define SCORES_HEIGHT	17
#define SCREEN_FPS		70
#define SCREEN_TICK_PER_FRAME	1000/SCREEN_FPS
#define BLOCK_WIDTH		30
#define BLOCK_HEIGHT	16
#define MAX_NUM_BLOCKS	182
#define ROUND_TEXT_POSX	260
#define ROUND_TEXT_POSY	220
#define BEGIN_LIVES		3
#define MAX_LIVES		5
#define WAIT_TIME		500
#define BLOCK_POINTS	50
#define SCORE_TABLE_SIZE	5
#define ROUND_WON_SCORE	500
#define SCORE_TABLE_POSX	100
#define SCORE_TABLE_POSY	50
#define POINTS_POSX		450
#define MAX_DOT_VELOCITY	(DOT_VELOCITY*1.5)
#define ACCELERATION_DOT	0.0015
#define MAX_BONUS_TABLE_SIZE	5
#define BONUS_POINTS	500
#define BONUS_PROBABILITY	50
#define MAX_NUMBER_OF_DOTS	3

enum BLOCK_KIND{
	GREEN_BLOCK,
	WHITE_BLOCK,
	ORANGE_BLOCK,
	AQUA_BLOCK,
	RED_BLOCK,
	BLUE_BLOCK,
	YELLOW_BLOCK,
	PURPLE_BLOCK,
	SILVER_BLOCK,
	GOLD_BLOCK
};

enum BONUS_KIND{
	STICKY_PADDLE,
	CHANGE_VELOCITY,
	ADDITIONAL_LIFE,
	WIDE_PADDLE,
	TRIPLE_DOT
};

struct DotDataInPaddle{
	bool stickyPaddle, touchingWall, widePaddle, slowMotion, tripleDot;
};

struct ScoreTable{
	char name[32];
	long int points;
};

struct GlobalGameData{
	int numberOfLives, level, numberOfBlocks, numberOfDots;
	long int score;
	ScoreTable scoreTable[SCORE_TABLE_SIZE];
};

struct Block{
	int posX, posY;
	int life, blockKind;
	SDL_Rect collider;
	bool destructible, onScreen;
};

struct Bonus{
	double posX, posY;
	bool onScreen;
	int type;
	SDL_Rect collider;
};

class Texture{
public:
	Texture();
	~Texture();

	bool loadFromFile(SDL_Renderer *renderer, char *text);
	bool loadFromRenderedText(SDL_Renderer *renderer, TTF_Font *font, char *textureText, SDL_Color textColor);
	void free();
	void render(SDL_Renderer *renderer, int x, int y);
	int getWidth();
	int getHeight();

	SDL_Texture* texture;

	int width;
	int height;
};

class Timer{
public:
	Timer();

	void start();
	void stop();
	bool isStarted();
	Uint32 getTicks();

private:
	bool started;

	Uint32 startTicks;
};

class Dot{
public:
	Dot();

	void handleEvent(SDL_Event &event);
	void move(SDL_Rect paddleCollider, DotDataInPaddle &outsideData);
	SDL_Rect move(Block *blocks, GlobalGameData &data);
	void render(SDL_Renderer *renderer);
	void resetDot(SDL_Rect paddleCollider);
	void loseLife(SDL_Rect paddleCollider, GlobalGameData &data, DotDataInPaddle &dataInPaddle, bool &onScreen);
	bool loadTexture(SDL_Renderer *renderer);
	bool isStarted();

	double posX, posY;
	double velocX, velocY, tempVelocX, tempVelocY;
	SDL_Rect collider;
	bool onScreen, started;

private:
	bool accelerating;
	Texture dotTexture;
	Timer bonusTimer;

	void makeMove();
	void accelerate();
	void addPoints(GlobalGameData &data, int blockKind);
	void setVeloc(SDL_Rect paddleCollider, double &velocX);
	bool deleteBlock(Block &block);
	bool frameCollision();
	bool paddleCollision(SDL_Rect paddleCollider, bool &stickyPaddle);
	bool checkPointInBlock(int x, int y, SDL_Rect blockCollider);
};

class Paddle{
public:
	Paddle();

	void reset(SDL_Renderer *renderer);
	void handleEvent(SDL_Event &event);
	void move(Dot &dot);
	void bonusCollision(Bonus table[], GlobalGameData &data);
	void render(SDL_Renderer *renderer);
	bool loadTexture(SDL_Renderer *renderer);
	SDL_Rect getCollider();
	DotDataInPaddle outsideData;

private:
	bool loadedLongPaddle;
	double posX, posY;
	double velocX, velocY;
	Timer bonusTimer;
	Texture paddleTexture;
	SDL_Rect collider;
};

class BlockBoard{
public:
	BlockBoard(SDL_Renderer *renderer);

	void drawBoard(SDL_Renderer *renderer);
	int initBoard(int level);
	bool loadTexture(SDL_Renderer *renderer);
	Block *getBlocks();

private:
	Block blocks[MAX_NUM_BLOCKS];
	Texture greenBlock, whiteBlock, blueBlock, purpleBlock, orangeBlock,
		yellowBlock, redBlock, silverBlock, goldBlock, aquaBlock;

	void freeBoard();
};

class BonusTable{
public:
	BonusTable(SDL_Renderer *renderer);

	void reset();
	void drawBonuses(SDL_Renderer *renderer);
	void createBonus(SDL_Rect bonusPosition, DotDataInPaddle data);
	void createSingleBonus(SDL_Rect bonusPosition, int index);
	Bonus *getBonusTable();

private:
	double velocY;
	Bonus bonusTable[MAX_BONUS_TABLE_SIZE];
	Texture stickyPaddleTexture, tripleDotTexture, additionalLifeTexture,
		widePaddleTexture, changeVelocityTexture;
};

bool init(SDL_Window **window, SDL_Renderer **renderer);

bool loadMainImages(SDL_Renderer *renderer, Texture &backgroungTexture, Texture &frameTexture, Texture &arcanoidTexture, 
	Texture &lifeTexture);

void close(SDL_Window *window, SDL_Renderer *renderer);

void displayRoundText(SDL_Renderer *renderer, TTF_Font *font, int &level);

bool loadFont(TTF_Font **font);

bool gameWonLost(SDL_Renderer *renderer, TTF_Font *font, int &level, char *text);

void initGlobalData(GlobalGameData &data);

void drawMainImages(SDL_Renderer *renderer, Texture &backgroundTexture, Texture &frameTexture, Texture &arcanoidTexture);

void drawLives(SDL_Renderer *renderer, Texture &lifeTexture, int lives);

void resetGlobalData(GlobalGameData &data);

bool checkEndGame(SDL_Renderer *renderer, TTF_Font *roundFont, GlobalGameData &globalData, Paddle &paddle, Dot dotTable[], 
	BlockBoard &blockBoard, BonusTable &bonuses);

bool loadTextTextures(SDL_Renderer *renderer, TTF_Font *roundFontTexture, Texture &presentScoreTexture, Texture &bestScoreTexture);

void drawTextTextures(SDL_Renderer *renderer, Texture &presentScoreTexture, Texture &bestScoreTexture);

void drawScore(SDL_Renderer *renderer, TTF_Font *font, long int score, long int best);

void drawScoreTable(SDL_Renderer *renderer, TTF_Font *font, ScoreTable table[], long int score);

int addScoreToTable(SDL_Renderer *renderer, TTF_Font *font, ScoreTable table[], long int score);

void insertToTable(ScoreTable table[], long int score, int index, char *text);

void saveTable(ScoreTable table[]);

void loadTable(ScoreTable scoreTable[]);

void createTripleDots(Dot dotTable[]);

void loseLife(Dot dotTable[], GlobalGameData &data, Paddle &paddle, BonusTable &bonusTable);