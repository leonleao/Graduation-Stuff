#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

//Constantes e Variaveis globais

#define QNT 18

#define QT 29

#define Nos 10000

static const char *gword[] = {
	"PROGRAMA",
	"then",
	"else",
	"do",
	"var",
	"if",
	"while",
	"=",
	"print",
};

static const char *expword[] = {
	"+",
	"-",
	"*",
	"/",
	"<",
	"<=",
	">",
	">=",
	"==",
	"~=",
	"and",
	"or",
	"not",
};

static const char *kword[] = {
	"if",
	"while",
	"var",
	"else",
	"end",
	"for",
	"then",
	"and",
	"do",
	"or",
	"not",
};

struct token{
  char* tipo;
  char* conteudo;
  
  int linha;
  int coluna;
  
  struct token *prox;
};

struct nodo{
	char* elemento;
	
	struct nodo *operando[Nos];
	
};

struct pilha {
	char* tipo;
	struct pilha *prox;
	
};

int linha = 1;
int coluna = 1;
int tamanhoPrograma = 0;
int data = 0, labels = 0;
char token[200], aux[2];
struct token* variaveis;
struct token* tabela[QNT+1][QT+1];
FILE *saida;

//Prototipos

/* FUNÇÕES GERAIS */

void lerPrograma (char* arquivo, char** programa);	//Verifica extensao do arquivo, obtem tamanho e conteudo do arquivo lua.
struct token* criarLista ();	//Cria uma lista nova
void gerarToken (struct token** lista, char* tipo, char* nome);		//Gera token

/* FUNÇÕES DO ANALISADOR LEXICO */ 

void analisadorLexico (struct token** lista, char* programa);	// Analisa lexicamente o arquivo.
void numero (struct token** lista, char* programa, int* i);		//Verifica se o numeral segue o padrão da linguagem e poe ele inteiro como um unico token.
void nome (struct token** lista, char* programa, int* i);		//Gera um token, verifica se é palavra reservada e guarda na lista.
void imprimirLista (struct token* lista);

/* FUNÇÕES DO ANALISADOR SINTÁTICO */

void analisadorSintatico (struct token* lista); //Verifica a gramática do programa e gera arvore
void iniciarTabela(); // Cria a tabela parse
void inserirPilha (struct pilha **g, char* tipo); //Insere um elemento na pilha
char *retirarpilha (struct pilha **g); //Retira o elemento topo da pilha
int terminal(char *token); //Obtem o ID do terminal, caso seja (cc retorna -1)
int naoTerminal(char *token); //Obtem o ID do não-terminal, caso seja (cc retorna -1)
void freeListaToken (struct token **lista);
void freeTabela();

/* GERAÇÃO DE ARVORE */

struct nodo* criarArvore();
void imprimirArvore(struct nodo* raiz);
void inserirArvore (struct token* inicio, struct token* fim, struct nodo** raiz);
void gerarArvore (struct token** lista, struct nodo** raiz);
void inserirExp (struct token** tokencopia, struct nodo** raiz, int tipo);

/* GERAÇÃO DE CODIGO */

int geracaoCodigo(struct nodo* raiz, int var);
int tratarVar(struct nodo* raiz);
void tratarIf(struct nodo* raiz);
void tratarWhile(struct nodo* raiz);
void tratarAtrib(struct nodo* raiz);
void tratarPrint(struct nodo* raiz);
void tratarExp(struct nodo* raiz, int lado);


int main(int argc, char** argv){
    
	if (argc != 3){
		fprintf(stderr, "Numero de argumentos invalido. Formato: .\\comp <arq_entrada> <arq_saida_codigo_asm>\n");
		exit(1);
	}
	
	char* programaCopia;
	
	//Obter uma copia do programa sem modifica-lo.
	
	lerPrograma(argv[1], &programaCopia);
	
	//Cria uma nova lista.
	
	struct token *lista = criarLista();
	
	//Começa a analise.
	
	analisadorLexico (&lista, programaCopia);
	
	free(programaCopia);
	
	//Imprime a lista
	
	imprimirLista(lista);
	printf("\n");
	fprintf(saida, "\n");
	
	analisadorSintatico(lista);
	printf("\n");
	
	struct nodo *raiz = criarArvore();
	
	struct token *tokencopia = lista->prox;
	
	variaveis = criarLista();
	
	gerarArvore(&tokencopia, &raiz);
	
	printf("Arvore:\n");
	fprintf(saida, "Arvore:\n");
	
	imprimirArvore(raiz);
	
	fclose(saida);
	
	printf("\n");
	
 	if ((saida=fopen(argv[2],"w"))==NULL){
		fprintf(stderr, "Arquivo não pode ser aberto\n");
		exit(1);  
	}
	
	geracaoCodigo(raiz,0);
	
	fclose(saida);
	
	return 1;
}

void lerPrograma (char* arquivo, char** programa) {
	
	//Verificacao da extensao.
	
	char nomeArquivo [strlen(arquivo)];
	strcpy(nomeArquivo, arquivo);
	
	FILE *fp;
	
	int tarquivo;
	
	if ((fp=fopen(nomeArquivo,"r"))==NULL){
		fprintf(stderr, "Arquivo não pode ser aberto\n");
		exit(1);  
	}
	
	fseek(fp, 0, SEEK_END);
	tarquivo=ftell(fp);
	fseek(fp, 0, SEEK_SET);
		
	//Criar arquivo zerado, mas do mesmo tamanho do qual estamos lendo.
		
	*programa = (char*)calloc(tarquivo+1, sizeof(char));
	fread(*programa, 1, tarquivo, fp);
	
	//obter tamanho do arquivo
	
	tamanhoPrograma = strlen(*programa);
	
	
	//Dados obtidos, fechando o arquivo.

	fclose(fp);
} 

