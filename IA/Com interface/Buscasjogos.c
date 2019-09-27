/* Dado as dimensões de um tabuleiro retangular e a posição inicial do cavalo,
o programa verifica e acha, caso exista, a solução para o passeio do cavalo começando naquele ponto. 
O usuário tem três opções de busca: Profundidade, Largura e Gulosa.
*/

//Inclusão de bibliotecas utilizadas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include "stuff.h"

/*Variáveis globais
nos: Nós visitados durante a busca.
memoria[3]: Quantidade de alocação de memória de cada estrutura.
memoriamax: Memória máxima utilizada na busca.
begin, end e time_spent: Variáveis para contagem de tempo.
board[2]: Dimensões do tabuleiro.
estadofinal: Estado final (quantidade máxima de casas no tabuleiro - 1).
Altura: Altura da árvore no estado atual.
*/

double nos, memoria[3], memoriamax;
clock_t begin, end;
double time_spent;
int board[2], estadofinal, altura;

/*Declaração de assinaturas.
buscaProf: Realiza busca recursiva de profundidade na árvore de estados. Tem como entrada o nó (estado) 
		   a ser visitado e uma lista que retorna solução, se existir.

buscaLarg: Realiza busca iterativa de largura na árvore de estados. Tem como entrada a agenda 
		   e uma lista que retorna solução, se existir.
		   
buscaGulosa: Realiza busca recursiva heurística gulosa na árvore de estados. Tem como entrada o nó (estado) 
			 a ser visitado e uma lista que retorna solução, se existir.
			 
possiveisPosicoes: Possíveis posições legais  que o cavalo pode fazer do estado atual para o próximo estado. 
				   Tem como entrada o estado atual e a lista de posições visitados.
				   
possiveisMovimentos: Verifica a função de avaliação dos nós filhos do estado atual.
*/

int buscaProf(struct nodo *p, struct lista **solu);
int buscaLarg(struct agenda **memory, struct lista **solu);
int buscaGulosa(struct nodo *p, struct lista **solu);
void possiveisPosicoes(struct nodo **p, struct lista *visitados);
void possiveisMovimentos (struct nodo **p, struct lista *visitados);

//Função Main. Imprime a solução, caso existe, e os dados da busca (Tempo, nós e memória).

