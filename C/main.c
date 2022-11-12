#include "libs.h"

typedef struct {
	int w, a, s, d, escape;
}botoesTeclas;

botoesTeclas Teclas;

float playerX, playerY, playerDeltaX, playerDeltaY, playerAngulo; //Posição do Jogador em X e Y

void drawPlayer(){
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(playerX, playerY);
	glEnd();

	// --- DEBUG PARA VIZUALIZAÇÃO --- //

	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(playerX, playerY);
	glVertex2i(playerX+playerDeltaX*5, playerY+playerDeltaY*5);
	glEnd(); //Isso é importante, não esqueça dessa função senão tudo quebra
	drawRays2D();
}	

int mapaX = 8, mapaY = 8, mapaT = 64; //Qtd de Unidades no X e Y do mapa, e o Tamanho de cada Unidade em Pixels

int mapa[]={
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 1, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1
	
	//Percebes? 8x8, os 0 é espaço livre, 1 parede
};

void drawMap2D(){
	int x, y, x0, y0;
	for(y = 0; y < mapaY; y++){
		for(x = 0; x < mapaX; x++){
			if(mapa[y * mapaX + x] == 1){ glColor3f(1,1,1); } else { glColor3f(0,0,0); } //se for 1 é branco, 0 preto
			x0 = x * mapaT; y0 = y * mapaT;
			glBegin(GL_QUADS);
			glVertex2i(x0		+1, y0		+1); //Esses +1 e -1 é para deixar uma grid
			glVertex2i(x0		+1, y0 + mapaT	-1); //para cada bloco renderizado
			glVertex2i(x0 + mapaT	-1, y0 + mapaT	-1);
			glVertex2i(x0 + mapaT	-1, y0 		+1);
			glEnd();
		}
	}
}

float distancia(float ax, float ay, float bx, float by, float angulo){
	return ( sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)) );
}

void drawRays2D(){
	int r, mapx, mapy, map, dof;
	float rayx, rayy, raya, xo, yo, disT;

	raya = playerAngulo - DR * 30; if(raya < 0){ raya += 2*PI; } if (raya > 2*PI) { raya -= 2*PI; } //isso é o FOV
	for(r = 0; r < 60; r++){ //quantidade de raios

		// ESSA É A PARTE MAIS IMPORTANTE DO CÓDIGO, ESTUDE-O

		// --- Checa as Linhas Horizontais

		float disH = 1000000, hx = playerX, hy = playerY;

		dof = 0;
		float aTangente = -1/tan(raya); // -1 / (sin(raya) / cos(raya))
		//está olhando pra cima (Norte), fazendo o rayy aproximar no número mais próximo de 64
		if(raya > PI) { rayy = (((int)playerY>>6)<<6) - 0.0001; rayx = (playerY - rayy) * aTangente + playerX; yo = -64; xo = -yo * aTangente; }
		// right foward 6 bits, depois left foward de 6 bits, zerando os bits de 6ª casa pra frente? Isso é como se tivesse dividindo por 64 e arredondar para baixo, depois multiplicar por 64
		// A.y=rounded_down(224/64) * (64) - 1 = 191; A.x = Px + (Py-A.y)/tan(ALPHA);
		
		// também: raioX = (posiçãoY - raioY) * (-1 / (sin(raioAngulo) / (cos(raioAngulo))) ) + posiçãoX?

		//está olhando para baixo (Sul)
		if(raya < PI) { rayy = (((int)playerY>>6)<<6) + 64; rayx = (playerY - rayy) * aTangente + playerX; yo = 64; xo = -yo * aTangente; }
		
		//olhando para esquerda ou direita
		if(raya == 0 || raya == PI){ rayx = playerX; rayy = playerY; dof = 8; }
		
		while(dof<8){
			mapx = (int) (rayx)>>6; mapy = (int) (rayy)>>6; map = mapy * mapaX + mapx; // Divide rayy e rayx por 64, arredondando pra baixo, e faça o encontro na matriz
			if(map > 0 && map<mapaX*mapaY && mapa[map] == 1){ hx = rayx; hy = rayy; disH = distancia(playerX, playerY, hx, hy, raya); dof=8; } //de frente à uma parede
			else { rayx += xo; rayy += yo; dof+=1; }; //proxima linha para ver (interessante...)
		}
		
		/*glColor3f(0,1,0); //realizando a linha novamente ;)
		glLineWidth(10);
		glBegin(GL_LINES);
		glVertex2i(playerX, playerY);
		glVertex2i(rayx, rayy);
		glEnd();*/


		// --- Checa as Linhas Verticais
		
		float disV = 1000000, vx = playerX, vy = playerY;
		
		dof = 0;
		float nTangente = -tan(raya);
		
		//está olhando pra esquerda (Oeste), fazendo o rayy aproximar no número mais próximo de 64
		if(raya > PI2 && raya < PI3) { rayx = (((int)playerX>>6)<<6) - 0.0001; rayy = (playerX - rayx) * nTangente + playerY; xo = -64; yo = -xo * nTangente; }
		
		//está olhando para direita (Leste)
		if(raya < PI2 || raya > PI3) { rayx = (((int)playerX>>6)<<6) + 64; rayy = (playerX - rayx) * nTangente + playerY; xo = 64; yo = -xo * nTangente; }
		
		//olhando para cima ou baixo
		if(raya == 0 || raya == PI){ rayx = playerX; rayy = playerY; dof = 8; }
		
		while(dof<8){
			mapx = (int) (rayx)>>6; mapy = (int) (rayy)>>6; map = mapy * mapaX + mapx;
			if(map > 0 && map<mapaX*mapaY && mapa[map] == 1){ vx = rayx; vy = rayy; disV = distancia(playerX, playerY, vx, vy, raya); dof=8; } //de frente à uma parede
			else { rayx += xo; rayy += yo; dof+=1; }; //proxima linha para ver (interessante...)
		}

		
		//Encontrando o menor caminho:
		if (disV < disH) { rayx = vx; rayy = vy; disT = disV; glColor3f(0,0.9,0); }
		else if (disH < disV) { rayx = hx; rayy = hy; disT = disH; glColor3f(0,0.5,0); }

		//glColor3f(1,0,0); //realizando a linha novamente ;)
		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2i(playerX, playerY);
		glVertex2i(rayx, rayy);
		glEnd();

		// Desenhando Parede 3D
		float cosA = playerAngulo - raya;  if(cosA < 0){ cosA += 2*PI; } if (cosA > 2*PI) { cosA -= 2*PI; }
		disT = disT * cos(cosA); // arruma o Olho de Peixe
		float linhaH = (mapaT*320)/disT; if (linhaH>320) { linhaH>320; } // comprimento da parede, limitando ela à 320
		float linhaOffset = 160 - linhaH/2;				// offset da parede
		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(r * 8 + 530, linhaOffset);
		glVertex2i(r * 8 + 530, linhaH + linhaOffset);
		glEnd();

		raya += DR; if(raya < 0){ raya += 2*PI; } if (raya > 2*PI) { raya -= 2*PI; } //isso é o FOV, aumentando ele em um degrau :)
	}
}

