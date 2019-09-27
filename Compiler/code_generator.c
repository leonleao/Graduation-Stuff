int geracaoCodigo(struct nodo* raiz, int var) {
	
	struct nodo* impressao;
	int i = 0, j = 0, aux = var;
	
	for (i = 0; i < 9 ; i++){
		if (!strcmp(raiz->elemento, gword[i])){
		  break;
		}
	}
	
	switch (i){
	
		case 0:
			break;
			
		case 1:
			break;
			
		case 2:
			break;
			
		case 3:
			break;
			
		case 4:
			return tratarVar(raiz);
			
		case 5:
			tratarIf(raiz);
			return -1;
			
		case 6:
			tratarWhile(raiz);
			return -1;
			
		case 7:
			tratarAtrib(raiz);
			return -1;
			
		case 8:
			tratarPrint(raiz);
			return -1;
			
		default:
			printf("Compilador com problema\n");
			exit(0);
	
	}
	
	while (raiz->operando[j] != NULL){
		impressao = raiz->operando[j];
		if (aux == 0){
			if (!strcmp(impressao->elemento, "var")) {
				if (geracaoCodigo(impressao, 0) == 1) {
					impressao->elemento = strdup("=");
				}
			}
			else {
				aux = 1;
				j = -1;
			}
		}
		
		else 
			if (strcmp(impressao->elemento, "var"))
				geracaoCodigo(impressao, 1);
			
		j++;
		
		if (aux == 0 && raiz->operando[j] == NULL) {
			aux = 1;
			j = 0;			
		}
	}
	
	if (i == 0) {
		
		if (data == 0 || data == 1){
			fprintf(saida, "\n");
			printf("\n");
			fprintf(saida, ".text\n");
			printf(".text\n");
			fprintf(saida, ".globl  main\n");
			printf(".globl  main\n");
			fprintf(saida, "\n");
			printf("\n");
			fprintf(saida, "main:\n");
			printf("main:\n");
			fprintf(saida, "\n");
			printf("\n");
			data = -1;
		}
		
		fprintf(saida, "\n");
		printf("\n");
		fprintf(saida, "\tli $v0, 10  \n");
		printf("\tli $v0, 10  \n");
		
		fprintf(saida, "\tsyscall\n");
		printf("\tsyscall\n");
	}
	
	
}

int tratarVar(struct nodo* raiz) {
	
	if (!data){
		fprintf(saida, ".data\n");
		printf(".data\n");
		data = 1;
	}
	
	if (raiz->operando[1] == NULL) {
	
		fprintf(saida, "\t%s: .word  0 # cria uma variavel inteira com valor inicial 0\n",raiz->operando[0]->elemento);
		printf("\t%s: .word  0 # cria uma variavel inteira com valor inicial 0\n",raiz->operando[0]->elemento);
		return 0;
	}
	else {
		int i;
		struct nodo* sinal = raiz->operando[1];
		
		if (raiz->operando[1]->operando[0] != NULL) {
			fprintf(saida, "\t%s: .word  0 # cria uma variavel inteira com valor inicial 0\n",raiz->operando[0]->elemento);
			printf("\t%s: .word  0 # cria uma variavel inteira com valor inicial 0\n",raiz->operando[0]->elemento);
			return 1;
		}
		
		fprintf(saida, "\t%s: .word  %s # cria uma variavel inteira com valor inicial %s\n",raiz->operando[0]->elemento,raiz->operando[1]->elemento, raiz->operando[1]->elemento);
		printf("\t%s: .word  %s # cria uma variavel inteira com valor inicial %s\n",raiz->operando[0]->elemento,raiz->operando[1]->elemento, raiz->operando[1]->elemento);
		return 0;
	}
	
}