int main () {
	
	/*Declarações de variáveis.
	busca: Tipo de busca a ser feito.
	achousolucao: Verifica a existência da solução.
	invalido: Verifica se as entradas do usuário são validas.
	pos[2]: Verifica a posição inicial.
	
	*fila: Agenda de entrada para a busca em largura.
	*solucao: Lista que conterá a solução, se esta existir.
	*auxiliar: lista auxiliar para limpeza de memória.
	*inicial: Estado inicial
	*/
	
	int busca, achousolucao, invalido;
	char err;	
	int pos[2];
	
	struct agenda *fila = NULL;
	struct lista *solucao = NULL, *auxiliar = NULL;
	struct nodo *inicial;
	
		
	nos = 0;
	memoria[0] = 1;
	memoria[1] = 0;
	memoria[2] = 0;		
	memoriamax = 0;
	invalido = 1;
	altura = 0;
	
	//Recebe entradas e verifica se são validas. entradas validas, elementos de board > 0 e elementos de pos >= 0.
	
	while (invalido) {
		printf ("Informe as dimensoes x e y do tabuleiro separados por espaco: ");
		
		err = scanf("%d",&board[0]) && scanf("%d",&board[1]);
		
		if (!err) {
			printf ("Formato invalido. Somente numeros sao aceitos. O programa terminara.!!.\n");
			exit(0);
		}
		
		if (board[0] <= 0 || board[1] <= 0) {
			printf ("Dimensao invalida. Os valores precisam ser maiores que 0.\n");
		}
		else 
			invalido = 0;
		
	}
			
	invalido = 1;
	
	while (invalido) {
		printf ("Informe a posicao inicial x e y do cavalo separados por espaco: ");
		
		err = scanf("%d",&pos[0]) && scanf("%d",&pos[1]);
		
		if (!err) {
			printf ("Formato invalido. Somente numeros sao aceitos. O programa terminara!!.\n");
			exit(0);
		}
		
		if (pos[0] < 0 || pos[0] >= board[0] || pos[1] < 0 || pos[1] >= board[1]) {
			printf ("Posicao invalida. Os valores nao podem ser numeros negativos.\n");
		}
		else 
			invalido = 0;
		
	}
	
	printf ("\n");
	
	//Estabelece o estado final, todos as posiçoes do tabuleiro - 1.
	
	estadofinal = (board[0]*board[1]) - 1;
	
	//Cria o estado inicial.
	
	inicial = calloc (1, sizeof(struct nodo));	
	inicial->ponto[0] = pos[0];
	inicial->ponto[1] = pos[1];
	
	invalido = 1;
	
	//Verifica a entrada do usuário para a busca. busca < 4 e > 0.
	
	while (invalido) {
		printf ("Tipos de buscas\n1) Busca em profundidade.\n2) Busca em Largura.\n3) Busca gulosa (heuristica).\nDigite o indice da busca desejada: ");
		err = scanf("%d", &busca);
		
		if (!err) {
			printf ("Formato invalido. Somente numeros sao aceitos. O programa terminara!!.\n");
			exit(0);
		}
		
		if (busca < 1 || busca > 3) {
			printf ("Indice invalido.\n");
		}
		else 
			invalido = 0;
		
	}
	
	//Inicia a contagem de tempo da busca.
	
	begin = clock();
	
	//Cases para o tipo de busca.
	
	switch(busca) {
		case 1:
			achousolucao = buscaProf(inicial, &solucao);
			break;
		
		case 2:
			//cria a agenda para busca em largura.
			fila = calloc(1,sizeof(struct agenda));
			fila->visitados = calloc(1,sizeof(struct lista));
			memoria[1]=1;
			memoria[2]=1;
			fila->no = inicial;
			fila->visitados->ponto[0] = inicial->ponto[0];
			fila->visitados->ponto[1] = inicial->ponto[1];
			achousolucao = buscaLarg(&fila, &solucao);
			break;
			
		 case 3:
			achousolucao = buscaGulosa(inicial, &solucao);
			break;
			
		default:
			break;
	}
			
	//Para a contagem do tempo de busca e verifica o tempo total.
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	
	printf ("\n");
	
	//Verifica se há solução. Se houver, imprime a solução.
	
	if (achousolucao == 0){
		printf("Nao ha solucao\n");
	}
	else if (achousolucao == 2) {
		printf("Termino forcado do programa\n");
		while (solucao != NULL){
			solucao = solucao->prox;
			free(auxiliar);
			memoria[1]--;
			auxiliar = solucao;
		}
	}
	else {
		auxiliar = solucao;
		interfaceGrafica(solucao,board);
		printf("Solucao: ");
		while (solucao != NULL){
			printf("(%d  %d)",solucao->ponto[0],solucao->ponto[1]);
			solucao = solucao->prox;
			free(auxiliar);
			memoria[1]--;
			auxiliar = solucao;
			if (solucao != NULL)
				printf("->");
		}
		printf ("\n");
	}
	
	printf ("\n");
	
	// Imprime os dados da busca (Tempo, memória e nós visitados).
	
	printf("Tempo decorrido: %.2es\n",time_spent);
	printf("Nos visitados: %.2e\n", nos);
	printf("Memoria maxima utilizada: %.2e Bytes\n", memoriamax);
	
	printf ("\n");
	
	//Verifica se usuário quer fazer nova busca.
	
	printf ("Pressione qualquer tecla para terminar o programa.");
	
	getch();
	getch();

	
}

//Função para definir a função de avaliação dos nós filhos.

