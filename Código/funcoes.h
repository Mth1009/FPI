#ifndef FUNCOES_H
#define FUNCOES_H

#include <QImage>

#include <vector>

void espelhar_img(unsigned char *dados, int largura, int altura, char modo, int vezes);
void conversao_cinza(unsigned char *dados, int largura, int altura);
void quantizacao(unsigned char *dados, int largura, int altura, int numero_tons);
std::vector<int> hist(const unsigned char *dados, int largura, int altura);
QImage imagem_histograma(const std::vector<int> &histograma);
void brilho(unsigned char *dados, int largura, int altura, int n);
void contraste(unsigned char *dados, int largura, int altura, int n);
void negativo(unsigned char *dados, int largura, int altura);
unsigned char* ampliar(unsigned char *dados, int largura, int altura);
void rotacionar90(unsigned char *dados, int largura, int altura, int n);
void zoomOut(unsigned char *dados, int largura, int altura, int sx, int sy);
void convolucao(unsigned char *dados, int largura, int altura, const double kernel[3][3], bool passa_baixas, bool adicionar_127);
void equalizar_histograma(unsigned char *dados, int largura, int altura);
void matching_histograma(unsigned char *dados, int largura, int altura,
                         const unsigned char *referencia,
                         int largura_referencia, int altura_referencia);
#endif // FUNCOES_H
