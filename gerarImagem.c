#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "definitions.h"
#include <errno.h>


int soma(char *final_number)  {
    int soma = 0;
    for (int i = 0; i < 7; i++) {
        if(i%2 == 0) {
            soma+= (final_number[i] - '0') * 3;
        } else {
            soma+= (final_number[i] - '0');
        }
    }

    return soma;
}

int find_next(int soma) {
    int next_number = soma;

    while(next_number%10 != 0){
        next_number++;
    }

    return next_number;
}

FILE * get_file() {
    FILE *arquivo;
    // Leitura do nome do arquivo
    char nome_arquivo[100];

    printf("\n");
    printf("----Nome do arquivo----\n");
    printf("\n");

    printf("Digite o caminho do arquivo: ");

    fgets(nome_arquivo, 100, stdin);

    // Remove o \n do nome do arquivo
    size_t len = strlen(nome_arquivo);
    if (len > 0 && nome_arquivo[len - 1] == '\n') {
        nome_arquivo[len - 1] = '\0';
    }

    arquivo = fopen(nome_arquivo, "r");

    return arquivo;
}

void find_first_line(int altura, int largura, FILE *arquivo, char *linha, int *contador) {
    bool linha_codigo = false;
    bool largura_contagem = true;

    int contador_linha = 0;

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            // Leitura do pixel
            int pixel;
            fscanf(arquivo, "%d", &pixel);

            // Encontrar primeira linha do codigo de barras com pixel preto
            if (pixel == 1) {
                linha_codigo = true;
            }

            // Encontrar largura do código de barras
            if(linha_codigo == true){
                if(pixel == 1 && largura_contagem == true) {
                    *contador = *contador + 1;
                } else if (pixel == 0 && largura_contagem == true) {
                    largura_contagem = false;

                }
                // Adiciona pixel a linha
                linha[contador_linha] = pixel;
                contador_linha++;
            }
        }
        if(largura_contagem == false) {
            break;
        }
    }
}

void find_number(int i, char *linha, int contador, int *contador_number, char *number) {
    if(i < 28) {
        char current_char = '0'+ linha[i * contador + contador*3];

        number[i%7] = current_char;
        *contador_number = *contador_number + 1;
    } else if ( i>= 28) {
        char current_char = '0'+ linha[i * contador + contador*8];

        number[i%7] = current_char;
        *contador_number = *contador_number + 1;
    }
}

int receber(void) {
    // Declaração de variáveis do pbm
    int largura, altura;
    int contador = 0;
    char tipo[3]; // P1

    FILE *arquivo = get_file();

    // Verifica se o arquivo foi aberto corretamente
    if(arquivo==NULL) {
        printf("Erro ao abrir o arquivo\n");
        printf("Erro foi: %s\n", strerror(errno));

        return 1;
    }

    // Leitura do cabeçalho
    fscanf(arquivo, "%s\n", tipo);
    fscanf(arquivo, "%d %d\n", &largura, &altura);

    char linha[largura];

    find_first_line(altura, largura, arquivo, linha, &contador);

    char number[8];
    int contador_number = 0;

    char final_number[8];
    int contador_final_number = 0;

    for (int i = 0; i < 7; i++) {
        final_number[i] = '1';
    }

    final_number[7] = '\0';

    for (int i = 0; i < 56; i++) {
        // Encontra o 7 valores do código de barras de um digito
        find_number(i, linha, contador, &contador_number, number);

        if(contador_number == 7) {

            int numeros_iguais=0;

            for (int j = 0; j < 10 ; j++) {
                for(int l = 0; l < 8; l++) {
                        // Verifica se o número é igual ao decodificado DIREIRA ou ESQUERDA
                    if(i < 28){
                        if(LCode[j][l] == number[l]){

                            numeros_iguais++;

                            if(numeros_iguais == 7) {
                                char index = '0' + j;
                                final_number[contador_final_number] = index;
                                contador_final_number++;

                                break;
                               }
                        } else {
                            break;
                        }
                    } else if ( i>= 28) {
                        if(RCode[j][l] == number[l]){

                            numeros_iguais++;

                            if(numeros_iguais == 7) {
                                char index = '0' + j;
                                final_number[contador_final_number] = index;
                                contador_final_number++;

                                break;

                            }
                        } else {
                            break;
                        }
                    }
                }
                numeros_iguais = 0;
            }

            contador_number = 0;
        }
    }

    if(contador_final_number != 8) {
        printf("Código nao encontrado ou valores inválidos\n");
        return 1;
    }

    // Verifica se o código é válido
    int soma_result = soma(final_number);
    int next_number = find_next(soma_result);

    char soma_char = '0' + (next_number - soma_result);

    if(soma_char == final_number[7]){
        printf("Código: %s\n", final_number);
    } else {
        printf("Código inválido\n");
        return 1;
    }


    return 0;
}


