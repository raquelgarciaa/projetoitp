#ifndef IMAGEM_H
#define IMAGEM_H

typedef struct imagem{
  char *nomeArquivo;
  char tipo[3];
  int largura;
  int altura;
  int espacamentoLateral;
  int alturaCodigo;
  int qtdPixels;
  int **Matriz;

  int digits[8];
}imagemPBM;

//guardar o código ean8 em outro lugar
extern char LCode[10][8];
extern char RCode[10][8];


#endif