void tratarIf(struct nodo* raiz) {
	
	int aux;
	
	if (data == 0 || data == 1){
		fprintf(saida, "\n");
		printf("\n");
		fprintf(saida, ".text\n");
		printf(".text\n");
		fprintf(saida, ".globl  main\n");
		printf(".globl  main\n");
		fprintf(saida, "\n");
		printf("\n");
		fprintf(saida, "main:\n");
		printf("main:\n");
		fprintf(saida, "\n");
		printf("\n");
		data = -1;
	}
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Inicio if\n");
	printf("\t#Inicio if\n");
	
	tratarExp (raiz->operando[0], 0);
	
	fprintf(saida, "\n");
	printf("\n");
	
	printf("\tlw $t1, 8($sp)\n");
	fprintf(saida, "\tlw $t1, 8($sp)\n");
		
	printf("\taddiu $sp, $sp, 8\n");
	fprintf(saida, "\taddiu $sp, $sp, 8\n");
	
	printf("\tli $t0, 1\n");
	fprintf(saida, "\tli $t0, 1\n");
	
	aux = labels;
	labels++;
	
	printf("\tbne $t0,$t1,false%d\n", aux);
	fprintf(saida, "\tbne $t0,$t1,false%d\n", aux);
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Inicio then\n");
	printf("\t#Inicio then\n");
		
	geracaoCodigo(raiz->operando[1], 1);
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Fim then\n");
	printf("\t#Fim then\n");	
	
	fprintf(saida, "\n");
	printf("\n");
	
	printf("\tb end%d\n", aux);
	fprintf(saida, "\tb end%d\n", aux);
	
	printf("\tfalse%d:\n", aux);
	fprintf(saida, "\tfalse%d:\n", aux);
	
	if (raiz->operando[2] != NULL){
		
		fprintf(saida, "\n");
		printf("\n");
		
		fprintf(saida, "\t#Inicio else\n");
		printf("\t#Inicio else\n");
	
		geracaoCodigo(raiz->operando[2], 1);
		
		fprintf(saida, "\n");
		printf("\n");
		
		fprintf(saida, "\t#Fim else\n");
		printf("\t#Fim else\n");	
		
		fprintf(saida, "\n");
		printf("\n");
	
	}
	
	printf("\tend%d:\n", aux);
	fprintf(saida, "\tend%d:\n", aux);
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Fim if\n");
	printf("\t#Fim if\n");
	
	return;
	
}

void tratarWhile(struct nodo* raiz) {
	
	int aux;
	
	if (data == 0 || data == 1){
		fprintf(saida, "\n");
		printf("\n");
		fprintf(saida, ".text\n");
		printf(".text\n");
		fprintf(saida, ".globl  main\n");
		printf(".globl  main\n");
		fprintf(saida, "\n");
		printf("\n");
		fprintf(saida, "main:\n");
		printf("main:\n");
		fprintf(saida, "\n");
		printf("\n");
		data = -1;
	}
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Inicio While\n");
	printf("\t#Inicio While\n");
	
	aux = labels;
	labels++;
	
	printf("\ttrue%d:\n", aux);
	fprintf(saida, "\ttrue%d:\n", aux);
	
	tratarExp (raiz->operando[0], 0);
	
	fprintf(saida, "\n");
	printf("\n");
	
	printf("\tlw $t1, 8($sp)\n");
	fprintf(saida, "\tlw $t1, 8($sp)\n");
		
	printf("\taddiu $sp, $sp, 8\n");
	fprintf(saida, "\taddiu $sp, $sp, 8\n");
	
	printf("\tli $t0, 1\n");
	fprintf(saida, "\tli $t0, 1\n");
	
	printf("\tbne $t0,$t1,end%d\n", aux);
	fprintf(saida, "\tbne $t0,$t1,end%d\n", aux);
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Inicio do\n");
	printf("\t#Inicio do\n");
		
	geracaoCodigo(raiz->operando[1], 1);
	
	fprintf(saida, "\n");
	printf("\n");
		
	fprintf(saida, "\t#Fim do\n");
	printf("\t#Fim do\n");	
		
	fprintf(saida, "\n");
	printf("\n");
	
	printf("\tb true%d\n", aux);
	fprintf(saida, "\tb true%d\n", aux);
	
	printf("\tend%d:\n", aux);
	fprintf(saida, "\tend%d:\n", aux);
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Fim While\n");
	printf("\t#Fim While\n");
	
	
	return;
	
}

