#include<stdio.h>
#include <stdlib.h>
#include <string.h>

struct nodo {
	char *prefixo, *flag;
	struct nodo *p[26];
};

struct nodo *criarindice ();
int verificar (char *palavra, struct nodo *p);
void inserir (char *palavra, struct nodo **p);
void remover (char *palavra, struct nodo **p);
void imprimir (struct nodo *p);
void imprimir2 (char *palavra, struct nodo *p);

int main()
{

	struct nodo *root = criarindice();
	char str[1000];
	
	FILE *fptr;
	
	if ((fptr=fopen("test.txt","r"))==NULL){
       printf("Error! opening file");
       exit(1);         /* Program exits if file pointer returns NULL. */
	}
	
	while (1) {
		if (feof(fptr))
			break;
		fscanf(fptr, "%s", str); 
		inserir(str, &root);
	}

	fclose(fptr);
	imprimir (root);
	printf("\n");
	imprimir2("\0", root);
	free(root);

    return 0;
}

struct nodo *criarindice () {
	
	struct nodo *p;
	
	p = calloc (1, sizeof(struct nodo));
	p->prefixo = strdup("\0");
	p->flag = calloc (1, sizeof(char));
	p->flag[0] = -1;
	
	return p;
}

void inserir (char *palavra, struct nodo **p){
	
	int lenpalavra, lenprefixo, i, j;
	char letra;
	
	lenpalavra = strlen(palavra);
	if ((*p) == NULL){		
		*p = calloc (1, sizeof(struct nodo));
		(*p)->prefixo = strdup(palavra);
		(*p)->flag = calloc (lenpalavra+1, sizeof(char));
		(*p)->flag[lenpalavra] = 1;
		return;
	}
	lenprefixo = strlen ((*p)->prefixo);
	if (lenprefixo == 0) {
		if ((*p)->flag[0] == -1 || lenpalavra > 0) {
			inserir (palavra + 1, &(*p)->p[palavra[0]-'a']);
			return;
		}
	}
	for (i=0; i<lenprefixo; i++){
		if (palavra[i] != (*p) -> prefixo[i]) {	
			if (palavra[i] == '\0')	{
				(*p)->flag[i] = 1;			
			}
			else {																	
				struct nodo *pai = calloc (1, sizeof(struct nodo));
				if (i == 0) {
					pai->prefixo = calloc (1, sizeof(char));
				}
				else { 
					pai->prefixo = calloc (i, sizeof(char));
					strncpy(pai->prefixo, palavra, i);
				}
				pai->flag=calloc(i+1, sizeof(char));
				for (j=0; j<=i; j++)
					pai->flag[j] = (*p)->flag[j];
				inserir(palavra +i +1, &pai->p[palavra[i]-'a']);
				pai->p[(*p)->prefixo[i]-'a'] = *p;
				memmove ((*p)->prefixo, (*p)->prefixo + i + 1, (lenprefixo - i)*sizeof(char));
				(*p)->prefixo = realloc ((*p)->prefixo, lenprefixo - i);
				memmove ((*p)->flag, (*p)->flag + i + 1, (lenprefixo - i)*sizeof(char));
				(*p)->flag = realloc ((*p)->flag, lenprefixo - i);
				*p = pai;
			}
			return;
		}
	}
	if (palavra[i] == '\0')
		(*p)->flag[i] = 1;
	else {
		for (j = 0; j < 26; j++){
			if ((*p)->p[j] != NULL) {
				inserir(palavra + i + 1, &(*p)->p[palavra[i] - 'a']);
				return;
			}
		}
		(*p)->flag = realloc ((*p)->flag, lenpalavra + 1);
		for (j = lenprefixo + 1; j < lenpalavra; j++)
			(*p)->flag[j] = 0;
		(*p)->flag[j] = 1;
		free((*p)->prefixo);
		(*p)->prefixo = strdup(palavra);
	}
} 

void imprimir (struct nodo *p) {
	
	int i, j, lenprefixo;
	
	if (p->prefixo[0] == '\0') {
		if (p->flag[0] == -1){
			j = 0;
			for (i=0; i<26; i++){
				if (p->p[i] != NULL){
					j++;
				}
				if (j > 1)
					break;
			}
			if (j == 0){
				return;
			}
			if (j > 1)
				printf("[");
			for (i=0; i<26; i++){
				if (p->p[i] != NULL){
					if (p->p[i]->flag[0]) 
						printf("[%c", (char)(65 + i));
					else
						printf("[%c", (char)(97 + i));
					imprimir (p->p[i]);
					printf("]");
				}
			}
			if (j > 1)
				printf("]");
			return;
		}
	}
	else {
		lenprefixo = strlen(p->prefixo);
		for (i=0; i<lenprefixo; i++){
			if (p->flag[i+1]){
				putchar (toupper(p->prefixo[i]));
			}
			else 
				putchar (p->prefixo[i]);
		}
	}
	for (i=0; i<26; i++) {
		if (p->p[i] != NULL) {
			if (p->p[i]->flag[0]) 
				printf("[%c", (char)(65 + i));
			else
				printf("[%c", (char)(97 + i));
			imprimir (p->p[i]);
			printf("]");
		}
	}
}

void imprimir2 (char *palavra, struct nodo *p) {
	
	int i, lenprefixo, lenpalavra;
	char *aux; 
	
	if (p->prefixo[0] == '\0') 
		if (p->flag[0] == 1)
			printf("%s\n", palavra);
	lenprefixo = strlen(p->prefixo);	
	if (lenprefixo > 0) {
		for (i=0; i<=lenprefixo; i++){
			if (p->flag[i]){
				printf("%s%.*s\n", palavra, i, p->prefixo);
				}
		}
	}
	lenpalavra = strlen(palavra);
	aux = calloc (lenpalavra + lenpalavra +1, sizeof(char *));
	strcat(strcat(aux,palavra), p->prefixo);
	for (i=0; i<26; i++) {
		if (p->p[i] != NULL) {
			aux[lenpalavra + lenprefixo] = (char)(97 + i);
			imprimir2 (aux, p->p[i]);
		}
	}
}