struct token* criarLista () {
	
	struct token* lista;
	
	lista = calloc (1, sizeof(struct token));
	lista->tipo = strdup ("INICIO");	
	
	return lista;
}

struct nodo *criarArvore() {
	
	struct nodo* raiz = calloc (1, sizeof(struct nodo));
	
	raiz->elemento = strdup ("PROGRAMA");
	
	return raiz;
	
}

void analisadorLexico (struct token** lista, char* programa) {
	
	int i = 0;
	
	
	while (i < tamanhoPrograma){
		switch (programa[i]){
			
			//Itens lexicos da linguagem lua e cadeia literais.
			
			case '(':
				strcpy(token, "\0");
				i++;
				coluna++;
				gerarToken (lista, "PAR ESQ", "(");
				break;
			case ')':
				strcpy(token, "\0");
				i++;
				coluna++;
				gerarToken (lista, "PAR DIR", ")");
				break;
			case ';':
				strcpy(token, "\0");
				i++;
				coluna++;
				gerarToken (lista, "PTO VIRG", ";");
				break;
			case ',':
				strcpy(token, "\0");
				i++;
				coluna++;
				gerarToken (lista, "VIRGULA", ",");
				break;
			case '+':
				strcpy(token, "\0");
				i++;
				coluna++;
				gerarToken (lista, "ADD", "+");
				break;
			case '-':
				strcpy(token, "\0");
				i++;
				coluna++;
				gerarToken (lista, "SUB/UNM", "-");
				break;
			case '*':
				strcpy(token, "\0");
				i++;
				coluna++;
				gerarToken (lista, "MUL", "*");
				break;	
			case '/':
				strcpy(token, "\0");
				i++;
				coluna++;
				gerarToken (lista, "DIV", "/");
				break;	
			case '=':
				strcpy(token, "\0");
				i++;
				coluna++;
				if (programa[i] == '='){ 
					i++;
					coluna++;
					gerarToken (lista, "EQ", "==");
				}
				else
					gerarToken(lista, "ATRIB", "=");
				break;
			case '~':
				strcpy(token, "\0");
				i++;
				coluna++;
				if (programa[i] == '='){ 
					i++;
					coluna++;
					gerarToken (lista, "NE", "~=");
				}
				else {
					fprintf(stderr, "linha: %d, coluna: %d: Comando nao existente na linguagem\n", linha, coluna);
					exit(0);
				}
				break;
			case '<':
				strcpy(token, "\0");
				i++;
				coluna++;
				if (programa[i] == '='){ 
					i++;
					coluna++;
					gerarToken (lista, "LE", "<=");
				}
				else 
					gerarToken (lista, "LT", "<");
				break;
			case '>':
				strcpy(token, "\0");
				i++;
				coluna++;
				if (programa[i] == '='){ 
					i++;
					coluna++;
					gerarToken (lista, "GE", ">=");
				}
				else 
					gerarToken (lista, "GT", ">");
				break;
				
			//Numerais
				
			case '0': 
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
			case '1':
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
			case '2':
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
			case '3':
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
			case '4':
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
			case '5':
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
			case '6':
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
			case '7':
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
			case '8':
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
			case '9':
				strcpy(token, "\0");
				aux[0] = programa[i];
				aux[1] = '\0';
				strcat(token, aux);
				i++;
				coluna++;
				numero(lista, programa, &i);
				break;
				
			//Espacos em branco
				
			case ' ':
				i++;
				coluna++;
				break;
			case '\t':
				i++;
				coluna++;
				break;
			case '\n':
				i++;
				linha++;
				coluna = 0;
				break;
				
			case '\r':
				i++;
				linha++;
				coluna = 0;
				break;
				
			//Qualquer outra entrada.
				
			default:
				if (((programa[i] >= 65) && (programa[i] <= 90)) || ((programa[i] >= 97) && (programa[i] <= 122)) || programa[i] == '_') {
					strcpy(token, "\0");
					aux[0] = programa[i];
					aux[1] = '\0';
					strcat(token, aux);
					i++;
					coluna++;
					nome(lista, programa, &i);
					break;
				}
				else {
					printf("%c", programa[i]);
					fprintf(stderr, "linha: %d, coluna: %d: Comando nao existente na linguagem\n", linha, coluna);
					exit(0);
				}
		}
		
	}
	
}

void gerarToken (struct token** lista, char* tipo, char* conteudo) {

	int j;
	struct token* ultimoElem = (*lista)->prox;
	
	if (ultimoElem == NULL){
		ultimoElem = calloc (1, sizeof(struct token));
		ultimoElem->conteudo = strdup(conteudo);
		(*lista)->prox = ultimoElem;
	}

	else {
		gerarToken(&ultimoElem, tipo, conteudo);
		return;
	}
	
	if (strcmp(tipo, "NOME"))
		ultimoElem->tipo = strdup(tipo);
	
	else {
		for (j = 0; j<11; j++)
			if (!strcmp(conteudo, kword[j])) 
				break;
		if (j < 11)
			ultimoElem->tipo = strdup("KWORD");
		else 
			ultimoElem->tipo = strdup("NOME");
	}
	
	ultimoElem->linha = linha;
	ultimoElem->coluna = coluna - strlen(conteudo);
	
}

void numero (struct token** lista, char* programa, int* i){
	
	if ((*i) < tamanhoPrograma) {
		
		if ((programa[*i] >= 48) && (programa[*i] <= 57)) {
			aux[0] = programa[*i];
			aux[1] = '\0';
			strcat(token, aux);
			(*i)++;
			coluna++;
			numero (lista, programa, i);
			return;
		}
		else if (programa[*i] < 65 || (programa[*i] > 91 && programa[*i] < 97) || programa[*i] > 122)  {
			gerarToken (lista, "NUM INTEIRO", token);
			return;
		}
		else {
			fprintf(stderr, "linha: %d,coluna: %d: Formato de nome,numeral invalido\n", linha, coluna);
			exit(0);
		}
		
	}		
	
	

	gerarToken (lista, "NUM INTEIRO", token);
	return;
	
	
	
}

