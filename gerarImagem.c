#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "definitions.h"

int codes[67];
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
  int index=0;
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
void gravarArquivoPBM(imagemPBM imagem, FILE *file) {
    fprintf(file, "P1\n");
    fprintf(file, "%d %d\n", imagem.largura, imagem.altura);
    for (int i = 0; i < imagem.altura; i++) {
        for (int j = 0; j < imagem.largura; j++) {
            fprintf(file, "%d ", imagem.Matriz[i][j]);
        }
        fprintf(file, "\n");
    }
}

void printarArquivo(imagemPBM imagem) {
    FILE *fileTeste = fopen(imagem.nomeArquivo, "r");
    if (fileTeste == NULL) {
        FILE *file = fopen(imagem.nomeArquivo, "w");
        gravarArquivoPBM(imagem, file);
        fclose(file);
    } else {
        fclose(fileTeste);
        int sobrescrever;
        printf("O arquivo já existe, deseja sobrescrever? (1 - sim, 0 - não): ");
        scanf("%d", &sobrescrever);
        if (sobrescrever == 1) {
            FILE *file = fopen(imagem.nomeArquivo, "w");
            gravarArquivoPBM(imagem, file);
            fclose(file);
        } else {
            printf("Arquivo resultante já existe\n");
        }
    }
}
int main(void) {
  imagemPBM imagem;
  //argumento obrigatório
  char *identificador = malloc(sizeof(char*)*100);
  //argumentos opcionais
  char *espacamentoLateral = malloc(sizeof(char*)*100) ;
  char *qtdPixels = malloc(sizeof(char*)*100);
  char *alturaCodigo = malloc(sizeof(char*)*100);
  char *nomeArquivo = malloc(sizeof(char)*100);


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

  printf("Digite o nome do arquivo com extensao PBM: ");
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

      size_t len = strlen(nomeArquivo);
      if (len > 0 && nomeArquivo[len - 1] == '\n') {
          nomeArquivo[len - 1] = '\0';
      }
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
    printarArquivo(imagem);

  }

  return 0;
}
