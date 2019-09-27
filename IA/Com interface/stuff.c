#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include "stuff.h"
 
struct timeval last_update, last_move;
/* board width */
int board[2];
/* power of 2 */
int twid = 1, thid = 1;
int W = 240, H = 240, gwin;

struct lista *solucao;
 
int **moves;
int n_moves, need_move = 1, stopped = 0, paused = 0, skip_draw = 0;
int delay = 1024 * 128;
 
GLubyte *tex;
GLuint texture;
 
int next_move()
{
	if (!need_move || stopped || paused)
		return 0;

	if (solucao == NULL){
		printf("Passeio completo\n");
		printf("Aperte espaco para continuar\n");
		stopped = 1;
		need_move = 0;
		return 0;
	}
	 
	moves[n_moves++] = solucao->ponto;
	solucao = solucao->prox;
	 
	need_move = 1;
	return 1;
}
 
void resize(int w, int h)
{
	int dx = 0, dy = 0;
	W = w; H = h;
 
	if (w > h) dx = w - h;
	else	   dy = h - w;
 
	glViewport(dx / 2, dy / 2, W - dx, H - dy);
	glOrtho(0, board[0], 0, board[1], -1, 1);
}
 
void render()
{
	double tw = (double) board[0] / twid;
	double th = (double) board[1] / thid;
 
	struct timeval tv;
	gettimeofday(&tv, 0);
	long usec = (tv.tv_sec - last_move.tv_sec) * 1000000
			+ tv.tv_usec - last_move.tv_usec;
	if (usec > delay || skip_draw) {
		next_move();
		last_move = tv;
	}
 
	if (skip_draw && !stopped) return;
 
	usec = (tv.tv_sec - last_update.tv_sec) * 1000000
			+ tv.tv_usec - last_update.tv_usec;
	if (usec < 25000) return;
	last_update = tv;
 
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, texture);
 
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f( 0,  0); glVertex2i(0, 0);
	glTexCoord2f( 0, th); glVertex2i(0, board[1]);
	glTexCoord2f(tw, th); glVertex2i(board[0], board[1]);
	glTexCoord2f(tw,  0); glVertex2i(board[0], 0);
	glEnd();
 
	glBegin(GL_QUADS);
	glColor3f(0, .5, 1);
	int x = moves[0][0], y = moves[0][1];
 
	glVertex2i(x + 0, y + 0);
	glVertex2i(x + 0, y + 1);
	glVertex2i(x + 1, y + 1);
	glVertex2i(x + 1, y + 0);
 
	glColor4f(.5, .7, .5, .7);
	for (int i = (n_moves == board[0] * board[1]) ? n_moves - 1 : 1; i < n_moves; i++) {
		if (i == n_moves - 1)
			glColor3f(1, 0, 0);
		x = moves[i][0], y = moves[i][1];
		glVertex2f(x + 0, y + 0);
		glVertex2f(x + 0, y + 1);
		glVertex2f(x + 1, y + 1);
		glVertex2f(x + 1, y + 0);
	}
	glEnd();
 
	glBegin(GL_LINE_STRIP);
	if (n_moves == board[0] * board[1])
		glColor3f(0, .4, .4);
	else
		glColor3f(0, 0, 0);
 
	for (int i = 0; i < n_moves; i++)
		glVertex2f(moves[i][0] + .5, moves[i][1] + .5);
	glEnd();
 
	glutSwapBuffers();
	need_move = 1;
}
 
void init_texture()
{
	int i, j;
	while (twid < board[0]) twid <<= 1;
	while (thid < board[1]) thid <<= 1;
 
	GLubyte * ptr = tex = malloc(thid * twid * 3);
 
	for (i = 0; i < thid; i++)
		for (j = 0; j < twid; j++, ptr += 3)
			if ((i & 1) != (j & 1))
				ptr[0] = ptr[1] = ptr[2] = 255;
			else
				ptr[0] = 120, ptr[1] = 255, ptr[2] = 200;
 
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, twid, thid,
		0, GL_RGB, GL_UNSIGNED_BYTE, tex);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 
	free(tex);
}
 
void set_delay(int inc)
{
	if (inc) {
		delay *= 2;
		if (!delay) delay = 1;
		if (delay >= 1 << 20) {
			delay = 1 << 20;
			paused = 1;
			printf("Pausado\n");
			return;
		}
	} else
		delay /= 2;
	paused = 0;
}
 
void keypress(unsigned char key, int x, int y)
{
	switch(key) {
	case ' ':
	case 'q':
	case 27:
		glFinish();
		glutDestroyWindow(gwin);
		glutLeaveMainLoop();
		return;
	case ',':
	case '<':
		set_delay(1);
		return;
	case '.':
	case '>':
		set_delay(0);
		return;
	case 's':
		skip_draw = !skip_draw;
		return;
	}
}
 
void init_graphics()
{
	
	int c = 1;
	char **v = calloc (1, sizeof(char *));
	v[0] = "Buscasjogos.c";
	
	
	glutInit(&c, v);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutDisplayFunc(render);
	glutIdleFunc(render);
	glutDisplayFunc(render);
 
	gwin = glutCreateWindow("ApresentacaoIA");
 
	glutKeyboardFunc(keypress);
	glutReshapeFunc(resize);
 
	glClearColor(.2, .2, .2, 1);
 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, board[0], 0, board[1], -1, 1);
	glMatrixMode(GL_MODELVIEW);
 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
	init_texture();
}
 
int interfaceGrafica(struct lista *soluction, int tabuleiro[2])
{	
	n_moves = 1;
	board[0] = tabuleiro[0];
	board [1] = tabuleiro[1];
 
	solucao = soluction;
 
	moves = calloc (board[0]*board[1], sizeof(int *));
	
	moves[0] = solucao->ponto;
	
	solucao = solucao->prox;
 
	init_graphics();
	gettimeofday(&last_update, 0);
	last_move = last_update;
	
	glutDisplayFunc(render);
 
	glutMainLoop();

	return 0;
}