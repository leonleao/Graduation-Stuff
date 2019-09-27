#ifndef STUFF_H_   /* Include guard */
#define STUFF_H_

//Movimentos possíveis do cavalo nos eixos x e y.
static const int mov[8][2] = {
	{2,1},
	{2,-1},
	{-2,1},
	{-2,-1},
	{1,2},
	{1,-2},
	{-1,2},
	{-1,-2},
};

/*Estrutuda dos nós (estado) da árvore
fa: Função de avaliação, sendo neste caso a quantidade máxima de movimentos legais do cavalo naquela posição.
*prox[8]: Ponteiros para os movimentos legais possíveis no nó atual (Sucessores).
ponto[2]: posição no tabuleiro do cavalo para este estado.
*/

struct nodo { 
	int fa;
	struct nodo *prox[8];
	int ponto[2];
};

/*Estrutura de agenda utilizada na busca em largura. É uma lista de nós.
*visitados: A lista de pontos visitados do ramo até o nó atual.
altura: Altura da árvore de estados para aquele estado.
*prox: Ponteiro para o sucessor.
*/

struct agenda {
	int altura;
	struct nodo *no;
	struct lista *visitados;
	struct agenda *prox;
};

/*Estrutura de lista de pontos. É utilizado para guardar as posições visitadas.
ponto[2]: Ponto visitados
*prox: Proximo elemento da lista
*/

struct lista {
	int ponto[2];
	struct lista *prox;
};

int interfaceGrafica(struct lista *solucao, int tabuleiro[2]);

#endif // FOO_H_