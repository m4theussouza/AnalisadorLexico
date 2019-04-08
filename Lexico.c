#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

/*
    Autor: Matheus de Almeida Souza
    Turma: BEC 7º Semestre
    Professor: Leonardo Takuno
    Trabalho de Teoria da Computação e Compiladores
    Analisador Léxico
*/

//tipo enum com os atomos
typedef enum{
    PALAVRA_RESERVADA, //palavra reservada da linguagem
    FRASE, //strings na linguagem ("Qualquer caracter ASCII")
    COMENTARIO, // qualquer caracter da tabela ASCII que esteja entre /*{vetor}*/
    IDENTIFICADOR, //variaveis da linguagem
    NUM_INT, // numeros inteiros
    NUM_REAL, // numeros reais
    OP_LOGICO, // operadores logicos (& -> E) ($ -> OU) (! -> NEGAÇÃO)
    OP_RELACIONAL, // operadores relacionais (Maior -> <) (Maior ou igual -> <=) (Menor -> >) (Menor ou igual -> >=) (IGUAL -> =) (DIFERENTE -> <>)
    ERRO, //caso nao seja nada dos anteriores, retorna erro
    EOS, // FIM DO ARQUIVO
    ATRIBUICAO, // <-
    RESTO, // %
    ABRE_PAR, // (
    FECHA_PAR, // )
    MULTIPLICACAO, // *
    ADICAO, // +
    VIRGULA, // ,
    SUBTRACAO, // -
    PONTO, // .
    DIVISAO, // /
    PONTO_VIRGULA,// ;
}Atomo;

//vetor com os caracteres sem atribuicao
char semAtribuicao[] = {'<', '-', '(', ')', '*', '+', ',', '-', '.', '/', ';', '%'};

//Vetor global com as mensagens para cada atomo
char *msgAtomo[] = { 
    "PALAVRA_RESERVADA", 
    "FRASE",
    "COMENTARIO",
    "IDENTIFICADOR",
    "NUMERO_INTEIRO",
    "NUMERO_REAL",
    "OPERADOR_LOGICO",
    "OPERADOR_RELACIONAL",
    "ERRO",
    "FIM_DO_ARQUIVO", 
    "ATRIBUICAO", 
    "RESTO",  
    "ABRE_PAR", 
    "FECHA_PAR", 
    "MULTIPLICACAO", 
    "ADICAO", 
    "VIRGULA", 
    "SUBTRACAO", 
    "PONTO", 
    "DIVISAO", 
    "PONTO_VIRGULA"
};

//Vetor global com as palavras reservadas pela linguagem
char *palavrasReservadas[] = {
    "ALGORITMO", // Inicio do algoritmo
    "ATE", // Inicializa a variável de controle do PARA
    "CADEIA", // Tipo de dado para sequência de caracteres
    "CARACTER", // Tipo de dado para definir um caracter
    "ENQUANTO", // Determina um laço com condição no início
    "ENTAO", // usado no bloco de estrutura de seleção
    "FACA", // usado no bloco de estrutura de repetição
    "FIM", // Fim de um bloco ou do programa
    "FUNCAO", // Determina uma função no algoritmo
    "INICIO", // Início do algoritmo
    "INTEIRO", // Tipo de dado para números inteiros
    "PARA", // Determina um laço com variável de controle
    "PASSO", // Incremento da variável de controle na repetição
    "PROCEDIMENTO", // Determina um procedimento no algoritmo
    "REAL", // Tipo de Dado para números reais ( ponto flutuante )
    "REF", // Operador para de referência para variáveis
    "RETORNE", // Retorna de uma função
    "SE", // Determina uma estrutura de seleção
    "SENAO", // Caso contrário da instrução de seleção
    "VARIAVEIS" // Início do bloco de declaração de variáveis
};

