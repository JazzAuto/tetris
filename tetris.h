#define A_LARGURA  10  // Arena largura
#define A_ALTURA 20  // Arena altura
#define T_LARGURA  4   // Peca largura
#define T_ALTURA 4   // Peca altura

// gera uma nova peca
void newPeca();

// verifica se a posição é valida
bool validPos(int peca, int rotation, int posX, int posY);

// roda a peca
int rotate(int x, int y, int rotation);

// processa os inputs
void processInputs();

// Move a peca pra baixo
bool moveDown();

// adiciona a peca na arena
void addToArena();

// checa se a linha ta limpa 
void checkLines();

// desenha arena
void drawArena();