void possiveisMovimentos (struct nodo **p, struct lista *visitados) {
	int i,j = 0, k, velho;
	
	struct lista *verif;
	
	//O programa verifica quantos movimentos possíveis legais os filhos do nó atual pode realizar e define como fa dos mesmos.
	
	while ((*p)->prox[j] != NULL && j < (*p)->fa){
		k = 0;
		for (i = 0; i <= 7; i++) {
			
			//Verifica se o movimento i é legal.
			
			if (((*p)->prox[j]->ponto[0] + mov[i][0] >= 0) && ((*p)->prox[j]->ponto[0] + mov[i][0] < board[0])){
				if (((*p)->prox[j]->ponto[1] + mov[i][1] >= 0) && ((*p)->prox[j]->ponto[1] + mov[i][1] < board[1])){
					velho = 0;
					verif = visitados;
					
					//Verifica se o movimento i legal não foi realizado naquele ramo. Caso não tenha sido, incrementa o fa.
					
					while (verif != NULL && velho == 0){
						if (verif->ponto[0] == (*p)->prox[j]->ponto[0] + mov[i][0]){
							if (verif->ponto[1] == (*p)->prox[j]->ponto[1] + mov[i][1]){
								velho = 1;
							}
						}
						verif = verif->prox;
					}
					if (velho == 0){
						k++;
					}
				}
			}
		}
		(*p)->prox[j]->fa = k;
		j++;
	}
}

//Função para encontrar os sucessores do estado atual.

void possiveisPosicoes(struct nodo **p, struct lista *visitados){
	
	int i,j = 0, velho;
	
	struct lista *verif;
	
	//Verifica se o movimento i é legal.
	
	for (i = 0; i <= 7; i++) {
		if (((*p)->ponto[0] + mov[i][0] >= 0) && ((*p)->ponto[0] + mov[i][0] < board[0])){
			if (((*p)->ponto[1] + mov[i][1] >= 0) && ((*p)->ponto[1] + mov[i][1] < board[1])){
				velho = 0;
				verif = visitados;
				
				//Verifica se o movimento legal i já foi realizado. Caso não tenhha sido, coloca como sucessor.
				
				while (verif != NULL && velho == 0){
					if (verif->ponto[0] == (*p)->ponto[0] + mov[i][0]){
						if (verif->ponto[1] == (*p)->ponto[1] + mov[i][1]){
							velho = 1;
						}
					}
					verif = verif->prox;
				}
					
				if (velho == 0){
					(*p)->prox[j] = calloc (1, sizeof(struct nodo));	
					memoria[0]++;
					(*p)->prox[j]->ponto[0] = (*p)->ponto[0] + mov[i][0];
					(*p)->prox[j]->ponto[1] = (*p)->ponto[1] + mov[i][1];
					j++;
				}
			}
		}
	}
	
	//Define o número de sucessores.
	
	(*p)->fa = j;
	
}

//Função de busca por profundidade.

int buscaProf(struct nodo *p, struct lista **solu) {
	
	//Atualiza a quantidade de nós visitados.
	
	nos++;
	
	struct lista *auxiliar = *solu;
	
	//Coloca a posição atual na lista de solução possível.
	
	if ((*solu) == NULL) {
		(*solu) = calloc(1,sizeof(struct lista));	
		memoria[1]++;
		(*solu)->ponto[0] = p->ponto[0];
		(*solu)->ponto[1] = p->ponto[1];
	} else {
		while (auxiliar->prox != NULL)
			auxiliar = auxiliar->prox;
		auxiliar->prox = calloc(1,sizeof(struct lista));
		memoria[1]++;
		auxiliar->prox->ponto[0] = p->ponto[0];
		auxiliar->prox->ponto[1] = p->ponto[1];
	}
	
	//Atualiza a memória máxima utilizada na busca.
	
	if ((memoria[0]*sizeof(struct nodo)) + (memoria[1]*sizeof(struct lista)) + (memoria[2]*sizeof(struct agenda)) > memoriamax) {
		memoriamax = (memoria[0]*sizeof(struct nodo)) + (memoria[1]*sizeof(struct lista)) + (memoria[2]*sizeof(struct agenda));
	}
	
	//Verifica se o estado atual é final.
	
	if (altura == estadofinal) {
		
		//O nó atual é final e seu ramo é uma solução.
		
		free(p);
		memoria[0]--;
		altura--;
		return 1;
	}
	
	if (kbhit()) {
		
		//Programa foi terminado a força.
		
		free(p);
		memoria[0]--;
		altura--;
		return 2;
	}
	
	//Acha os sucessores.
	
	possiveisPosicoes (&p, *solu);
	
	int i = 0, result = 0;
	
	//Verifica os sucessores recursivamente.
	
	while (p->prox[i] != NULL && i < 8) {
		altura++;
		result = buscaProf(p->prox[i], &(*solu));
		if (result) {
			
			//O nó atual pertence ao ramo solução.
			
			int j;
			for (j = i+1; j < 8; j++) {
				if (p->prox[j] != NULL) {
					free(p->prox[j]);
					memoria[0]--;
				}
			}
			free(p);
			memoria[0]--;
			altura--;
			return result;		
		}
		i++;
	}
	
	//O nó atual e seus filhos não fazem parte da solução. Remove o nó atual e sobe uma altura da árvore.
	
	free(p);
	memoria[0]--;
	
	if (auxiliar != NULL){
		free(auxiliar->prox);
		auxiliar->prox = NULL;
		
	}
	else {
		free((*solu));
		(*solu) = NULL;
	}
	
	memoria[1]--;
	
	altura--;
	return 0;
}