//Declaracao das funcções utilizadas pelo programa
Atomo reconhece_COMENTARIO(void); //reconhece comentario /**/
Atomo reconhece_FRASE(void); //reconhece cadeia de caracteres entre "cadeia"
Atomo reconhece_OP_RELACIONAL(void); // reconhece os operadores relacionais
Atomo reconhece_SEM_ATRIBUICAO(void); //reconhece lexemes sem atribuitos
Atomo reconhece_ID_E_PALAVRA(void); //reconhece identificadores e palavras reservadas e armazena os lexemes
Atomo reconhece_NUM(void); //reconhece numeros reais ou inteiros e armazena os lexemes 
Atomo proximo_token(void); //maquina de estado que reconhece os atomos
char proximo_char(void); //adianta o ponteiro do buffer e retorna o char atual
void retract_char(void); //atrasa o ponteiro do buffer
char char_atual(void); //retorna o caracter atual sem adiantar o ponteiro do buffer

//variaveis globais
char *buffer; //guarda os valores lidos do arquivo ptl
char *lexeme; // guarda o lexeme
int nlinha = 1; // guarda o numero de linhas
int numPalavrasRes = sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]); //guarda numero de palavras reservadas

char proximo_char(void) {
    return *buffer++;
}

void retract_char() {
    buffer--;
}

char char_atual(){
    char temp;
    temp = proximo_char();
    retract_char();
    return temp;
}

Atomo reconhece_OP_RELACIONAL() {
    char c;
    char *op_relacional = (char*) calloc( 10 , sizeof(char) );
    
    //guarda o endereço para o ponteiro de chars op_relacional
    lexeme = op_relacional;
    
    //retorna um no buffer para recuperar o token perdido
    retract_char();
    c = proximo_char();
    
    /*
        como foi verificado na funcao proximo_token ja sabemos que
        c é um simbolo de operador relacional entao o guardamos
    */
    *op_relacional++ = c;
    
    /*
        operadores possiveis <, <=, =, <>, >, >=
        começamos verificando se c é igual <, pois, esse tem
        incio a 3 possiveis constantes simbolicas
        se depois de < for = ou > ele sera MEI ou DI respectivamente
        se apos <, for quebra de linha, espaço, fim do arquivo, numero ou digito temos o ME
        se apos <, for - retornamos erro e tratamos dps como atribuicao '<-'
        se apos <, for outro < tbm guardamos pois temos somente o analisador lexico e nao sintatico
        se nao é erro
        para > e >= e = é o mesmo processo
    */

    if( c == '<') {
        c = proximo_char();
        if ( c == '=' || c == '>') {
            *op_relacional++ = c;
            return OP_RELACIONAL;
        } else if (c == '<') {
            buffer++;
            *op_relacional++ = c;
            return OP_RELACIONAL;
        } else if ( isdigit(c) || isalpha(c) || c == ' ' || c == '\n' || c == '\0') {
            retract_char();
            return OP_RELACIONAL;
        } else if (c == '-') {
            retract_char();
            return ERRO;
        } else {
            lexeme = "ERRO";
            return ERRO;
        }
    } else if ( c == '>') {
        c = proximo_char();
        if ( c == '>' || c == '<' || c == '=' ) {
            buffer++;
            *op_relacional++ = c;
            return OP_RELACIONAL;
        } else if ( isdigit(c) || isalpha(c) || c == ' ' || c == '\n' || c == '\0' ) {
            retract_char();
            return OP_RELACIONAL;
        } else {
            lexeme = "ERRO";
            return ERRO;
        }
    } else if ( c == '=') {
        return OP_RELACIONAL;
    }
    
    lexeme = "ERRO";
    return ERRO;
}

