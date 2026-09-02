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
