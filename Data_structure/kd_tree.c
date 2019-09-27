#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

struct nodo {
	int indice, flag; 
	struct nodo *esq, *dir;
	int* ponto;
};

#define SQ(x)			((x) * (x))

int melhordistancia = -1;
int resposta;

void buscaProx (struct nodo *p, int *ponto, int eixo, int tamanho);
void inserirKD (struct nodo **p, int ***pontos, int tamanho, int eixo, int inicio, int fim, int space);
void ordenarMatriz( int ***matriz, int eixo, int inicio, int fim, int tamanho );

int main () {
	
	int tamanho, i, j, ma, mb;
		
 	scanf("%d",&tamanho);
	scanf("%d",&ma);
	scanf("%d",&mb);  
	
	int tamanhoBytes = (tamanho + 1) * sizeof(int);
	
	int **vetorA = malloc (ma*sizeof(int *));
	int **vetorB = malloc (mb*sizeof(int *));
	
	for (i = 0; i < ma; i++){
		vetorA[i] = calloc(tamanho, sizeof(int));
		for (j = 0; j < tamanho; j++)
			scanf("%d",&vetorA[i][j]); 
	}

	struct nodo *raiz = NULL;

	for (i = 0; i < mb; i++) {
		vetorB[i] = calloc(tamanho+1, sizeof(int));
		for (j = 0; j < tamanho; j++)
			scanf("%d",&vetorB[i][j]);
		vetorB[i][tamanho] = i+1;
	}
	
	inserirKD(&raiz, &vetorB, tamanho, 0, 0, mb-1, tamanhoBytes);
	
	for (i = 0; i < ma; i++) {
		 buscaProx (raiz, vetorA[i], 0, tamanho);
		 printf("%d\n", resposta);
		 melhordistancia = -1;
	 }	 	
}

void inserirKD (struct nodo **p, int ***pontos, int tamanho, int eixo, int inicio, int fim, int space) {
	
	int i, j;
	
	if (inicio > fim)
		return;
	
	*p = calloc(1, sizeof(struct nodo));
	(*p)->ponto = malloc(space);
	
	if (inicio < fim) {
		int metade = (inicio + fim + 1)/2;
		ordenarMatriz(pontos, eixo, inicio, fim, tamanho);
		(*p)->flag = 1;
		memcpy((*p)->ponto,  (*pontos)[metade], space);
		inserirKD(&(*p)->esq, pontos, tamanho, (eixo + 1)%tamanho, inicio, metade-1, space);
		inserirKD(&(*p)->dir, pontos, tamanho, (eixo + 1)%tamanho, metade+1 , fim, space);
		return;
	}
	else {
		(*p)->flag = 0;
		memcpy((*p)->ponto, (*pontos)[inicio], space);
		return;
	}
}

void ordenarMatriz( int ***matriz, int eixo, int inicio, int fim, int tamanho) {
	
	
	int i, j, posicaoPrimeira, posicaoSegunda, posicaoMatriz, metade;

	if ( inicio == fim ) {
		return;
	}
	
	int **aux = malloc ((fim-inicio+1) * sizeof(int *));

	metade = ( inicio+fim )/2;
	ordenarMatriz( matriz, eixo, inicio, metade, tamanho );
	ordenarMatriz( matriz, eixo, metade+1, fim, tamanho );

	posicaoPrimeira = inicio;
	posicaoSegunda = metade+1;
	posicaoMatriz = 0;
	
	while( posicaoPrimeira < metade+1 || posicaoSegunda  < fim+1 ) { 
		if ( posicaoPrimeira == metade+1 ){ 
			aux[posicaoMatriz] = (*matriz)[posicaoSegunda];
			posicaoSegunda++;
			posicaoMatriz++;
		} 
		else {
			if (posicaoSegunda == fim+1) { 
				aux[posicaoMatriz] = (*matriz)[posicaoPrimeira];
				posicaoPrimeira++;
				posicaoMatriz++;
			} else {
				if ((*matriz)[posicaoPrimeira][eixo] < (*matriz)[posicaoSegunda][eixo]) { 
					aux[posicaoMatriz] = (*matriz)[posicaoPrimeira];
					posicaoPrimeira++;
					posicaoMatriz++;
				} 
				else { 
					aux[posicaoMatriz] = (*matriz)[posicaoSegunda];
					posicaoSegunda++;
					posicaoMatriz++;
				}
			}
		}

	}
		
	for( i = inicio; i <= fim; i++ )
		(*matriz)[i] = aux[i-inicio];	
	
	free(aux);
}


void buscaProx (struct nodo *p, int *ponto, int eixo, int tamanho) {
		
	int distanciaatual, hiperplane;
	int lado, i;
	int proxEixo = (eixo + 1)%tamanho;
	
	if (p == NULL) return;
		
	distanciaatual = 0;
	for(i=0; i<tamanho; i++) {
		distanciaatual += SQ(p->ponto[i] - ponto[i]);
	}
	
	if ((melhordistancia == -1) || (distanciaatual < melhordistancia)) {
		melhordistancia = distanciaatual;
		resposta = p->ponto[tamanho];
	}
	
	lado = p->ponto[eixo] - ponto[eixo];
	
	hiperplane = SQ(lado);
	
	
	if (melhordistancia == 0) return;
		
	if (p->flag != 0) {
		if (lado > 0)
			buscaProx(p->esq, ponto, proxEixo, tamanho);
		else 
			buscaProx(p->dir, ponto, proxEixo, tamanho);
	}
	
	if (hiperplane >= melhordistancia) return;
	
	if (p->flag != 0) {
		if (lado > 0)
			buscaProx(p->dir, ponto, proxEixo, tamanho);
		else 
			buscaProx(p->esq, ponto, proxEixo, tamanho);
	}
	
}