#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "definitions.h"

int codes[67];
bool ehPar(int n)
{
  if (n % 2 == 0 || n == 1)
  {
    return true;
  }
  return false;
}
int proximoMultiplo10(int n)
{
  // próximo múltiplo de 10
  // 35, 36, 37, 38, 39, 40, 40%10 = 0
  if (n % 10 == 0)
  {
    return n;
  }
  return proximoMultiplo10(n + 1);
}
void gerarCodigoEAN8(imagemPBM imagem)
{
  // adicionando os marcadores iniciais, centrais e finais
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
  // calculando L-code e R-code dos 8 dígitos
  int index = 3;
  char ean8[8];
  for (int i = 0; index != 64 && i < 8; i++)
  {
    // não interferir nas areas centrais
    if (index == 31)
    {
      index = 36;
    }
    // L code
    if (i < 4)
    {
      strcpy(ean8, LCode[imagem.digits[i]]);

      for (int j = 0; j < 7; j++)
      {
        codes[index] = ean8[j] - '0';
        index++;
      }
    }
    // R code
    if (i >= 4)
    {
      strcpy(ean8, RCode[imagem.digits[i]]);

      for (int j = 0; j < 7; j++)
      {
        codes[index] = ean8[j] - '0';
        index++;
      }
    }
  }
}
void gerarMatriz(imagemPBM imagem)
{
  int altura = imagem.altura;
  int largura = imagem.largura;
  int espacamento = imagem.espacamentoLateral;
  int pixels = imagem.qtdPixels;

  int count = 0;
  int index = 0;
  for (int i = 0; i < altura; i++)
  {
    index = 0;
    for (int j = 0; j < largura; j++)
    {
      if (i >= espacamento && i < altura - espacamento && j >= espacamento && j < largura - espacamento && index != 67)
      {
        // codes = [1,0,1]
        // 000111222, repetir a quantidade de pixels
        imagem.Matriz[i][j] = codes[index];
        count++;
        if (count >= pixels)
        {
          index++;
          count = 0;
        }
        // 6 pixels
        // 101 = 3*qtdpixels, 01010
      }
    }
  }
}
void gravarArquivoPBM(imagemPBM imagem, FILE *file)
{
  fprintf(file, "P1\n");
  fprintf(file, "%d %d\n", imagem.largura, imagem.altura);
  for (int i = 0; i < imagem.altura; i++)
  {
    for (int j = 0; j < imagem.largura; j++)
    {
      fprintf(file, "%d ", imagem.Matriz[i][j]);
    }
    fprintf(file, "\n");
  }
}

void printarArquivo(imagemPBM imagem)
{
  FILE *fileTeste = fopen(imagem.nomeArquivo, "r");
  if (fileTeste == NULL)
  {
    FILE *file = fopen(imagem.nomeArquivo, "w");
    gravarArquivoPBM(imagem, file);
    fclose(file);
  }
  else
  {
    fclose(fileTeste);
    int sobrescrever;
    printf("O arquivo já existe, deseja sobrescrever? (1 - sim, 0 - não): ");
    scanf("%d", &sobrescrever);
    if (sobrescrever == 1)
    {
      FILE *file = fopen(imagem.nomeArquivo, "w");
      gravarArquivoPBM(imagem, file);
      fclose(file);
    }
    else
    {
      printf("Arquivo resultante já existe\n");
    }
  }
}
int main(int argc, char *argv[])
{
  /*
  argumentos não passados na linha de comando
  identificador 40170725 espacamento pixels altura nome

  argumentos passados na linha de comando
  identificador 40170725 espacamento 1 pixels 2 altura 3 nome exemplo.pbm
  */

  imagemPBM imagem;
  // argumento obrigatório
  char *identificador = strcpy(identificador, argv[2]);
  // declarando o tamanho de nome do arquivo
  imagem.nomeArquivo = malloc(sizeof(char *) * 500);

  bool ok;
  ok = true;
  if (strlen(identificador) == 8 && ok == true)
  {
    for (int i = 0; i < strlen(identificador); i++)
    {
      if (identificador[i] != '\n')
      {
        // não é um valor numérico
        if (isdigit(identificador[i]) == 0)
        {
          printf("Identificador contém valores não numéricos.\n");
          ok = false;
        }
        if (isdigit(identificador[i]) != 0)
        {
          imagem.digits[i] = identificador[i] - '0';
        }
      }
    }
    // numero verificador
    int soma = 0;
    for (int i = 0; i < 7; i++)
    {
      if (ehPar(imagem.digits[i]) == true)
      {
        soma = soma + imagem.digits[i] * 3;
      }
      if (ehPar(imagem.digits[i]) == false)
      {
        soma = soma + imagem.digits[i] * 1;
      }
    }
    int proximoMultiplo = proximoMultiplo10(soma);
    if (imagem.digits[7] != proximoMultiplo - soma)
    {
      printf("O dígito verificador do identificador é inválido.\n");
      ok = false;
    }
  }
  // não tem 8 dígitos
  if (strlen(identificador) != 8)
  {
    printf("Identificador não possui 8 dígitos.\n");
    ok = false;
  }

  if (ok == true)
  {
    // percorrer linha de comando para argumentos opcionais

    // se a última posicao for nome quer dizer que nenhum nome foi inserido
    if (strcmp(argv[argc - 1], "nome") == 0)
    {
      strcpy(imagem.nomeArquivo, "exemplo.pbm");
    }
    if (strcmp(argv[argc - 1], "nome") != 0)
    {
      strcpy(imagem.nomeArquivo, argv[argc - 1]);
    }

    for (int i = 3; i < argc - 1; i++)
    {
      if (strcmp(argv[i], "espacamento") == 0)
      {
        // não é numero
        if (isdigit(argv[i + 1][0]) == 0)
        {
          imagem.espacamentoLateral = 4;
        }
        if (isdigit(argv[i + 1][0]) != 0)
        {
          imagem.espacamentoLateral = atoi(argv[i + 1]);
        }
      }
      if (strcmp(argv[i], "pixels") == 0)
      {
        // não é numero
        if (isdigit(argv[i + 1][0]) == 0)
        {
          imagem.qtdPixels = 3;
        }
        if (isdigit(argv[i + 1][0]) != 0)
        {
          imagem.qtdPixels = atoi(argv[i + 1]);
        }
      }
      if (strcmp(argv[i], "altura") == 0)
      {
        // não é numero
        if (isdigit(argv[i + 1][0]) == 0)
        {
          imagem.alturaCodigo = 50;
        }
        if (isdigit(argv[i + 1][0]) != 0)
        {
          imagem.alturaCodigo = atoi(argv[i + 1]);
        }
      }
    }
    imagem.altura = (imagem.alturaCodigo) + (imagem.espacamentoLateral * 2);
    imagem.largura = (67 * imagem.qtdPixels) + (imagem.espacamentoLateral * 2);

    // alocando espaço para a matriz
    imagem.Matriz = (int **)malloc(imagem.altura * sizeof(int *));

    for (int i = 0; i < imagem.altura; i++)
    {
      imagem.Matriz[i] = (int *)malloc(imagem.largura * sizeof(int));
      memset(imagem.Matriz[i], 0, imagem.largura * sizeof(int));
    }

    gerarCodigoEAN8(imagem);
    gerarMatriz(imagem);
    printarArquivo(imagem);
  }

  return 0;
}