void nome (struct token** lista, char* programa, int* i) {
		
	if ( (((*i) < tamanhoPrograma) && (((programa[*i] >= 48) && (programa[*i] <= 57)) || ((programa[*i] >= 65) && (programa[*i] <= 90)) || ((programa[*i] >= 97) && (programa[*i] <= 122)) || (programa[*i] == '_') ) ) ){
		aux[0] = programa[*i];
		aux[1] = '\0';
		strcat(token, aux);
		(*i)++;
		coluna++;
		nome (lista, programa, i);
		return;
	}
	
	gerarToken (lista, "NOME", token);
	return;
}

void imprimirLista (struct token* lista) {
		
	if (!strcmp (lista->tipo,"INICIO"))
		lista = lista->prox;
	else {
		fprintf(stderr, "Estrutura defeituosa\n");
		exit(1);
	}		
	
	if (lista == NULL) {
		fprintf(stderr, "Estrutura defeituosa\n");
		exit(1);
	}
	
	 if ((saida=fopen("saida.txt","w"))==NULL){
		 fprintf(stderr, "Arquivo não pode ser aberto\n");
		 exit(1);  
	 }
	
	printf("Lista de Tokens lexicos\n");
	fprintf(saida, "Lista de Tokens lexicos\n");
	
	while (lista != NULL) {
		
		fprintf(saida, "(%s, \'%s\')\n", lista->tipo, lista->conteudo);
		printf("(%s, \'%s\')\n", lista->tipo, lista->conteudo);
		lista = lista->prox;
	}
}