//Função de busca por largura

int buscaLarg(struct agenda **memory, struct lista **solu) {
	
	int solucao;
	
	while (1){
	
		//Atualiza a contagem de nós.
	
		nos++;
		
		//Atualiza a memória máxima utilizada na busca.
		
		if ((memoria[0]*sizeof(struct nodo)) + (memoria[1]*sizeof(struct lista)) + (memoria[2]*sizeof(struct agenda)) > memoriamax) {
			memoriamax = (memoria[0]*sizeof(struct nodo)) + (memoria[1]*sizeof(struct lista)) + (memoria[2]*sizeof(struct agenda));
		}
		
		//Verifica se a agenda está vazia. Se estiver, não existe solução e sai do loop.
		
		if ((*memory) == NULL){
			solucao = 0;
			break;
		}
		
		//Verifica se o estado atual é final. Se for, sai do loop.
		
		if ((*memory)->altura == estadofinal) {
			(*solu) = (*memory)->visitados;
			struct agenda *erease = (*memory);
			struct lista *erease2, *erease3;
			(*memory) = (*memory)->prox;
			free(erease->no);
			memoria[0]--;
			free(erease);
			memoria[2]--;
			while ((*memory)!= NULL){
				erease = (*memory);
				free(erease->no);
				memoria[0]--;
				erease2 = erease->visitados;
				while (erease2 != NULL){
					erease3 = erease2;
					erease2 = erease2->prox;
					free(erease3);
					memoria[1]--;
				}
				(*memory) = (*memory)->prox;
				free(erease);
				memoria[2]--;
			}
			solucao = 1;
			break;
			
		}
		
		//Programa terminado de maneira forçada
		
		if (kbhit()) {
			(*solu) = (*memory)->visitados;
			struct agenda *erease = (*memory);
			struct lista *erease2, *erease3;
			(*memory) = (*memory)->prox;
			free(erease->no);
			memoria[0]--;
			free(erease);
			memoria[2]--;
			while ((*memory)!= NULL){
				erease = (*memory);
				free(erease->no);
				memoria[0]--;
				erease2 = erease->visitados;
				while (erease2 != NULL){
					erease3 = erease2;
					erease2 = erease2->prox;
					free(erease3);
					memoria[1]--;
				}
				(*memory) = (*memory)->prox;
				free(erease);
				memoria[2]--;
			}
			solucao = 2;
			break;
			
		}
		
		int i = 0;
		struct agenda *aux = (*memory);
		struct lista *aux2 = NULL, *aux3 = NULL;
		
		//Gera os sucessores.
		
		possiveisPosicoes(&((*memory)->no), (*memory)->visitados);
		
		//Manipulação de dados para colocar os elementos sucessores na fila.
		
		while ((*memory)->no->prox[i] != NULL && i < 8) {
			while (aux->prox != NULL)
				aux = aux->prox;
			aux->prox = calloc (1, sizeof(struct agenda));
			memoria[2]++;
			aux->prox->no = (*memory)->no->prox[i];
			aux->prox->altura = (*memory)->altura + 1;
			aux->prox->visitados = calloc (1, sizeof(struct lista));	
			memoria[1]++;
			aux2 = aux->prox->visitados;
			aux3 = (*memory)->visitados;
			aux2->ponto[0] = aux3->ponto[0];
			aux2->ponto[1] = aux3->ponto[1];
			while (aux3->prox != NULL){
				aux2->prox = calloc (1, sizeof(struct lista));
				memoria[1]++;
				aux3 = aux3->prox;
				aux2->prox->ponto[0] = aux3->ponto[0];
				aux2->prox->ponto[1] = aux3->ponto[1];
				aux2 = aux2->prox;
			}
			aux2->prox = calloc (1, sizeof(struct lista));
			memoria[1]++;
			aux2->prox->ponto[0] = (*memory)->no->prox[i]->ponto[0];
			aux2->prox->ponto[1] =(*memory)->no->prox[i]->ponto[1];
			aux = aux->prox;
			i++;
		}
		
		//Remove o estado atual da fila.
		
		aux = (*memory);
		(*memory) = (*memory)->prox;
		
		free(aux->no);
		memoria[0]--;
		aux2 = aux->visitados;
		while (aux2 != NULL){
			aux3 = aux2;
			aux2 = aux2->prox;
			free(aux3);
			memoria[1]--;
		} 
		
		free(aux);
		memoria[2]--;
		
	}
		
	return solucao;
	
}

