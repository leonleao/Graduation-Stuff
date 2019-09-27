#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash [6251], memoria = 6250;

int inserir (int num, char pessoa);


int main () {
	
	int calice, cbeatriz, i, aux, card, card2, repetida;
	
	scanf("%d",&calice);
	scanf("%d",&cbeatriz);
	
	while (calice != 0 || cbeatriz != 0) {
		
		memset(hash, 0,sizeof(hash));
			
		card = 0;
		card2 = 0;
				
		for (i = 0; i<calice; i++) {
			
			scanf("%d", &aux);
			if (inserir(aux, 1) == 1)
				card++;					//cartas únicas de Alice
		}
						
		for (i = 0; i<cbeatriz; i++) {
			
			scanf("%d", &aux);
			repetida = inserir(aux, 0);
			if (repetida == 1) 
				card2++;				//cartas que beatriz tem que alice nao tem.
			else if (repetida == -1) {
				card--;
			}
		}
		
		printf("%d\n", (card >= card2) ? card2 : card);
				
		scanf("%d", &calice);
		scanf("%d", &cbeatriz);
	}
	
}

int inserir (int num, char usuario) {
	
	int r = (num - 1) / memoria, i = (num-1) % memoria,	a = 1 << r, b = 1 << r+16 ;
		
	if (usuario) {
		if (hash[i] == 0)
			hash[i] = a;
		else if (!(hash[i] & a))
			hash[i] |= a;
		else 
			return 0;
		return 1; 
	}
	
	else {
		if (hash[i] == 0)
			hash[i] = b;
		else if ((!(hash[i] & a)) && (!(hash[i] & b))) {
			hash[i] |= b;
		}
		else if (hash[i] & b) 
			return 0;
		else {
			hash[i] |= b;
			return -1;
		}
		return 1; 
	}
}