void iniciarTabela(){
	
	int i,j;
	
	for (i = 0; i <= QNT; i++){
		for (j = 0; j <= QT; j++){
		  tabela[i][j] = criarLista();
		}	
	}
	
	//Gerar terminais
	
	gerarToken(&tabela[0][1], "if", "");
	gerarToken(&tabela[0][2], "for", "");
	gerarToken(&tabela[0][3], "while", "");
	gerarToken(&tabela[0][4], "else", "");
	gerarToken(&tabela[0][5], "end", "");
	gerarToken(&tabela[0][6], "then", "");
	gerarToken(&tabela[0][7], "and", "");
	gerarToken(&tabela[0][8], "do", "");
	gerarToken(&tabela[0][9], "or", "");
	gerarToken(&tabela[0][10], "NOME", "");
	gerarToken(&tabela[0][11], "PAR ESQ", "");
	gerarToken(&tabela[0][12], "PAR DIR", "");
	gerarToken(&tabela[0][13], "PTO VIRG", "");
	gerarToken(&tabela[0][14], "VIRGULA", "");
	gerarToken(&tabela[0][15], "ADD", "");
	gerarToken(&tabela[0][16], "SUB/UNM", "");
	gerarToken(&tabela[0][17], "MUL", "");
	gerarToken(&tabela[0][18], "DIV", "");
	gerarToken(&tabela[0][19], "EQ", "");
	gerarToken(&tabela[0][20], "ATRIB", "");
	gerarToken(&tabela[0][21], "var", "");
	gerarToken(&tabela[0][22], "NE", "");
	gerarToken(&tabela[0][23], "LE", "");
	gerarToken(&tabela[0][24], "LT", "");
	gerarToken(&tabela[0][25], "GE", "");
	gerarToken(&tabela[0][26], "GT", "");
	gerarToken(&tabela[0][27], "NUM INTEIRO", "");
	gerarToken(&tabela[0][28], "not", "");
	
	//Gerar não terminais
	//trecho ::= {comando [`;´]}
	
	gerarToken(&tabela[1][0], "trecho", "");
	
	gerarToken(&tabela[1][1], "comando_seq", "");
	
	gerarToken(&tabela[1][2], "comando_seq", "");
	
	gerarToken(&tabela[1][3], "comando_seq", "");
	
	gerarToken(&tabela[1][10], "comando_seq", "");
	
	gerarToken(&tabela[1][21], "comando_seq", "");
	
	gerarToken(&tabela[1][29], "epsilon", "");
		
	//comando_seq ::=  comando {[;] comando_seq}
	
	gerarToken(&tabela[2][0], "comando_seq", "");
	
	gerarToken(&tabela[2][1], "comando_if", "");
	gerarToken(&tabela[2][1], "op_pto_virgula", "");
	gerarToken(&tabela[2][1], "trecho", "");
	
	gerarToken(&tabela[2][2], "comando_for", "");
	gerarToken(&tabela[2][2], "op_pto_virgula", "");
	gerarToken(&tabela[2][2], "trecho", "");
	
	gerarToken(&tabela[2][3], "comando_while", "");
	gerarToken(&tabela[2][3], "op_pto_virgula", "");
	gerarToken(&tabela[2][3], "trecho", "");
	
	gerarToken(&tabela[2][10], "comando_other", "");
	gerarToken(&tabela[2][10], "op_pto_virgula", "");
	gerarToken(&tabela[2][10], "trecho", "");
	
	gerarToken(&tabela[2][21], "comando_decl", "");
	gerarToken(&tabela[2][21], "op_pto_virgula", "");
	gerarToken(&tabela[2][21], "trecho", "");
	
	gerarToken(&tabela[2][29], "epsilon", "");
	
	//comando_if ::= if exp then bloco [else bloco] end
	
	gerarToken(&tabela[3][0], "comando_if", "");
	
	
	gerarToken(&tabela[3][1], "if", "");
	gerarToken(&tabela[3][1], "comando_exp_seq", "");
	gerarToken(&tabela[3][1], "then", "");
	gerarToken(&tabela[3][1], "trecho", "");
	gerarToken(&tabela[3][1], "comando_else", "");
	gerarToken(&tabela[3][1], "end", "");
	
	//comando_else ::= else bloco
		
	gerarToken(&tabela[4][0], "comando_else", "");
	
	gerarToken(&tabela[4][4], "else", "");
	gerarToken(&tabela[4][4], "trecho", "");
	
	gerarToken(&tabela[4][29], "epsilon", "");
	
	//comando_for ::= for Nome `=´ exp `,´ exp [`,´ exp] do bloco end
	
	gerarToken(&tabela[5][0], "comando_for", "");
	
	
	gerarToken(&tabela[5][2], "for", "");
	gerarToken(&tabela[5][2], "NOME", "");
	gerarToken(&tabela[5][2], "ATRIB", "");
	gerarToken(&tabela[5][2], "comando_exp_seq", "");
	gerarToken(&tabela[5][2], "VIRGULA", "");
	gerarToken(&tabela[5][2], "comando_exp_seq", "");
	gerarToken(&tabela[5][2], "op_for1", "");
	gerarToken(&tabela[5][2], "do", "");
	gerarToken(&tabela[5][2], "trecho", "");
	gerarToken(&tabela[5][2], "end", "");
	
	
	//op_for1 ::= [`,´ exp]
	
	gerarToken(&tabela[6][0], "op_for1", "");
	
	gerarToken(&tabela[6][14], "VIRGULA", "");
	gerarToken(&tabela[6][14], "comando_exp_seq", "");
	
	gerarToken(&tabela[6][29], "epsilon", "");
	
	
	//comando_while ::= while exp do bloco end
	
	gerarToken(&tabela[7][0], "comando_while", "");
	
	
	gerarToken(&tabela[7][3], "while", "");
	gerarToken(&tabela[7][3], "comando_exp_seq", "");
	gerarToken(&tabela[7][3], "do", "");
	gerarToken(&tabela[7][3], "trecho", "");
	gerarToken(&tabela[7][3], "end", "");
		
	//comando_other ::= Nome `=´ exp | chamadadefuncao
	
	gerarToken(&tabela[8][0], "comando_other", "");
	
	gerarToken(&tabela[8][10], "NOME", "");
	gerarToken(&tabela[8][10], "comando_other_decisao", "");
	
	//comando_other_decisao
	
	gerarToken(&tabela[9][0], "comando_other_decisao", "");
	
	gerarToken(&tabela[9][20], "comando_atri", "");

	gerarToken(&tabela[9][11], "comando_args", "");
	
	//comando_atri ::= Nome `=´ exp
	
	gerarToken(&tabela[10][0], "comando_atri", "");
	
	gerarToken(&tabela[10][20], "ATRIB", "");
	gerarToken(&tabela[10][20], "comando_exp_seq", "");

	
	//comando_exp_seq ::= Numero | Nome | chamadadefunção | exp opbin exp | opunaria exp | `(´ exp `)´
	
	gerarToken(&tabela[11][0], "comando_exp_seq", "");
		
	
	gerarToken(&tabela[11][10], "NOME", "");
	gerarToken(&tabela[11][10], "comando_opbin", "");
	
	gerarToken(&tabela[11][27], "NUM INTEIRO", "");
	gerarToken(&tabela[11][27], "comando_opbin", "");
	
	gerarToken(&tabela[11][16], "SUB/UNM", "");
	gerarToken(&tabela[11][16], "comando_exp_seq", "");
	
	gerarToken(&tabela[11][11], "PAR ESQ", "");
	gerarToken(&tabela[11][11], "comando_exp_seq", "");
	gerarToken(&tabela[11][11], "PAR DIR", "");
	gerarToken(&tabela[11][11], "comando_opbin", "");
	
	gerarToken(&tabela[11][28], "not", "");
	gerarToken(&tabela[11][28], "comando_exp_seq", "");
	
	
	//comando_opbin ::= `+´ | `-´ | `*´ | `/´ | `<´ | `<=´ | `>´ | `>=´ | `==´ | `~=´ | and | or 
	
	gerarToken(&tabela[12][0], "comando_opbin", "");
	
	gerarToken(&tabela[12][7], "and", "");
	gerarToken(&tabela[12][7], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][9], "or", "");
	gerarToken(&tabela[12][9], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][15], "ADD", "");
	gerarToken(&tabela[12][15], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][16], "SUB/UNM", "");
	gerarToken(&tabela[12][16], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][17], "MUL", "");
	gerarToken(&tabela[12][17], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][18], "DIV", "");
	gerarToken(&tabela[12][18], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][19], "EQ", "");
	gerarToken(&tabela[12][19], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][22], "NE", "");
	gerarToken(&tabela[12][22], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][23], "LE", "");
	gerarToken(&tabela[12][23], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][24], "LT", "");
	gerarToken(&tabela[12][24], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][25], "GE", "");
	gerarToken(&tabela[12][25], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][26], "GT", "");
	gerarToken(&tabela[12][26], "comando_exp_seq", "");
	
	gerarToken(&tabela[12][29], "epsilon", "");
	
	//listaexp_seq ::= ',' listaexp
	
	gerarToken(&tabela[13][0], "listaexp_seq", "");
	
	gerarToken(&tabela[13][14], "VIRGULA", "");
	gerarToken(&tabela[13][14], "comando_exp_seq", "");
	gerarToken(&tabela[13][14], "listaexp_seq", "");
	
	gerarToken(&tabela[13][29], "epsilon", "");
	
	//comando_args ::=  `(´ [listaexp] `)´ |
	
	gerarToken(&tabela[14][0], "comando_args", "");
	
	gerarToken(&tabela[14][11], "PAR ESQ", "");
	gerarToken(&tabela[14][11], "lista_opt", "");
	gerarToken(&tabela[14][11], "PAR DIR", "");	
	
	//lista_opt
	
	gerarToken(&tabela[15][0], "lista_opt", "");
	
	gerarToken(&tabela[15][10], "comando_exp_seq", "");
	gerarToken(&tabela[15][10], "listaexp_seq", "");
	
	gerarToken(&tabela[15][11], "comando_exp_seq", "");
	gerarToken(&tabela[15][11], "listaexp_seq", "");
	
	gerarToken(&tabela[15][16], "comando_exp_seq", "");
	gerarToken(&tabela[15][16], "listaexp_seq", "");
	
	gerarToken(&tabela[15][27], "comando_exp_seq", "");
	gerarToken(&tabela[15][27], "listaexp_seq", "");

	
	gerarToken(&tabela[15][29], "epsilon", "");
	
	//op_pto_virgula
	gerarToken(&tabela[16][0], "op_pto_virgula", "");
	
	gerarToken(&tabela[16][13], "PTO VIRG", "");
	
	gerarToken(&tabela[16][29], "epsilon", "");
	
	//comando_decl ::= var Nome [`=´ exp]
	
	gerarToken(&tabela[17][0], "comando_decl", "");
	
	gerarToken(&tabela[17][21], "var", "");
	gerarToken(&tabela[17][21], "NOME", "");
	gerarToken(&tabela[17][21], "op_decl", "");
	
	//op_decl ::= [`=´ exp]
	
	gerarToken(&tabela[18][0], "op_decl", "");
	
	gerarToken(&tabela[18][20], "ATRIB", "");
	gerarToken(&tabela[18][20], "comando_exp_seq", "");
	
	gerarToken(&tabela[18][29], "epsilon", "");
	
	/* 	 for (i = 0; i <= QNT; i++){
		 for (j = 0; j <= QT; j++){
			 if (tabela[i][j]->prox != NULL){
				 printf("Tabela[%d][%d]: ",i,j);
				 printf("%s\n", tabela[i][j]->prox->tipo);
			 }
		 }	
	 } */
	
}