int codes[67];

void GerarCodigoEAN8(imagemPBM imagem){
  //adicionando os marcadores iniciais, centrais e finais
  codes[0] = 1;
  codes[1] = 0;
  codes[2] = 1;
  codes[31] = 0;
  codes[32] = 1;
  codes[33] = 0;
  codes[34] = 1;
  codes[35] = 0;
  codes[64] = 1;
  codes[65] = 0;
  codes[66] = 1;
  //calculando L-code e R-code dos 8 dígitos
  int index = 3;
  char ean8[8];
  for(int i=0; index != 64 && i< 8; i++){
      //não interferir nas areas centrais
      if(index == 31){
        index = 36;
      }
    //L code
      if(i <  4){
        strcpy(ean8, LCode[imagem.digits[i]]);

        for(int j = 0; j< 7; j++){
          codes[index] = ean8[j] - '0';
          index++;
        }
      }
    //R code
      if(i >=  4){
        strcpy(ean8, RCode[imagem.digits[i]]);

        for(int j = 0; j< 7; j++){
          codes[index] = ean8[j] - '0';
          index++;
        }
      }
  }
}
void GerarMatriz(imagemPBM imagem){
  int altura = imagem.altura;
  int largura = imagem.largura;
  int espacamento = imagem.espacamentoLateral;
  int pixels = imagem.qtdPixels;


  int count=0;
  /*
    for(int i =0; i<67;i++){
    printf("%d", codes[i]);
  }
  */
 int estado = 0;
 int index=0;
 printf("altura %d\n", altura);
  for(int i = 0; i<altura; i++){
    index=0;
    for(int j = 0; j<largura; j++){
      if(i>=espacamento && i<altura-espacamento && j>=espacamento && j<largura-espacamento && index!=67){
        //codes = [1,0,1]
        //000111222, repetir a quantidade de pixels
        imagem.Matriz[i][j] = codes[index];
        count++;
        if (count >= pixels) {
          index++;
          count = 0;
        }
        //6 pixels
        //101 = 3*qtdpixels, 01010
      }
    }
    printf("\n");
  }

}
void printarArquivo(imagemPBM imagem){
  FILE * file;
  file = fopen(imagem.nomeArquivo, "w+");
  fprintf(file, "P1\n");
  fprintf(file, "%d %d\n", imagem.largura, imagem.altura);
  for(int i = 0; i<imagem.altura; i++){
    for(int j = 0; j<imagem.largura; j++){
      fprintf(file, "%d ", imagem.Matriz[i][j]);
    }
    fprintf(file, "\n");
  }
  fclose(file);

}
bool ehPar(int n){
  if(n%2==0 || n==1){
    return true;
  }
  return false;
}
int proximoMultiplo10(int n){
  //próximo múltiplo de 10
  //35, 36, 37, 38, 39, 40, 40%10 = 0
  if(n%10==0){
    return n;
  }
  return proximoMultiplo10(n+1);
}
int main(void) {
  imagemPBM imagem;
  //argumento obrigatório
  char *identificador = malloc(sizeof(char*)*100);
  //argumentos opcionais
  char *espacamentoLateral = malloc(sizeof(char*)*100) ;
  char *qtdPixels = malloc(sizeof(char*)*100);
  char *alturaCodigo = malloc(sizeof(char*)*100);
  char *nomeArquivo = malloc(sizeof(char*)*100);

  receber();

  printf("\n");
  printf("----Argumentos obrigatorios----\n");
  printf("\n");
  printf("Digite o identificador: ");
  fgets(identificador, 100, stdin);

  printf("\n");
  printf("----Argumentos opcionais----\n");
  printf("\n");
  printf("Digite o espaçamento lateral do codigo na imagem: ");
  fgets(espacamentoLateral, 100, stdin);
  printf("Digite a quantidade de pixels por area: ");
  fgets(qtdPixels, 100, stdin);
  printf("Digite a altura do codigo de barras: ");
  fgets(alturaCodigo, 100, stdin);

  printf("Digite o nome do arquivo: ");
  fgets(nomeArquivo, 100, stdin);
  printf("\n");

  bool ok;
  ok=true;
  if(strlen(identificador)==9 && ok == true){
    for(int i = 0; i < strlen(identificador); i++){
      if(identificador[i]!='\n'){
        //não é um valor numérico
        if(isdigit(identificador[i])==0){
          printf("Identificador contém valores não numéricos.\n");
          ok = false;
        }
        if(isdigit(identificador[i])!=0){
          imagem.digits[i] = identificador[i] - '0';
        }
      }
    }
    //numero verificador
    int soma=0;
    for(int i = 0; i< 7; i++){
      if(ehPar(imagem.digits[i])==true){
        soma = soma + imagem.digits[i]*3;
      }
      if(ehPar(imagem.digits[i])==false){
        soma = soma + imagem.digits[i]*1;
      }
    }
    int proximoMultiplo = proximoMultiplo10(soma);
    if(imagem.digits[7]!=proximoMultiplo-soma){
      printf("O dígito verificador do identificador é inválido.\n");
      ok = false;
    }
  }
  //não tem 8 dígitos
  if(strlen(identificador)!=9){
    printf("Identificador não possui 8 dígitos.\n");
    ok = false;
  }

  if(ok == true){
    //se o valor da string for 1, quer dizer que só contem /n
    imagem.espacamentoLateral = (strlen(espacamentoLateral) == 1) ? 4 : atoi(espacamentoLateral);
    imagem.qtdPixels = (strlen(qtdPixels) == 1) ? 3 : atoi(qtdPixels);
    imagem.alturaCodigo = (strlen(alturaCodigo) == 1) ? 50 : atoi(alturaCodigo);
    imagem.altura = (imagem.alturaCodigo) + (imagem.espacamentoLateral*2);
    imagem.largura = (67*imagem.qtdPixels) + (imagem.espacamentoLateral*2);

    imagem.nomeArquivo =  malloc(sizeof(char*)*500);
    strcpy(imagem.nomeArquivo, (strlen(nomeArquivo) == 1) ? "exemplo.pbm" : nomeArquivo);

    //alocando espaço para a matriz
    imagem.Matriz = (int **)malloc(imagem.altura * sizeof(int *));

    for(int i = 0; i < imagem.altura; i++){
      imagem.Matriz[i] = (int *)malloc(imagem.largura * sizeof(int));
      memset(imagem.Matriz[i], 0, imagem.largura * sizeof(int));
    }


    free(espacamentoLateral);
    espacamentoLateral = NULL;
    free(qtdPixels);
    qtdPixels = NULL;
    free(alturaCodigo);
    alturaCodigo = NULL;
    free(nomeArquivo);
    nomeArquivo = NULL;

    GerarCodigoEAN8(imagem);
    GerarMatriz(imagem);
    for(int i  = 0; i<imagem.altura; i++){
    for(int j = 0; j<imagem.largura; j++){
      printf("%d", imagem.Matriz[i][j]);
    }
    printf("\n");
  }
    printarArquivo(imagem);

  }

  return 0;
}