Atomo reconhece_SEM_ATRIBUICAO() {
    char c;
    char *palavra;
    Atomo atomo = ERRO;
    palavra = (char*) calloc( 4 , sizeof(char) );  

    lexeme = palavra; //guarda endereço incial do ponteiro para char palavra
    
    retract_char(); //retorna um para nao perder o token consumido
    c = char_atual(); //guarda o char atual e anda com o buffer
    
    /*
        é preciso cuidar do caso especiaç em que ATRIBUICAO é uma
        uniao de '<' com '-'
    */
    if ( c == '<' ) { 
        *palavra++ = c;
        buffer++;
        if ( char_atual() == '-' ) {
            *palavra++ = char_atual();
            buffer++;
            return ATRIBUICAO;
        }  
        else {
            lexeme = "ERRO";
            return ERRO;
        }
    }
    
    //verifica se o token é %
    if ( c == '%') {
        *palavra++ = c;
        buffer++;
        return RESTO;
    } 

    //dec 59 na tabela ascii é ;
    if ( (int)c == 59 ) {
        *palavra++ = c;
        buffer++;
        return PONTO_VIRGULA;
    }

    /*
        todos os outros atomos sem atribuicao com exceção dos anteriores
        são uma sequencia da tabela ascii que vai do 40 ao 47
        entao procuramos binariamente se nao for 
        nenhum dos outros nessa sequencia
        como nosso vetor msgAtomos tem um tamanho constante 
        subtraio 28 do token encontrado para que ele retorne 
        a posicao correta do token no vetor 
    */
    for(int i = 40; i <= 47; i++) {
        if ( (int)c == i ) {
            *palavra++ = c;
            buffer++;
            return i-28;
        }
    }

    //se c for igual \0 chegamos no carater final
    if ( c == '\0' ) {
        return EOS;
    }

    //se nao for nenhum desse retorna erro
    lexeme = "ERRO";
    return ERRO;
}

Atomo reconhece_COMENTARIO() {
    char c, char_posterior;
    char *palavra;
    
    palavra = (char*) calloc( 500, sizeof(char) );    
    
    //guarda lexeme
    lexeme = palavra;
    /*
        como na funcao proximo_token() consumimos 
        2 caracteres temos que retorna
        o ponteiro do buffer duas vezes

    */
    buffer = buffer - 2;
    /*
        para o comentario precisamos sempre estar olhando o caracter
        da frente para saber se temos a sequencia /*
    */
    c = proximo_char();
    char_posterior = proximo_char();
    if ( c == '/' && char_posterior == '*') {
        *palavra++ = c; //guarda c
        *palavra++ = char_posterior; //guarda char_posterior
        c = proximo_char(); //pega o char atual e anda o buffer
        char_posterior = char_atual(); // pega o char que o buffer aponta nesse momento sem andar o buffer
        while( c != '*' && char_posterior != '/'){
            if ( c == '\n' ) { //caso tenha uebra de linha no comentario represento com '\n'
                *palavra++ = '\'';
                *palavra++ = '\\';
                *palavra++ = 'n';
                *palavra++ = '\'';
                //conta quebra de linha do meio dos comentarios, para continuar a ter o controle do numero de linha
                 nlinha++; 
            } else {
                *palavra++ = c; //vai armazenando enquanto c for difernete de / e char_posterior for diferente de * 
            }
            c = proximo_char(); //proximo char
            char_posterior = char_atual(); //proximo sem avançar o buffer
        }
        *palavra++ = c; //guarda o ultimo *
        *palavra++ = char_posterior; // e guarda o /
        buffer++; //avança para o proximo token
        return COMENTARIO;
    }

}

Atomo reconhece_FRASE() {
    char c, d;
    char *palavra;
    
    palavra = (char*) calloc( 500, sizeof(char) );    
    
    //guarda lexeme
    lexeme = palavra;

    //volta uma posicao para nao perder o char analisado na funcao anterior
    retract_char();
    c = proximo_char();
    /*
        Se c for uma "(aspa) começamos a guardar enquanto c (caracter atual) 
        for diferente de outra "(aspa) ou se o proximo caracter for \.
        Se "(aspa) vir antes de \(barra invertida) precisamos 
        armazenar a "(aspa) e tambem precisamos armazenar \(barra invertida).
        Se tiver duas aspas seguidas ou quebra de linha é erro.
    */
    if ( c == '"') {
        *palavra++ = c;
        c = proximo_char();
        d = char_atual();
        while( c != '"' || d == '\\'){
            if ( c == '"' && d == '\\' ) {
                *palavra++ = c;
                buffer++;
            } else if ( c == '\\' ) {
                *palavra++ = c;
                buffer++;
            } else {
               *palavra++ = c; 
            }
    
            c = proximo_char();
            d = char_atual();
            if ( c == '\n' || c == '"' && d == '"') {
                lexeme = "ERRO";
                return ERRO;
            }
            
            
        }
        *palavra++ = '*';
        *palavra++ = c;
        return FRASE;
    }

}