void analisadorSintatico (struct token* lista) {
	
	struct token *tokencopia = lista->prox;
	struct token *producao;
	int continua = 1, termID, nTermID;
	
	iniciarTabela();
	
	struct pilha *gramatica = NULL;
	struct pilha *auxiliar = NULL;
	char* tokenlista = strdup (tokencopia->tipo);
	if (!strcmp(tokencopia->tipo, "KWORD"))
		tokenlista = tokencopia->conteudo;
	char* tokenpilha = strdup("trecho");

	inserirPilha(&gramatica, tokenpilha);
	
	while (continua) {
		tokenpilha = strdup(retirarpilha(&gramatica));
		if (tokenpilha == '\0') {
			printf ("Erro: Comando %s invalido. Linha: %d, Coluna: %d\n",tokencopia->conteudo, tokencopia->linha, tokencopia->coluna);
			exit(0);
		}
		termID = terminal(tokenpilha);
		
		//.verifica se o token da produção é um terminal e se é igual ao da lista.
		
		if (termID > 0 && termID < 29) {
			tokenlista = strdup (tokencopia->tipo);
		
			if (!strcmp(tokencopia->tipo, "KWORD"))
				tokenlista = tokencopia->conteudo;
			
 			/* printf("Na pilha: %s\n",tokenpilha);
			printf("Na lista: %s\n", tokenlista); */ 
			
			if (!strcmp(tokenlista, tokenpilha)) {
				tokencopia = tokencopia->prox;
			}
			else {
				//erroParse(tokencopia,tokenpilha ); criar funcao
				printf("Erro: Elemento esperado não encontrado (Erro sintax: 01)\n");
				printf("Elemento esperado: %s\n", tokenpilha);
				printf("Elemento encontrado: %s, linha: %d, coluna: %d\n",tokencopia->conteudo, tokencopia->linha, tokencopia->coluna);
				exit(0);
			}
			
			// Lista acabou.
			
			if (tokencopia == NULL){
				continua = 0;
				//printf("Hey Jude\n");
				while (gramatica != NULL) {
					tokenpilha = strdup(retirarpilha(&gramatica));
					termID = terminal(tokenpilha);
					if (termID == -1){
						nTermID = naoTerminal(tokenpilha);
						if (tabela[nTermID][29]->prox == NULL){
							printf("Erro: Estrutura gramatical incompleta (Erro: 03): O programa acabou sem respeitar a sintatica da linguagem\n");
							exit(0);					
						}
					}
					else {
						printf("Erro: Programa incompleto (Erro Sintax: 02): O programa acabou sem respeitar a sintatica da linguagem\n");
						printf("Próximo elemento esperado: %s\n", tokenpilha);
						exit(0);
					}
				}
			}
			else {
				tokenlista = strdup (tokencopia->tipo);
				if (!strcmp(tokencopia->tipo, "KWORD"))
					tokenlista = tokencopia->conteudo;
			}
		}
		
		else {	
			termID = terminal(tokenlista);
			
			nTermID = naoTerminal(tokenpilha);
			
			
			/* printf("Na pilha: %s\n",tokenpilha);
			printf("Na lista: %s\n", tokenlista);
			printf("Term: %d nTermID: %d\n", termID, nTermID); */
			
			producao = tabela[nTermID][termID]->prox;			
			
			if (producao == NULL){
				if (tabela[nTermID][29]->prox == NULL){
					printf("Erro: gramática errada (Erro: 04): Combinação de tokens não respeita a gramatica da linguagem\n");
					printf("Erro encontrado no elemento: %s, linha: %d, coluna: %d\n",tokencopia->conteudo, tokencopia->linha, tokencopia->coluna);
					exit(0);					
				}
			}
			
			//printf("(");
			while (producao != NULL){
				//printf("%s,", producao->tipo);
				inserirPilha(&auxiliar, producao->tipo);
				producao = producao->prox;
			}
			//printf(")\n");
			while (auxiliar != NULL){
				tokenpilha = strdup(retirarpilha(&auxiliar));
				inserirPilha(&gramatica, tokenpilha);
			}
		}
		
	}
	
	free(producao);
	
	freeTabela();
	
	printf("Gramatica correta\n");	
	
}