void resize(int w, int h){
	glutReshapeWindow(WIDTH,HEIGHT);
}

float frame1, frame2, fps; //FPS

void display(){

	//FPS
	frame2 = glutGet(GLUT_ELAPSED_TIME);
	fps = frame2 - frame1;
	frame1 = glutGet(GLUT_ELAPSED_TIME);
	

	/*if(tecla == 'a') { playerX -= 5; };
	if(tecla == 'd') { playerX += 5; };	Esses exemplos serviriam para movimentar esquerda e direita
	if(tecla == 'w') { playerY += 5; };	sem mudar o ângulo apontado pelo jogador
	if(tecla == 's') { playerY -= 5; };*/

	//if(Teclas.a == 1) { playerAngulo -= 0.1 * fps; if(playerAngulo<0){playerAngulo += 2*PI;} playerDeltaX = cos(playerAngulo) * 5; playerDeltaY = sin(playerAngulo) * 5; }; This ain't gonna work pal
	if(Teclas.a == 1) { playerAngulo -= 0.05; if(playerAngulo<0){playerAngulo += 2*PI;} playerDeltaX = cos(playerAngulo) * 5; playerDeltaY = sin(playerAngulo) * 5; };
	if(Teclas.d == 1) { playerAngulo += 0.05; if(playerAngulo>2*PI){playerAngulo -= 2*PI;} playerDeltaX = cos(playerAngulo) * 5; playerDeltaY = sin(playerAngulo) * 5; };
	if(Teclas.w == 1) { playerX += playerDeltaX * 0.5; playerY += playerDeltaY * 0.5; };
	if(Teclas.s == 1) { playerX -= playerDeltaX * 0.5; playerY -= playerDeltaY * 0.5; };
	if(Teclas.escape == 1) { exit(0); }; //Esc em ASCII
	glutPostRedisplay();

	glClear(GL_COLOR_BUFFER_BIT);
	drawMap2D();
	drawPlayer();
	glutSwapBuffers();
}

void botaoApertado(unsigned char tecla, int x, int y){
	if (tecla == 'a') { Teclas.a = 1; }
	if (tecla == 'd') { Teclas.d = 1; }
	if (tecla == 'w') { Teclas.w = 1; }
	if (tecla == 's') { Teclas.s = 1; }
	if (tecla == 27) { Teclas.escape = 1; }
	glutPostRedisplay();
}

void botaoSoltar(unsigned char tecla, int x, int y){
	if (tecla == 'a') { Teclas.a = 0; }
	if (tecla == 'd') { Teclas.d = 0; }
	if (tecla == 'w') { Teclas.w = 0; }
	if (tecla == 's') { Teclas.s = 0; }
	glutPostRedisplay();
}


int init(){
	glClearColor(0.3,0.3,0.3,0); //Deixar a cor como cinza
	gluOrtho2D(0,WIDTH,HEIGHT,0); //Criar uma janela por 1024x512
	playerX = 300; playerY = 300; playerDeltaX = cos(playerAngulo)*5; playerDeltaY = sin(playerAngulo)*5; //Inicialisando o Local do jogador
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutInitWindowPosition(200,200);
	glutCreateWindow("Teste de Raycaster");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(botaoApertado);
	glutKeyboardUpFunc(botaoSoltar);
	//gluOrtho2D(0,640,0,640);
	glutMainLoop();
	return 0;
}