//Função de busca heurística gulosa.

int buscaGulosa (struct nodo *p, struct lista **solu) {
	
	//Atualiza a contagem de nós.
	
	nos++;
	
	struct lista *auxiliar = *solu;
	
	//Coloca o ponto atual na lista de solução possível.
	
	if ((*solu) == NULL) {
		(*solu) = calloc(1,sizeof(struct lista));
		memoria[1]++;
		(*solu)->ponto[0] = p->ponto[0];
		(*solu)->ponto[1] = p->ponto[1];
	} else {
		while (auxiliar->prox != NULL)
			auxiliar = auxiliar->prox;
		auxiliar->prox = calloc(1,sizeof(struct lista));
		memoria[1]++;
		auxiliar->prox->ponto[0] = p->ponto[0];
		auxiliar->prox->ponto[1] = p->ponto[1];
	}
	
	//Atualiza a memória máxima utilizada na busca.
	
	if ((memoria[0]*sizeof(struct nodo)) + (memoria[1]*sizeof(struct lista)) + (memoria[2]*sizeof(struct agenda)) > memoriamax) {
		memoriamax = (memoria[0]*sizeof(struct nodo)) + (memoria[1]*sizeof(struct lista)) + (memoria[2]*sizeof(struct agenda));
	}
	
	//Verifica se o estado atual é final. Se for, retorna 1.
	
	if (altura == estadofinal) {	
		free(p);
		memoria[0]--;
		altura--;
		return 1;
	}
	
	if (kbhit()) {	
		free(p);
		memoria[0]--;
		altura--;
		return 2;
	}
	
	int result = 0, i, j, k;
	
	//Gera os sucessores e define suas funções de avaliação.
	
	possiveisPosicoes (&p, *solu);
	possiveisMovimentos(&p, *solu);
		
	//Verifica recursivamente o nó com menor estado de avaliação mais a esquerda.
		
	for (i = 0; i < 8; i++) {
		 for (j = 0; j < p->fa; j++) {
			if (p->prox[j] != NULL) {
				if (p->prox[j]->fa == i){
					altura++;
					result = buscaGulosa(p->prox[j], &(*solu));
					if (result) {
						//O nó atual faz parte da solução.
						p->prox[j] = NULL;
						for (k = 0; k < 8; k++) {
							if (p->prox[k] != NULL) {
								free(p->prox[k]);
								memoria[0]--;
								p->prox[k] = NULL;
							}
						}
						free(p);
						memoria[0]--;
						altura--;
						return result;
						}
					else {
						free(p->prox[j]);
						p->prox[j] = NULL;
						memoria[0]--;
					}
				}
			}
		 }
	}
	
	//O nó atual não faz parte da solução. Retira da lista e sobe na altura da árvore.
	
	if (auxiliar != NULL){
	
		free(auxiliar->prox);
		auxiliar->prox = NULL;
		
	}
	else {
		free((*solu)->prox);
		free((*solu));
		(*solu) = NULL;
	}
	
	memoria[1]--;
	
	altura--;
	return 0;
	
}