void tratarAtrib(struct nodo* raiz) {
	
	if (data == 0 || data == 1){
		fprintf(saida, "\n");
		printf("\n");
		fprintf(saida, ".text\n");
		printf(".text\n");
		fprintf(saida, ".globl  main\n");
		printf(".globl  main\n");
		fprintf(saida, "\n");
		printf("\n");
		fprintf(saida, "main:\n");
		printf("main:\n");
		fprintf(saida, "\n");
		printf("\n");
		data = -1;
	}
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Inicio ATRIB\n");
	printf("\t#Inicio ATRIB\n");
	
	tratarExp (raiz->operando[1], 0);
	
	fprintf(saida, "\n");
	printf("\n");
	
	printf("\taddiu $sp, $sp, 8\n");
	fprintf(saida, "\taddiu $sp, $sp, 8\n");
	
	printf("\tsw $a0,  %s\n",raiz->operando[0]->elemento);
	fprintf(saida, "\tsw $a0,  %s\n",raiz->operando[0]->elemento);
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Fim ATRIB\n");
	printf("\t#Fim ATRIB\n");
	
	
	
}

void tratarPrint(struct nodo* raiz){
	
	if (data == 0 || data == 1){
		fprintf(saida, "\n");
		printf("\n");
		fprintf(saida, ".text\n");
		printf(".text\n");
		fprintf(saida, ".globl  main\n");
		printf(".globl  main\n");
		fprintf(saida, "\n");
		printf("\n");
		fprintf(saida, "main:\n");
		printf("main:\n");
		fprintf(saida, "\n");
		printf("\n");
		data = -1;
	}
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Inicio Print\n");
	printf("\t#Inicio Print\n");
	
	tratarExp(raiz->operando[0], 0);
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\tli $v0, 1  \n");
	printf("\tli $v0, 1  \n");
		
	fprintf(saida, "\tsyscall\n");
	printf("\tsyscall\n");
	
	fprintf(saida, "\n");
	printf("\n");
	
	fprintf(saida, "\t#Fim Print\n");
	printf("\t#Fim Print\n");
	
}