void inserirPilha(struct pilha **g, char* tipo) {
	if (*g == NULL){
		(*g) = calloc(1, sizeof(struct pilha));
		(*g)->tipo = strdup(tipo);
		return;
	}
	
	struct pilha* proxElem = calloc (1, sizeof(struct pilha));
	
	proxElem->tipo = strdup((*g)->tipo);
	proxElem->prox = (*g)->prox;
	
	(*g)->tipo = strdup(tipo);
	(*g)->prox = proxElem;
	
	return;
}

char *retirarpilha (struct pilha **g) {
	
	if (*g == NULL) {
		return '\0';
	}
	
	char *token = strdup((*g)->tipo);
	*g = (*g)->prox;
	
	return token;
	
}

int terminal(char *token) {
	int i;	
	for (i = 1; i < QT; i++){
		if (!strcmp(tabela[0][i]->prox->tipo, token)){
		  return i;
		}
	}
	return -1;
}

int naoTerminal(char *token) {
	int i;
	for (i = 1; i <= QNT; i++){
		if (!strcmp(tabela[i][0]->prox->tipo, token)){
		  return i;
		}
	}
	return -1;
}

void freeListaToken (struct token **lista){
	
	if (*lista == NULL)
		return;
	
	struct token *ultimoElem = (*lista)->prox;
	
	if (ultimoElem == NULL) {
		free((*lista)->tipo);
		free((*lista)->conteudo);
		free(*lista);
		return;		
	}
	
	else {
		freeListaToken(&ultimoElem);
		free((*lista)->tipo);
		free((*lista)->conteudo);
		free(*lista);
		return;
	}
	
}

void freeTabela(){
  int i, j;
  for (i = 0; i <= QNT; i++){
    for (j = 0; j <= QT; j++){
      freeListaToken(&tabela[i][j]);
    }
  }
}

void gerarArvore (struct token** tokencopia, struct nodo** raiz) {
	
	struct nodo* p;
	int i, j;
	struct token* search;
	
	while((*tokencopia) != NULL) {
		
		search = variaveis;
		
		if ((!strcmp((*tokencopia)->conteudo, "end")) || (!strcmp((*tokencopia)->conteudo, "else")))
			return;
		
		for (i=0; i<Nos; i++) {
			if ((*raiz)->operando[i] == NULL) {
				(*raiz)->operando[i] = calloc (1, sizeof(struct nodo));
				p = (*raiz)->operando[i];
				break;
			}
		}
		
		for (i = 0; i<6; i++)
			if (!strcmp((*tokencopia)->conteudo, kword[i])) 
				break;
								
		switch (i) {
			
			case 0:
				p->elemento = strdup("if");
				(*tokencopia) = (*tokencopia)->prox;
				p->operando[0] = calloc (1, sizeof(struct nodo));
				inserirExp (tokencopia, &p->operando[0], 2);
				p->operando[1] = calloc (1, sizeof(struct nodo));
				p->operando[1]->elemento = strdup("then");
				(*tokencopia) = (*tokencopia)->prox;
				gerarArvore (tokencopia, &p->operando[1]);
				if (!strcmp((*tokencopia)->conteudo, "else")) {
					p->operando[2] = calloc (1, sizeof(struct nodo));
					p->operando[2]->elemento = strdup("else");
					(*tokencopia) = (*tokencopia)->prox;
					gerarArvore (tokencopia, &p->operando[2]);
				}
				(*tokencopia) = (*tokencopia)->prox;
				break;
			case 1:
				p->elemento = strdup("while");
				(*tokencopia) = (*tokencopia)->prox;
				p->operando[0] = calloc (1, sizeof(struct nodo));
				inserirExp (tokencopia, &p->operando[0], 2);
				p->operando[1] = calloc (1, sizeof(struct nodo));
				p->operando[1]->elemento = strdup("do");
				(*tokencopia) = (*tokencopia)->prox;
				gerarArvore (tokencopia, &p->operando[1]);
				(*tokencopia) = (*tokencopia)->prox;
				break;
			case 2: 				
				while (search != NULL) {
					if (search->conteudo != NULL) {
						if (!strcmp((*tokencopia)->prox->conteudo, search->conteudo)) {
							printf("Error (Gerar arvore): Variavel %s duplamente declarada, linha: %d, coluna: %d\n", (*tokencopia)->prox->conteudo, (*tokencopia)->prox->linha, (*tokencopia)->prox->coluna);
							exit(0);
						}
					}
					if (search->prox == NULL) {
						search->prox = calloc (1, sizeof(struct token));
						search->prox->conteudo = strdup ((*tokencopia)->prox->conteudo);
						break;
					}
					
					search = search->prox;
				}
				
				p->elemento = strdup("var");
				(*tokencopia) = (*tokencopia)->prox;
				p->operando[0] = calloc (1, sizeof(struct nodo));
				p->operando[0]->elemento = strdup ((*tokencopia)->conteudo);
				(*tokencopia) = (*tokencopia)->prox;
				if ((*tokencopia) == NULL)
					break;
				if (!strcmp((*tokencopia)->tipo, "ATRIB")) {
					(*tokencopia) = (*tokencopia)->prox;
					p->operando[1] = calloc (1, sizeof(struct nodo));
					inserirExp (tokencopia, &p->operando[1], 1);
				}
				break;
			case 5:
				p->elemento = strdup("for");
				(*tokencopia) = (*tokencopia)->prox;
				gerarArvore (tokencopia, &p);
				(*tokencopia) = (*tokencopia)->prox;
				p->operando[1] = calloc (1, sizeof(struct nodo));
				inserirExp (tokencopia, &p->operando[1],2);
				if (!strcmp((*tokencopia)->tipo, "VIRGULA")) {
					(*tokencopia) = (*tokencopia)->prox;
					p->operando[2] = calloc (1, sizeof(struct nodo));
					inserirExp (tokencopia, &p->operando[2],2);
				}
				p->operando[3] = calloc (1, sizeof(struct nodo));
				p->operando[3]->elemento = strdup("do");
				(*tokencopia) = (*tokencopia)->prox;
				gerarArvore (tokencopia, &p->operando[3]);
				(*tokencopia) = (*tokencopia)->prox;		
				break;			
			default:
				if (!strcmp((*tokencopia)->conteudo, "print") && !strcmp((*tokencopia)->prox->tipo, "PAR ESQ")) {
					p->elemento = strdup("print");
					(*tokencopia) = (*tokencopia)->prox;
					j = 0;
					while (strcmp((*tokencopia)->tipo, "PAR DIR")){
						p->operando[j] = calloc (1, sizeof(struct nodo));
						inserirExp (tokencopia, &p->operando[j], 0);
						j++;
					}
					(*tokencopia) = (*tokencopia)->prox;
				}
				else {
					
					while (search != NULL) {
						if (search->conteudo != NULL) {
							if (!strcmp((*tokencopia)->conteudo, search->conteudo)) {
								break;
							}
						}
						
						if (search->prox == NULL) {
							printf("Error (Gerar arvore): Variavel %s nao declarada, linha: %d, coluna: %d\n", (*tokencopia)->conteudo, (*tokencopia)->linha, (*tokencopia)->coluna);
							exit(0);
						}
						
						search = search->prox;
					}
					
					p->elemento = strdup("=");
					p->operando[0] = calloc (1, sizeof(struct nodo));
					p->operando[0]->elemento = strdup ((*tokencopia)->conteudo);
					(*tokencopia) = (*tokencopia)->prox;
					(*tokencopia) = (*tokencopia)->prox;
					p->operando[1] = calloc (1, sizeof(struct nodo));
					inserirExp (tokencopia, &p->operando[1], 1);
				}
				break;
		}
			
		if ((*tokencopia) != NULL) {
			if (!strcmp((*tokencopia)->tipo, "PTO VIRG")) {
				(*tokencopia) = (*tokencopia)->prox;
			}		
		}
		
	}
	
	return;
}

