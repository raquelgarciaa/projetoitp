#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "definitions.h"
#include <errno.h>

FILE *get_file(char *nome_arquivo)
{
    FILE *arquivo;

    /*
        size_t len = strlen(nome_arquivo);
    if (len > 0 && nome_arquivo[len - 1] == '\n')
    {
        nome_arquivo[len - 1] = '\0';
    }
    */
    // Remove o \n do nome do arquivo

    arquivo = fopen(nome_arquivo, "r");

    return arquivo;
}

void find_first_line(int altura, int largura, FILE *arquivo, char *linha, int *contador)
{
    bool linha_codigo = false;
    bool largura_contagem = true;

    int contador_linha = 0;

    for (int i = 0; i < altura; i++)
    {
        for (int j = 0; j < largura; j++)
        {
            // Leitura do pixel
            int pixel;
            fscanf(arquivo, "%d", &pixel);

            // Encontrar primeira linha do codigo de barras com pixel preto
            if (pixel == 1)
            {
                linha_codigo = true;
            }

            // Encontrar largura do código de barras
            if (linha_codigo == true)
            {
                if (pixel == 1 && largura_contagem == true)
                {
                    *contador = *contador + 1;
                }
                else if (pixel == 0 && largura_contagem == true)
                {
                    largura_contagem = false;
                }
                // Adiciona pixel a linha
                linha[contador_linha] = pixel;
                contador_linha++;
            }
        }
        if (largura_contagem == false)
        {
            break;
        }
    }
}

void find_number(int i, char *linha, int contador, int *contador_number, char *number)
{
    if (i < 28)
    {
        char current_char = '0' + linha[i * contador + contador * 3];

        number[i % 7] = current_char;
        *contador_number = *contador_number + 1;
    }
    else if (i >= 28)
    {
        char current_char = '0' + linha[i * contador + contador * 8];

        number[i % 7] = current_char;
        *contador_number = *contador_number + 1;
    }
}

int main(int argc, char *argv[])
{
    /*
    argumentos passados na linha de comando
    exemplo.pbm
    */

    imagemPBM imagem;
    int largura_numero = 0;

    FILE *arquivo = get_file(argv[1]);

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo '%s': %s\n", argv[1], strerror(errno));
        return 1;
    }

    // Leitura do cabeçalho
    printf("--- Lendo cabeçalho do arquivo PBM...\n");
    if (fscanf(arquivo, "%s", imagem.tipo) != 1 || strcmp(imagem.tipo, "P1") != 0) {
        printf("Formato de arquivo inválido\n");
        return 1;
    }

    printf("Tipo: %s\n", imagem.tipo);

    if (fscanf(arquivo, "%d %d\n", &imagem.largura, &imagem.altura) != 2) {
        printf("Erro ao ler largura e altura do arquivo PBM.\n");
        return 1;
    }

    printf("Largura: %d, Altura: %d\n", imagem.largura, imagem.altura);

    printf("--- Lendo codigo do arquivo PBM...\n");
    char *linha = (char *)malloc(imagem.largura * sizeof(char));
    if (linha == NULL) {
        printf("Erro ao alocar memória para linha.\n");
        return 1;
    }

    find_first_line(imagem.altura, imagem.largura, arquivo, linha, &largura_numero);

    char number[8];
    int contador_number = 0;

    char final_number[8];
    int contador_final_number = 0;

    for (int i = 0; i < 7; i++)
    {
        final_number[i] = '1';
    }

    final_number[7] = '\0';

    for (int i = 0; i < 56; i++)
    {
        // Encontra o 7 valores do código de barras de um digito
        find_number(i, linha, largura_numero, &contador_number, number);

        if (contador_number == 7)
        {

            int numeros_iguais = 0;

            for (int j = 0; j < 10; j++)
            {
                for (int l = 0; l < 8; l++)
                {
                    // Verifica se o número é igual ao decodificado DIREIRA ou ESQUERDA
                    if (i < 28)
                    {
                        if (LCode[j][l] == number[l])
                        {

                            numeros_iguais++;

                            if (numeros_iguais == 7)
                            {
                                char index = '0' + j;
                                final_number[contador_final_number] = index;
                                contador_final_number++;

                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    else if (i >= 28)
                    {
                        if (RCode[j][l] == number[l])
                        {

                            numeros_iguais++;

                            if (numeros_iguais == 7)
                            {
                                char index = '0' + j;
                                final_number[contador_final_number] = index;
                                contador_final_number++;

                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                numeros_iguais = 0;
            }

            contador_number = 0;
        }
    }

    if (contador_final_number != 8)
    {
        printf("Código nao encontrado ou valores inválidos\n");
        return 1;
    }

    // Verifica se o código é válido

    printf("Código: %s\n", final_number);

    free(linha);

    return 0;
}