Atomo reconhece_ID_E_PALAVRA() {
    char c;
    char *palavra;
    
    palavra = (char*) calloc( 20 , sizeof(char) );    
    
    //guarda lexeme
    lexeme = palavra;
    retract_char();
    c = proximo_char();
    do {
        // ********************************************************
        //algoritmo que cuida do caso especial, quando temos o OP_EXP
        if( c == 'e' || c == 'E' ) {
            *palavra++ = c;
            c = proximo_char();
            if ( c == '+' || c == '-' || isdigit(c) ) {
                *palavra++ = c;
                c = proximo_char();
                while ( isdigit(c) ) {
                    *palavra++ = c;
                    c = proximo_char();
                }
                retract_char();
                return NUM_REAL;
            }
        }
        // ********************************************************   
        *palavra++ = c;
        c = proximo_char();
    } while( isalpha(c) || isdigit(c) );
    
    *palavra = '\0';
    //rotina busca no vetor de palavras reservadas
    for( int i = 0; i < numPalavrasRes; i++ ) {
        if  ( ( _stricmp(lexeme, palavrasReservadas[i]) ) == 0) {
            retract_char();
            return PALAVRA_RESERVADA;
        }
    }
    //caso nao seja palavra reservada é um identicador
    retract_char();
    return IDENTIFICADOR;    
}

Atomo reconhece_NUM() {
    char c, *numero = (char*) calloc(100 , sizeof(c) );

    //guarda lexeme
    lexeme = numero;

    retract_char();
    c = proximo_char();
    *numero++ = c;
    /*
        Maquina de estado para achar numeros inteiros e reias
        NUMERO_INTEIRO -> DIGITOS+
        OP_FRACAO -> .DIGITOS*
        OP_EXP -> ( (E|e) ( +| – | λ) DIGITO+) 
        (   na funcao reconhece_ID_E_PALAVRA() cuido do
            caso especial quando o nuemro começa com 'e'
            seguido de numero exemplo: e2 )
        NUMERO_REAL -> NUMERO_INTEIRO OP_FRACAO (OP_EXP | λ )  
    */
    int estado = 2;
    while(1) {
        switch (estado)
        {
            case 2:
                c = proximo_char();
                if  (c == '.') {
                    *numero++ = c;
                    estado = 3;
                } else if ( isdigit(c) ) {
                    *numero++ = c;
                    estado = 2;
                } else if ( !isalpha(c) ) {
                    retract_char();
                    return NUM_INT;
                } else if ( c == 'e' || c == 'E' ) {
                    *numero++ = c;
                    c = proximo_char();
                    if ( isdigit(c) ) {
                        *numero++ = c;
                        estado = 4;
                    } else if ( c == '+' || c == '-' ) {                  
                        *numero++ = c;
                        estado = 3;
                    }
                } else {
                    lexeme = NULL;
                    return ERRO;
                } 
            break;
            case 3:
                c = proximo_char();
                if ( isdigit(c) ) {
                    *numero++ = c;
                    estado = 4;
                } else if ( !isalpha(c) ) {
                    retract_char();
                    return NUM_REAL;
                } else if ( c == 'e' || c == 'E' ) {
                    *numero++ = c;
                    c = proximo_char();
                    if ( isdigit(c) ) {
                        *numero++ = c;
                        estado = 4;
                    } else if ( c == '+' || c == '-' ) {
                        *numero++ = c;
                        estado = 3;
                    }
                } else {
                    lexeme = NULL;
                    return ERRO;
                }
            break;
            case 4:
                c = proximo_char();
                if ( isdigit(c) ) {
                    *numero++ = c;
                    estado = 4;
                } else if ( !isdigit(c) ) {
                    retract_char();
                    return NUM_REAL;
                } else {
                    lexeme = NULL;
                    return ERRO;
                }
            break;                
        }
    }
    
}

