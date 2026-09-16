#include "funcoes.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void espelhar_img(unsigned char *dados, int largura, int altura, char modo, int vezes){
    int bytes_linha = largura*3;
    if (vezes%2==0){
        return;
    }
    std::vector<unsigned char> linhatemp(bytes_linha);
    if (tolower(modo) == 'h'){
        for (int y=0; y<altura; y++){
            unsigned char *linhaatual = dados + y*bytes_linha;
            memcpy(linhatemp.data(), linhaatual, bytes_linha);
            for (int x=0; x<largura/2;x++){
                int indice_esq = x*3;
                int indice_dir = (largura - 1 - x) * 3;
                linhaatual[indice_esq] = linhaatual[indice_dir];
                linhaatual[indice_esq+1] = linhaatual[indice_dir+1];
                linhaatual[indice_esq+2] = linhaatual[indice_dir+2];
                linhaatual[indice_dir] = linhatemp[indice_esq];
                linhaatual[indice_dir+1] = linhatemp[indice_esq+1];
                linhaatual[indice_dir+2] = linhatemp[indice_esq+2];
            }
        }
    }
    if (tolower(modo)=='v'){
        for (int x=0; x<altura/2; x++){
            unsigned char *lincima = dados + (x*bytes_linha);
            unsigned char *linbaixo = dados + ((altura-1-x)*bytes_linha);
            memcpy(linhatemp.data(), lincima, bytes_linha);
            memcpy(lincima, linbaixo, bytes_linha);
            memcpy(linbaixo, linhatemp.data(), bytes_linha);
            }
        }
}

void conversao_cinza(unsigned char *dados, int largura, int altura){
    int total_pix = altura*largura;
    for (int x=0;x<total_pix;x++){
        int indice = x*3;
        unsigned char cinza = (unsigned char)(0.299*dados[indice]+0.587*dados[indice+1]+0.114*dados[indice+2]);
        dados[indice]=cinza;
        dados[indice+1]=cinza;
        dados[indice+2]=cinza;

    }
}

void quantizacao(unsigned char *dados, int largura, int altura, int n){
    conversao_cinza(dados, largura, altura);
    if (n<1){
        return;
    }
    int total_bytes = altura*largura*3;
    int maior=0;
    int menor=255;
    for (int x=0;x<total_bytes;x=x+3){
        if (maior<dados[x]){
            maior=dados[x];
        }
        if(menor>dados[x]){
            menor=dados[x];
        }
    }
    int tam_intervalo = maior-menor+1;
    if (tam_intervalo>n){
        double tons_desejados = (double)tam_intervalo/n; // tb no pdf do sor
        for (int i =0;i<total_bytes;i=i+3){
            unsigned char novo = dados[i];
            int bin = static_cast<int>((novo -(menor - 0.5))/tons_desejados);
            bin = std::clamp(bin, 0, n - 1);
            double inicio_bin = menor - 0.5 + bin*tons_desejados;
            double fim_bin = inicio_bin + tons_desejados;
            double centro = (inicio_bin + fim_bin)/2;
            int centro_bin = std::clamp(static_cast<int>(std::round(centro)), 0, 255);
            dados[i]= centro_bin;
            dados[i+1]= centro_bin;
            dados[i+2]= centro_bin;
            
            
        }
    }
}

std::vector <int> hist(unsigned char *dados, int largura, int altura){
    conversao_cinza(dados, largura, altura);
    int total_bytes = largura*altura*3;
    std::vector <int> hist(256);
    for (int i = 0; i<total_bytes; i+=3){
        hist[dados[i]]+=1;
    }
    return hist;
}

QImage imagem_histograma(std::vector<int> hist){
    int largura = 256;
    int altura = 256;

    QImage img(largura, altura, QImage::Format_RGB888);
    img.fill(Qt::white);

    int maior = 0;

    // procura o maior valor do histograma
    for(int i = 0; i < 256; i++){
        if(hist[i] > maior){
            maior = hist[i];
        }
    }

    // desenha cada coluna normalizada
    for(int x = 0; x < 256; x++){

        int altura_coluna = 0;

        if(maior > 0){
            altura_coluna = hist[x] * altura / maior;
        }

        for(int y = altura - 1; y >= altura - altura_coluna; y--){
            img.setPixel(x, y, qRgb(0, 0, 0));
        }
    }

    return img;
}

