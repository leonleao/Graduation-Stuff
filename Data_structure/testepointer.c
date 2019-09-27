#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main () {

	srand(time(0));

	int N = 4, M = 3, i, j;

	int **origem = malloc (N * sizeof(int *));
	
	printf("Pontos na origem\n");
	
	for (i = 0; i < N; i++){
		origem[i] = calloc(M, sizeof(int));
		for (j = 0; j < M; j++){
			origem[i][j] = rand()%100;
			printf("%d ", origem[i][j]);
		}
		printf("\n");
	}
	
	printf("\n");
	
	int **destino = malloc (N * sizeof(int *));;
	
	for (i = N - 1 ; i >= 0; i--){
		destino[N - 1 - i] = origem[i];
	}
	
	printf("Posicoes de memoria\n");
	for (i = 0; i < N; i++){
		printf("origem[%d] = %p\n", i, origem[i]);
		printf("destino[%d] = %p\n", i, destino[i]);
		printf("\n");
	}
	
	printf("\n");
	
	for (i = 0; i < N; i++){
		free(origem[i]);
	}
	
	origem = destino;
	
	for (i = 0; i < N; i++){
		printf("origem[%d] = %p\n", i, origem[i]);
		printf("destino[%d] = %p\n", i, destino[i]);
		printf("\n");
	}
	
}



void ordenarMatriz( int ***matriz, int eixo, int inicio, int fim, int tamanho, int space ) {
	
	
	int i, j, posicaoPrimeira, posicaoSegunda, posicaoMatriz, metade;

	if ( inicio == fim ) {
		return;
	}
	
	int **aux = malloc ((fim-inicio+1) * sizeof(int *));

	metade = ( inicio+fim )/2;
	ordenarMatriz( matriz, eixo, inicio, metade, tamanho, space );
	ordenarMatriz( matriz, eixo, metade+1, fim, tamanho, space );

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