void inserirExp (struct token** tokencopia, struct nodo** raiz, int tipo) {

	struct token *inicio = (*tokencopia), *fim;
	int continua = 1, parenteses;
	switch (tipo) {
		case 0:
			inicio = (*tokencopia)->prox;
			parenteses = 1;
			while ( continua )  {
				if (!strcmp((*tokencopia)->prox->tipo, "PAR ESQ"))
					parenteses++;
				else if (!strcmp((*tokencopia)->prox->tipo, "PAR DIR"))
					parenteses--;
				else if (!strcmp((*tokencopia)->prox->tipo, "VIRGULA"))
					parenteses = 0;
				
				if (parenteses == 0)
					continua = 0;
				else
					(*tokencopia) = (*tokencopia)->prox;
			}
			fim = (*tokencopia);
			(*tokencopia) = (*tokencopia)->prox;
			inserirArvore(inicio, fim, raiz);
			return;			
			
		case 1:
		
			parenteses = 0;
		
			while ( continua ) {
				if (!strcmp((*tokencopia)->tipo,"PAR ESQ"))
					parenteses++;
				else if (!strcmp((*tokencopia)->tipo,"PAR DIR"))
					parenteses--;
							
				if (parenteses == 0) {
				
					if ((*tokencopia)->prox != NULL){
						if (!strcmp((*tokencopia)->tipo, "NOME") || !strcmp((*tokencopia)->tipo, "NUM INTEIRO") || !strcmp((*tokencopia)->tipo,"PAR DIR"))
							if (!strcmp((*tokencopia)->prox->tipo, "NOME") || !strcmp((*tokencopia)->prox->tipo, "PTO VIRG"))
								continua = 0;
							else if (!strcmp((*tokencopia)->prox->tipo, "KWORD"))
								if (strcmp((*tokencopia)->prox->conteudo, "and") && strcmp((*tokencopia)->prox->conteudo, "or"))
									continua = 0;
								else
									(*tokencopia) = (*tokencopia)->prox;
							else {
								(*tokencopia) = (*tokencopia)->prox;
							}
						else {
							(*tokencopia) = (*tokencopia)->prox;
						}
					}
					else
						continua = 0;
				}
				else
					(*tokencopia) = (*tokencopia)->prox;
			}
		
			fim = (*tokencopia);
			(*tokencopia) = (*tokencopia)->prox;
			
			inserirArvore(inicio, fim, raiz);
			return;	
			
		case 2:
			while ( continua )  {
				if (!strcmp((*tokencopia)->prox->tipo, "VIRGULA"))
					continua = 0;
				else if (!strcmp((*tokencopia)->prox->tipo, "KWORD"))
					if (strcmp((*tokencopia)->prox->conteudo, "or") && strcmp((*tokencopia)->prox->conteudo, "and"))
						continua = 0;
				
				if (continua)
					(*tokencopia) = (*tokencopia)->prox;
			}
			
			fim = (*tokencopia);
			(*tokencopia) = (*tokencopia)->prox;
			
			inserirArvore(inicio, fim, raiz);
			return;	
	}

}