Atomo proximo_token() {
    char c;
    Atomo atomo = ERRO;
    int IdentificaSemAtribuicao = 0;

    //ignora espaço em branco e trata nova linha
    while(*buffer == ' ' || *buffer == '\n') { 
        if(*buffer == '\n') {
            nlinha++;
        }
        buffer++;
    }

    //se \0 chegamos no final do ptl lido
    if(*buffer == '\0') {
        lexeme = "\\0";
        return EOS;
    }

    //pega o caracter a ser analisado e avança o buffer
    c = proximo_char();

    //se c é igual a '\n' temos uma quebra de linha logo uma nova linha
    if( c == '\n' ) {
        nlinha++;
    }

    /*  
        se c é igual a um caracter alfabetico 
        inicia maquina de estado para buscar palvras reservadas e identificadores
        e o caso especial de um numero que começa com uma letra 
    */
    if( isalpha(c) ) {
        atomo = reconhece_ID_E_PALAVRA();
    }

    //se c é igual a " inicia algoritmo para reconhecer frase
    if ( c == '"' ) {
        atomo = reconhece_FRASE();
    }

    //se c é igual a / e o proximo é igual a * inicia algoritmo para reconhecer comentarios 
    if ( c == '/' ) {
        c = proximo_char();
        if ( c == '*' ) {
            atomo = reconhece_COMENTARIO();
        }
    }

    //se c é um digito inicia maquina de estado para reconhecer o conjunto dos numeros reais
    if( isdigit(c) ) {
        atomo = reconhece_NUM();
    }

    /* 
        pela tabela ascii os caracteres ><= estão entre 60 e 62 (DEC)
        logo se o caracter em decimal for maior ou igual a 60
        e menor ou igual 62 iniciamos o algoritmo para reconhecer operadores relacionais
    */
    if( (int) c >= 60 ) {
        if ( (int) c <= 62 ) {
            atomo = reconhece_OP_RELACIONAL();
        }
    }
    
    //se c é um dos tres caracteres a seguir é um operador logico
    if ( c == '$' || c == '&' || c == '!') {
        lexeme = (char*) calloc( 2, sizeof(c) );
        *lexeme = c;
        return OP_LOGICO;
    }

    /*
        se até esse momento o atomo é igual a ERRO, verifico se é um atomo
        sem atribuição comparando esse caracter com o vetor que contem os simbolos que representam 
        atomos sem atribuicao, se nao for retorna ERRO
        se estiver no vetor, inicia o algoritmo para encontrar os atomos sem atribuicao
    */
    if ( atomo == ERRO ) {
        IdentificaSemAtribuicao = 1;
    }

    /*
        se o atomo for erro e o caracter pertencer aos atomos sem atribuicao
        inicia o algoritmo para buscar atomo sem atribuicao 
    */
    if ( IdentificaSemAtribuicao == 1 ) {
        for(int i = 0; i < strlen(semAtribuicao); i++) {
            if ( c == semAtribuicao[i] ) {
                atomo = reconhece_SEM_ATRIBUICAO();
            }
        }
    }

    return atomo;
}

void main(int argc, char *argv[]) {
    Atomo atomo;
    FILE *f;
    char c, *string, *nomeArquivo;
    int contaAtomo = 0;

    /*
        guarda o nome do arquivo passado como argumento no incio do programa
        e abre o arquivo para leitura
    */
    nomeArquivo = argv[1];
    f = fopen(nomeArquivo, "r");
    
    //conta quando carateres tem no arquivo
    while( fgetc(f) != EOF ) {
        contaAtomo++;
    }

    /*
        aloca um poteiro para char com tamanho 
        exato dos numeros de carater no arquivo
    */
    string = (char *)calloc( (contaAtomo+1) , sizeof(char) );

    //volta o curso para o incio do arquivo já que o fomos ate o final dele
    fseek(f, 0, SEEK_SET);
    
    //salva cada caracter do arquivo um uma posicao desse vetor de caracteres
    for(int i = 0; i < contaAtomo; i++) { 
        string[i] = fgetc(f);
    }

    //faz o buffer apontar para esse vetor
    buffer = string;

    //inicia a analise lexica
    do{
         atomo = proximo_token();
         printf("{Numero da Linha do Atomo %d, Atomo %s, Lexeme %s}\n", nlinha, msgAtomo[atomo], lexeme );
         lexeme = NULL;
         free(lexeme);
    } while( atomo != EOS && atomo != ERRO );
    
    free(string); 
    fclose(f);
}