void brilho(unsigned char *dados, int largura, int altura, int n){
    if (n>255){
        return;
    }
    if (n<-255){
        return;
    }
    int total_bytes = largura*altura*3;
    for(int i = 0; i<total_bytes; i++){
        int novo = dados[i]+n;
        if (novo>255){
            novo=255;
        }
        if (novo<0){
            novo=0;
        }
        dados[i] = novo;
    }
}

void contraste(unsigned char *dados, int largura, int altura, int n){
    if (n>255){
        return;
    }
    if (n<=0){
        return;
    }
    int total_bytes = largura*altura*3;
    for(int i = 0; i<total_bytes; i++){
        int novo = dados[i]*n;
        if (novo>255){
            novo=255;
        }
        if (novo<0){
            novo=0;
        }
        dados[i] = novo;
    }
}

void negativo(unsigned char *dados, int largura, int altura){
    int total_bytes = largura*altura*3;
    for(int i = 0; i<total_bytes; i++){
        unsigned char negativo = 255-dados[i];
        dados[i] = negativo;    
    }
}

unsigned char* ampliar(unsigned char *dados, int largura, int altura){
    int nova_altura = altura*2-1;
    int nova_largura = largura*2-1;
    int total_bytes = nova_altura*nova_largura*3;
    unsigned char*ampliado = new unsigned char[total_bytes];
    int flagv=0;
    int flagh=0;
    int j =0;
    for(int i =0; i<total_bytes;i++){
        if ((i*3)%2==0 && flagv==0 && flagh==0){
            ampliado[i] = dados[i/2];
        }
        j++;
        if (j==nova_largura){
            j=0;
            flagv=1-flagv;
        }
    }
    return ampliado;
}

void rotacionar90(unsigned char *dados, int largura, int altura, int n){
    // n = 1  -> horário
    // n = -1 -> anti-horário

    if(n != 1 && n != -1){
        return;
    }

    int total_bytes = largura * altura * 3;

    unsigned char *novo = new unsigned char[total_bytes];

    for(int y = 0; y < altura; y++){
        for(int x = 0; x < largura; x++){

            int posOriginal = (y * largura + x) * 3;

            int novoX;
            int novoY;

            if(n == 1){
                // 90° horário
                novoX = altura - 1 - y;
                novoY = x;
            }
            else{
                // 90° anti-horário
                novoX = y;
                novoY = largura - 1 - x;
            }

            // A nova imagem tem largura = altura original
            int posNova = (novoY * altura + novoX) * 3;

            novo[posNova]     = dados[posOriginal];
            novo[posNova + 1] = dados[posOriginal + 1];
            novo[posNova + 2] = dados[posOriginal + 2];
        }
    }

    for(int i = 0; i < total_bytes; i++){
        dados[i] = novo[i];
    }

    delete[] novo;
}

void zoomOut(unsigned char *dados, int largura, int altura, int sx, int sy){
    if(sx < 1 || sy < 1){
        return;
    }

    // Arredonda para cima para incluir os pixels que sobrarem nas bordas
    int novaLargura = (largura + sx - 1) / sx;
    int novaAltura = (altura + sy - 1) / sy;

    int total_bytes = novaLargura * novaAltura * 3;

    unsigned char *novo = new unsigned char[total_bytes];

    int novoY = 0;

    for(int y = 0; y < altura; y += sy){

        int novoX = 0;

        for(int x = 0; x < largura; x += sx){

            int somaR = 0;
            int somaG = 0;
            int somaB = 0;

            int quantidade = 0;

            // Percorre o retângulo sx x sy
            for(int j = 0; j < sy; j++){

                for(int i = 0; i < sx; i++){

                    int px = x + i;
                    int py = y + j;

                    // Verifica se ainda está dentro da imagem
                    if(px < largura && py < altura){

                        int pos = (py * largura + px) * 3;

                        somaR += dados[pos];
                        somaG += dados[pos + 1];
                        somaB += dados[pos + 2];

                        quantidade++;
                    }
                }
            }

            int posNova = (novoY * novaLargura + novoX) * 3;

            novo[posNova]     = somaR / quantidade;
            novo[posNova + 1] = somaG / quantidade;
            novo[posNova + 2] = somaB / quantidade;

            novoX++;
        }

        novoY++;
    }

    // Copia a imagem reduzida para o início do vetor original
    for(int i = 0; i < total_bytes; i++){
        dados[i] = novo[i];
    }

    delete[] novo;
}

