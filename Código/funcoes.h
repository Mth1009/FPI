#ifndef FUNCOES_H
#define FUNCOES_H

void espelhar_img(unsigned char *dados, int largura, int altura, char modo, int vezes);
void conversao_cinza(unsigned char *dados, int largura, int altura);
void quantizacao(unsigned char *dados, int largura, int altura, int numero_tons);
void histograma(unsigned char *dados, int largura, int altura);
void brilho(unsigned char *dados, int largura, int altura, int n);
void contraste(unsigned char *dados, int largura, int altura, int n);
void negativo(unsigned char *dados, int largura, int altura);
#endif // FUNCOES_H
