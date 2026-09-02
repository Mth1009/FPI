#ifndef FUNCOES_H
#define FUNCOES_H

void espelhar_img(unsigned char *dados, int largura, int altura, char modo, int vezes);
void conversao_cinza(unsigned char *dados, int largura, int altura);
void quantizacao(unsigned char *dados, int largura, int altura, int numero_tons);
#endif // FUNCOES_H