void inserirArvore (struct token* inicio, struct token* fim, struct nodo** raiz) {
	
	if (inicio == fim) {
		if (inicio->conteudo[0] < 48 || inicio->conteudo[0] > 57) {
			struct token *search = variaveis;
			while (search != NULL) {
				if (search->conteudo != NULL){
					if (!strcmp(inicio->conteudo, search->conteudo)) {
						break;
					}
				}

				if (search->prox == NULL) {
					printf("Error (Inserir arvore): Variavel %s nao declarada, linha: %d, coluna: %d\n", inicio->conteudo, inicio->linha, inicio->coluna);
					exit(0);
				}
				
				search = search->prox;
			}
		}
		
		(*raiz)->elemento = strdup (inicio->conteudo);
		return;
	}
	
/* 	if (!strcmp(inicio->conteudo, "print")) {
		(*raiz)->elemento = strdup("print");
		inicio = inicio->prox;
		int j = 0;
		while (strcmp(inicio->tipo, "PAR DIR")){
			(*raiz)->operando[j] = calloc (1, sizeof(struct nodo));
			inserirExp (inicio, &((*raiz)->operando[j]), 0);
			j++;
		}
		return;
	} */
	
	int i, parenteses = 0, trigger = 0;
	struct token* atual = inicio, *ultimaAparicao;
	
	if (!strcmp(atual->tipo, "PAR ESQ")) {
		parenteses = 1;
		int continua = 1;
		while (continua) {
			if (!strcmp(atual->prox->tipo, "PAR ESQ"))
				parenteses++;
			else if (!strcmp(atual->prox->tipo, "PAR DIR"))
				parenteses--;
			if (parenteses == 0){
				continua = 0;
			}
			else
				atual = atual->prox;
		}
		if (atual->prox == fim){
			inserirArvore (inicio->prox, atual, raiz);
			return;
		}
		
		atual = inicio; 
	}
	
	for (i = 0; i<6; i++) {
		if (!strcmp(atual->tipo, "PAR ESQ")) {
			parenteses++;
		}
		while (atual != fim) {
			switch (i) {
				case 0:
					if (!strcmp(atual->prox->conteudo, "or") && parenteses == 0){
						trigger = 1;
						ultimaAparicao = atual;
					}
					break;
					
				case 1:
					if (!strcmp(atual->prox->conteudo, "and") && parenteses == 0){
						trigger = 1;
						ultimaAparicao = atual;
					}
					break;
					
				case 2:
					if ((!strcmp(atual->prox->tipo, "EQ") || !strcmp(atual->prox->tipo, "NE")) && parenteses == 0){
						trigger = 1;
						ultimaAparicao = atual;
					}
					break;
					
				case 3:
					if ((!strcmp(atual->prox->tipo, "GT") || !strcmp(atual->prox->tipo, "LT") || !strcmp(atual->prox->tipo, "LE") || !strcmp(atual->prox->tipo, "GE")) && parenteses == 0){
						trigger = 1;
						ultimaAparicao = atual;
					}
					break;
					
				case 4:
					if ((!strcmp(atual->prox->tipo, "ADD")) && parenteses == 0){
						trigger = 1;
						ultimaAparicao = atual;
					}
					else if ((!strcmp(atual->prox->tipo, "SUB/UNM")) && parenteses == 0){
						if (!strcmp(atual->tipo, "NOME") || !strcmp(atual->tipo, "NUM INTEIRO") || !strcmp(atual->tipo, "PAR DIR")){
							trigger = 1;
							ultimaAparicao = atual;
						}
					}
					break;
					
				case 5:
					if ((!strcmp(atual->prox->tipo, "MUL") || !strcmp(atual->prox->tipo, "DIV")) && parenteses == 0){
						trigger = 1;
						ultimaAparicao = atual;
					}
					break;
			}
			
			
			if (!strcmp(atual->prox->tipo, "PAR ESQ"))
				parenteses++;
			else if (!strcmp(atual->prox->tipo, "PAR DIR"))
				parenteses--;
					
			atual = atual->prox;
			
		}
		
		if (trigger) {
				(*raiz)->elemento = strdup (ultimaAparicao->prox->conteudo);
				(*raiz)->operando[0] = calloc (1, sizeof(struct nodo));
				(*raiz)->operando[1] = calloc (1, sizeof(struct nodo));
				inserirArvore (inicio, ultimaAparicao, &(*raiz)->operando[0]);
				inserirArvore (ultimaAparicao->prox->prox, fim, &(*raiz)->operando[1]);
				return;
			}
			
		atual = inicio;
	}
	
	(*raiz)->elemento = strdup (inicio->conteudo);
	(*raiz)->operando[0] = calloc (1, sizeof(struct nodo));
	inserirArvore (inicio->prox, fim, &(*raiz)->operando[0]);
	return;
	
}

void imprimirArvore(struct nodo* raiz) {
	
	if (raiz == NULL)
		return;
	
	struct nodo* impressao = raiz;
	int i = 0;
	
	fprintf(saida, "(");
	printf("(");
	
	fprintf(saida, "%s", impressao->elemento);
	printf("%s", impressao->elemento);
	
	while (impressao != NULL){
		impressao = raiz->operando[i];
		imprimirArvore(impressao);
		i++;
	}
	
	fprintf(saida, ")");
	printf(")");
	
	return;
	
}

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
	
	if (strcmp(raiz->elemento, "not") && strcmp(raiz->elemento, "and") && strcmp(raiz->elemento, "or")) {
		
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