void tratarExp(struct nodo* raiz, int lado) {
	
	if (raiz == NULL)
		return;
	
	tratarExp(raiz->operando[0], 0);
	
	if ((raiz->elemento[0] >= 48 && raiz->elemento[0] <= 57)  ) {
	
		printf("\n");
		fprintf(saida, "\n");
	
		printf("\t# load/push %s\n", raiz->elemento);
		fprintf(saida, "\t# load/push %s\n", raiz->elemento);
	
		printf("\tli $a0, 1\n");
		fprintf(saida, "\tli $a0, 1\n");
		
		printf("\tsw $a0, 0($sp)\n");
		fprintf(saida, "\tsw $a0, 0($sp)\n");
		
		printf("\taddiu $sp, $sp, -4\n");
		fprintf(saida, "\taddiu $sp, $sp, -4\n");
	
		printf("\tli $a0, %s\n",raiz->elemento);
		fprintf(saida, "\tli $a0, %s\n",raiz->elemento);
		
		printf("\tsw $a0, 0($sp)\n");
		fprintf(saida, "\tsw $a0, 0($sp)\n");
		
		printf("\taddiu $sp, $sp, -4\n");
		fprintf(saida, "\taddiu $sp, $sp, -4\n");	
		
		return;
	
	} else if ((raiz->elemento[0] >= 65 && raiz->elemento[0] <= 90) || (raiz->elemento[0] >= 97 && raiz->elemento[0] <= 122) || raiz->elemento[0] == 95 ) {
	
		printf("\n");
		fprintf(saida, "\n");
	
		printf("\t# load/push valor da variavel %s\n", raiz->elemento);
		fprintf(saida, "\t# load/push valor da variavel %s\n", raiz->elemento);
	
		printf("\tli $a0, 1\n");
		fprintf(saida, "\tli $a0, 1\n");
		
		printf("\tsw $a0, 0($sp)\n");
		fprintf(saida, "\tsw $a0, 0($sp)\n");
		
		printf("\taddiu $sp, $sp, -4\n");
		fprintf(saida, "\taddiu $sp, $sp, -4\n");
		
		printf("\tlw $a0,  %s\n",raiz->elemento);
		fprintf(saida, "\tlw $a0,  %s\n",raiz->elemento);
		
		printf("\tsw $a0, 0($sp)\n");
		fprintf(saida, "\tsw $a0, 0($sp)\n");
		
		printf("\taddiu $sp, $sp, -4\n");
		fprintf(saida, "\taddiu $sp, $sp, -4\n");	
		
		
		return;
	
	}
	
	tratarExp(raiz->operando[1], 1);	
	
	printf("\n");
	fprintf(saida, "\n");
	
	printf("\t# pop valores da pilha\n");
	fprintf(saida, "\t# pop valores da pilha\n");
	
	if (raiz->operando[1] != NULL) {
	
		printf("\tlw $t1, 8($sp)\n");
		fprintf(saida, "\tlw $t1, 8($sp)\n");
		printf("\tlw $t0, 12($sp)\n");
		fprintf(saida, "\tlw $t0, 12($sp)\n");
		printf("\tlw $t2, 16($sp)\n");
		fprintf(saida, "\tlw $t2, 16($sp)\n");
		printf("\taddiu $sp, $sp, 16\n");
		fprintf(saida, "\taddiu $sp, $sp, 16\n");

	}	
	else {
		printf("\tlw $t2, 8($sp)\n");
		fprintf(saida, "\tlw $t2, 8($sp)\n");
		
		printf("\taddiu $sp, $sp, 8\n");
		fprintf(saida, "\taddiu $sp, $sp, 8\n");
	}
	
	//Valor do elemento a esquerda em t0 e da direita em a0. Ponteiro volta à posição normal. t1 e t2 contem os valores verdade.
	
	
	int i,j;
		
	for (i = 0; i < 13; i++){
		if (!strcmp(raiz->elemento, expword[i])){
		  break;
		}
	}
	
	switch (i) {
		
		case 0:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# add/push valor verdade e resultado\n");
			fprintf(saida, "\t# add/push valor verdade e resultado\n");
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
		
			printf("\taddu $a0, $t0, $a0\n");
			fprintf(saida, "\taddu $a0, $t0, $a0\n");
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
		
			return;
			
		case 1: 
		
			if (raiz->operando[1] != NULL) {
				
				printf("\n");
				fprintf(saida, "\n");
				
				printf("\t# sub/push valor verdade e resultado\n");
				fprintf(saida, "\t# sub/push valor verdade e resultado\n");
				
				printf("\tli $t1, 1\n");
				fprintf(saida, "\tli $t1, 1\n");
				
				printf("\tsw $t1, 0($sp)\n");
				fprintf(saida, "\tsw $t1, 0($sp)\n");
				
				printf("\taddiu $sp, $sp, -4\n");
				fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
				printf("\tsubu $a0, $t0, $a0\n");
				fprintf(saida, "\tsubu $a0, $t0, $a0\n");
				
				printf("\tsw $a0, 0($sp)\n");
				fprintf(saida, "\tsw $a0, 0($sp)\n");
				
				printf("\taddiu $sp, $sp, -4\n");
				fprintf(saida, "\taddiu $sp, $sp, -4\n");
			}
			
			else {
				
				printf("\n");
				fprintf(saida, "\n");
				
				printf("\t# neg/push valor verdade e resultado\n");
				fprintf(saida, "\t# neg/push valor verdade e resultado\n");
				
				printf("\tli $t1, 1\n");
				fprintf(saida, "\tli $t1, 1\n");
				
				printf("\tsw $t1, 0($sp)\n");
				fprintf(saida, "\tsw $t1, 0($sp)\n");
				
				printf("\taddiu $sp, $sp, -4\n");
				fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
				printf("\tnegu $a0, $a0\n");
				fprintf(saida, "\tnegu $a0, $a0\n");
				
				printf("\tsw $a0, 0($sp)\n");
				fprintf(saida, "\tsw $a0, 0($sp)\n");
				
				printf("\taddiu $sp, $sp, -4\n");
				fprintf(saida, "\taddiu $sp, $sp, -4\n");
			}
			
			return;
			
		case 2:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# mul/push valor verdade e resultado\n");
			fprintf(saida, "\t# mul/push valor verdade e resultado\n");
		
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
		
			printf("\tmul $a0, $t0, $a0\n");
			fprintf(saida, "\tmul $a0, $t0, $a0\n");
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			return;
			
		case 3:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# div/push valor verdade e resultado\n");
			fprintf(saida, "\t# div/push valor verdade e resultado\n");
		
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
		
			printf("\tdiv $t0, $a0\n");
			fprintf(saida, "\tdiv $t0, $a0\n");
			
			printf("\tmflo $a0\n");
			fprintf(saida, "\tmflo $a0\n");
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			return;
			
		case 4:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# </push valor verdade e resultado\n");
			fprintf(saida, "\t# </push valor verdade e resultado\n");
		
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tbeq $t1, $t2, false%d\n", labels);
			fprintf(saida, "\tbeq $t1, $t2, false%d\n", labels);		
		
			printf("\tblt $t0,$a0,true%d\n", labels);
			fprintf(saida, "\tblt $t0,$a0,true%d\n", labels);
			
			printf("\tfalse%d:\n", labels);
			fprintf(saida, "\tfalse%d:\n", labels);
			
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tb end%d\n", labels);
			fprintf(saida, "\tb end%d\n", labels);
			
			printf("\ttrue%d:\n", labels);
			fprintf(saida, "\ttrue%d:\n", labels);
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tend%d:\n", labels);
			fprintf(saida, "\tend%d:\n", labels);
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			if (lado != 0) {
				
				printf("\tmove $a0, $t0\n");
				fprintf(saida, "\tmove $a0, $t0\n");
				
			}
				
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
				
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			labels++;
			
			return;
			
		case 5:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# <=/push valor verdade e resultado\n");
			fprintf(saida, "\t# <=/push valor verdade e resultado\n");
		
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tbeq $t1, $t2, false%d\n", labels);
			fprintf(saida, "\tbeq $t1, $t2, false%d\n", labels);		
		
			printf("\tble $t0,$a0,true%d\n", labels);
			fprintf(saida, "\tble $t0,$a0,true%d\n", labels);
			
			printf("\tfalse%d:\n", labels);
			fprintf(saida, "\tfalse%d:\n", labels);
			
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tb end%d\n", labels);
			fprintf(saida, "\tb end%d\n", labels);
			
			printf("\ttrue%d:\n", labels);
			fprintf(saida, "\ttrue%d:\n", labels);
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tend%d:\n", labels);
			fprintf(saida, "\tend%d:\n", labels);
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			if (lado != 0) {
				
				printf("\tmove $a0, $t0\n");
				fprintf(saida, "\tmove $a0, $t0\n");
				
			}
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			labels++;
			
			return;
			
		case 6:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# >/push valor verdade e resultado\n");
			fprintf(saida, "\t# >/push valor verdade e resultado\n");
		
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tbeq $t1, $t2, false%d\n", labels);
			fprintf(saida, "\tbeq $t1, $t2, false%d\n", labels);		
		
			printf("\tbgt $t0,$a0,true%d\n", labels);
			fprintf(saida, "\tbgt $t0,$a0,true%d\n", labels);
			
			printf("\tfalse%d:\n", labels);
			fprintf(saida, "\tfalse%d:\n", labels);
			
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tb end%d\n", labels);
			fprintf(saida, "\tb end%d\n", labels);
			
			printf("\ttrue%d:\n", labels);
			fprintf(saida, "\ttrue%d:\n", labels);
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tend%d:\n", labels);
			fprintf(saida, "\tend%d:\n", labels);
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			if (lado != 0) {
				
				printf("\tmove $a0, $t0\n");
				fprintf(saida, "\tmove $a0, $t0\n");
				
			}
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			labels++;
			
			return;
			
		case 7:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# >=/push valor verdade e resultado\n");
			fprintf(saida, "\t# >=/push valor verdade e resultado\n");
		
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tbeq $t1, $t2, false%d\n", labels);
			fprintf(saida, "\tbeq $t1, $t2, false%d\n", labels);		
		
			printf("\tbge $t0,$a0,true%d\n", labels);
			fprintf(saida, "\tbge $t0,$a0,true%d\n", labels);
			
			printf("\tfalse%d:\n", labels);
			fprintf(saida, "\tfalse%d:\n", labels);
			
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tb end%d\n", labels);
			fprintf(saida, "\tb end%d\n", labels);
			
			printf("\ttrue%d:\n", labels);
			fprintf(saida, "\ttrue%d:\n", labels);
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tend%d:\n", labels);
			fprintf(saida, "\tend%d:\n", labels);
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			if (lado != 0) {
				
				printf("\tmove $a0, $t0\n");
				fprintf(saida, "\tmove $a0, $t0\n");
				
			}
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			labels++;
			
			return;
			
		case 8:
		
			printf("\n");
			fprintf(saida, "\n");
					
			for (j = 0; j < 2; j++) {
				for (i = 0; i < 13; i++){
					if (!strcmp(raiz->operando[j]->elemento, expword[i])){
					  break;
					}
				}
				if (i >= 4 && i <= 7) {
					break;
				}
				
			}
						
			if (j == 0) {
				for (i = 0; i < 13; i++){
					if (!strcmp(raiz->operando[1]->elemento, expword[i])){
					  break;
					}
				}
				
				if (i >= 4 && i <= 7)
					j = 3;
				else {
					
					printf("\t# ==/push valor verdade e resultado\n");
					fprintf(saida, "\t# ==/push valor verdade e resultado\n");
					
					printf("\tbeq $t2,$a0,true%d\n", labels);
					fprintf(saida, "\tbeq $t2,$a0,true%d\n", labels);
				
				}
			}
			
			if ( j == 1) {
				
				printf("\t# ==/push valor verdade e resultado\n");
				fprintf(saida, "\t# ==/push valor verdade e resultado\n");
				
				printf("\tbeq $t0,$t1,true%d\n", labels);
				fprintf(saida, "\tbeq $t0,$t1,true%d\n", labels);
				
			}
			
			else if (j == 2) {
				
			
				printf("\t# ==/push valor verdade e resultado\n");
				fprintf(saida, "\t# ==/push valor verdade e resultado\n");
			
				printf("\tbeq $t0,$a0,true%d\n", labels);
				fprintf(saida, "\tbeq $t0,$a0,true%d\n", labels);
			
			}
			else {
				printf("\t# XNOR/push valor verdade e resultado\n");
				fprintf(saida, "\t# XNOR/push valor verdade e resultado\n");
			
				printf("\tbeq $t2, $t1, true%d\n", labels);
				fprintf(saida, "\tbeq $t2, $t1, true%d\n", labels);
			}
			
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tb end%d\n", labels);
			fprintf(saida, "\tb end%d\n", labels);
			
			printf("\ttrue%d:\n", labels);
			fprintf(saida, "\ttrue%d:\n", labels);
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tend%d:\n", labels);
			fprintf(saida, "\tend%d:\n", labels);
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			labels++;
			
			return;
			
		case 9:
		
			printf("\n");
			fprintf(saida, "\n");
			
			for (j = 0; j < 2; j++) {
				for (i = 0; i < 13; i++){
					if (!strcmp(raiz->operando[j]->elemento, expword[i])){
					  break;
					}
				}
				if (i >= 4 && i <= 7) 
					break;
				
			}
			
			if (j == 0) {
				for (i = 0; i < 13; i++){
					if (!strcmp(raiz->operando[1]->elemento, expword[i])){
					  break;
					}
				}
				
				if (i >= 4 && i <= 7)
					j = 3;
				else {
					
					printf("\t# ~=/push valor verdade e resultado\n");
					fprintf(saida, "\t# ~=/push valor verdade e resultado\n");
					
					printf("\tbne $t2,$a0,true%d\n", labels);
					fprintf(saida, "\tbne $t2,$a0,true%d\n", labels);
				
				}
			}
			
			if ( j == 1) {
				
				printf("\t# ~=/push valor verdade e resultado\n");
				fprintf(saida, "\t# ~=/push valor verdade e resultado\n");
				
				printf("\tbne $t0,$t1,true%d\n", labels);
				fprintf(saida, "\tbne $t0,$t1,true%d\n", labels);
				
			}
			
			else if (j == 2) {
			
				printf("\t# ~=/push valor verdade e resultado\n");
				fprintf(saida, "\t# ~=/push valor verdade e resultado\n");
			
				printf("\tbne $t0,$a0,true%d\n", labels);
				fprintf(saida, "\tbne $t0,$a0,true%d\n", labels);
			
			}
			else {
				printf("\t# XOR/push valor verdade e resultado\n");
				fprintf(saida, "\t# XOR/push valor verdade e resultado\n");
			
				printf("\tbne $t2, $t1, true%d\n", labels);
				fprintf(saida, "\tbne $t2, $t1, true%d\n", labels);
			}
			
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tb end%d\n", labels);
			fprintf(saida, "\tb end%d\n", labels);
			
			printf("\ttrue%d:\n", labels);
			fprintf(saida, "\ttrue%d:\n", labels);
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tend%d:\n", labels);
			fprintf(saida, "\tend%d:\n", labels);
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			labels++;
			
			return;
			
		case 10:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# and/push valor verdade e resultado\n");
			fprintf(saida, "\t# and/push valor verdade e resultado\n");
		
			printf("\tli $t3, 0\n");
			fprintf(saida, "\tli $t3, 0\n");
			
			printf("\tbeq $t1, $t3, false%d\n", labels);
			fprintf(saida, "\tbeq $t1, $t3, false%d\n", labels);

			printf("\tbeq $t2, $t3, false%d\n", labels);
			fprintf(saida, "\tbeq $t2, $t3, false%d\n", labels);			
		
			printf("\tb true%d\n", labels);
			fprintf(saida, "\tb true%d\n", labels);
			
			printf("\tfalse%d:\n", labels);
			fprintf(saida, "\tfalse%d:\n", labels);
			
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tb end%d\n", labels);
			fprintf(saida, "\tb end%d\n", labels);
			
			printf("\ttrue%d:\n", labels);
			fprintf(saida, "\ttrue%d:\n", labels);
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tend%d:\n", labels);
			fprintf(saida, "\tend%d:\n", labels);
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			labels++;
			
			return;
			
		case 11:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# or/push valor verdade e resultado\n");
			fprintf(saida, "\t# or/push valor verdade e resultado\n");
		
			printf("\tli $t3, 1\n");
			fprintf(saida, "\tli $t3, 1\n");
			
			printf("\tbeq $t1, $t3, true%d\n", labels);
			fprintf(saida, "\tbeq $t1, $t3, true%d\n", labels);

			printf("\tbeq $t2, $t3, true%d\n", labels);
			fprintf(saida, "\tbeq $t2, $t3, true%d\n", labels);			
			
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tb end%d\n", labels);
			fprintf(saida, "\tb end%d\n", labels);
			
			printf("\ttrue%d:\n", labels);
			fprintf(saida, "\ttrue%d:\n", labels);
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tend%d:\n", labels);
			fprintf(saida, "\tend%d:\n", labels);
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			labels++;
			
			return;
			
		case 12:
		
			printf("\n");
			fprintf(saida, "\n");
			
			printf("\t# not/push valor verdade e resultado\n");
			fprintf(saida, "\t# not/push valor verdade e resultado\n");
		
			printf("\tli $t1, 0\n");
			fprintf(saida, "\tli $t1, 0\n");
			
			printf("\tbeq $t1, $t2, true%d\n", labels);
			fprintf(saida, "\tbeq $t1, $t2, true%d\n", labels);		
			
			printf("\tb end%d\n", labels);
			fprintf(saida, "\tb end%d\n", labels);
			
			printf("\ttrue%d:\n", labels);
			fprintf(saida, "\ttrue%d:\n", labels);
			
			printf("\tli $t1, 1\n");
			fprintf(saida, "\tli $t1, 1\n");
			
			printf("\tend%d:\n", labels);
			fprintf(saida, "\tend%d:\n", labels);
			
			printf("\tsw $t1, 0($sp)\n");
			fprintf(saida, "\tsw $t1, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			printf("\tsw $a0, 0($sp)\n");
			fprintf(saida, "\tsw $a0, 0($sp)\n");
			
			printf("\taddiu $sp, $sp, -4\n");
			fprintf(saida, "\taddiu $sp, $sp, -4\n");
			
			labels++;
			
			return;
		
			
		
	}
	
	
}

