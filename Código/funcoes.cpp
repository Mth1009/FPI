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

std::vector<int> hist(const unsigned char *dados, int largura, int altura){
    std::vector<int> histograma(256, 0);

    if(dados == nullptr || largura <= 0 || altura <= 0){
        return histograma;
    }

    int total_pixels = largura * altura;
    for(int i = 0; i < total_pixels; i++){
        int pos = i * 3;
        int luminancia = static_cast<int>(
            0.299 * dados[pos] +
            0.587 * dados[pos + 1] +
            0.114 * dados[pos + 2]);
        histograma[luminancia]++;
    }

    return histograma;
}

QImage imagem_histograma(const std::vector<int> &histograma){
    const int largura = 256;
    const int altura = 256;

    QImage img(largura, altura, QImage::Format_RGB32);
    img.fill(Qt::white);

    if(histograma.size() < 256){
        return img;
    }

    int maior = *std::max_element(histograma.begin(), histograma.begin() + 256);
    if(maior <= 0){
        return img;
    }

    for(int x = 0; x < largura; x++){
        int altura_coluna = static_cast<int>(
            std::round((static_cast<double>(histograma[x]) / maior) * altura));

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
    if(dados == nullptr || largura <= 0 || altura <= 0){
        return nullptr;
    }

    int nova_altura = altura * 2 - 1;
    int nova_largura = largura * 2 - 1;
    int total_bytes = nova_altura * nova_largura * 3;
    unsigned char *ampliado = new unsigned char[total_bytes]();

    // Primeiro passo: copia os pixels originais e interpola ao longo das linhas.
    for(int y = 0; y < altura; y++){
        int novo_y = y * 2;

        for(int x = 0; x < largura; x++){
            int novo_x = x * 2;
            int pos_original = (y * largura + x) * 3;
            int pos_ampliada = (novo_y * nova_largura + novo_x) * 3;

            for(int canal = 0; canal < 3; canal++){
                ampliado[pos_ampliada + canal] = dados[pos_original + canal];
            }

            if(x < largura - 1){
                int pos_direita = (y * largura + x + 1) * 3;
                int pos_meio = (novo_y * nova_largura + novo_x + 1) * 3;

                for(int canal = 0; canal < 3; canal++){
                    ampliado[pos_meio + canal] = static_cast<unsigned char>(
                        (static_cast<int>(dados[pos_original + canal]) +
                         static_cast<int>(dados[pos_direita + canal])) / 2);
                }
            }
        }
    }

    // Segundo passo: interpola as linhas que ficaram em branco.
    for(int y = 1; y < nova_altura; y += 2){
        for(int x = 0; x < nova_largura; x++){
            int pos_cima = ((y - 1) * nova_largura + x) * 3;
            int pos_baixo = ((y + 1) * nova_largura + x) * 3;
            int pos_meio = (y * nova_largura + x) * 3;

            for(int canal = 0; canal < 3; canal++){
                ampliado[pos_meio + canal] = static_cast<unsigned char>(
                    (static_cast<int>(ampliado[pos_cima + canal]) +
                     static_cast<int>(ampliado[pos_baixo + canal])) / 2);
            }
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



void convolucao(unsigned char *dados, int largura, int altura,
                const double kernel[3][3], bool passa_baixas, bool adicionar_127){
    if(dados == nullptr || kernel == nullptr || largura < 3 || altura < 3){
        return;
    }

    if(!passa_baixas){
        conversao_cinza(dados, largura, altura);
    }

    int total_bytes = largura * altura * 3;
    std::vector<unsigned char> original(dados, dados + total_bytes);

    // As bordas permanecem iguais; a convolucao e aplicada somente no interior.
    for(int y = 1; y < altura - 1; y++){
        for(int x = 1; x < largura - 1; x++){
            int pos_destino = (y * largura + x) * 3;

            for(int canal = 0; canal < 3; canal++){
                double soma = 0.0;

                for(int desloc_y = -1; desloc_y <= 1; desloc_y++){
                    for(int desloc_x = -1; desloc_x <= 1; desloc_x++){
                        int pos_origem =
                            ((y + desloc_y) * largura + (x + desloc_x)) * 3 + canal;

                        // Rotacao de 180 graus do kernel antes da aplicacao.
                        double peso = kernel[1 - desloc_y][1 - desloc_x];
                        soma += peso * original[pos_origem];
                    }
                }

                if(adicionar_127){
                    soma += 127.0;
                }

                int resultado = std::clamp(
                    static_cast<int>(std::round(soma)), 0, 255);
                dados[pos_destino + canal] = static_cast<unsigned char>(resultado);
            }
        }
    }
}

void equalizar_histograma(unsigned char *dados, int largura, int altura){
    if(dados == nullptr || largura <= 0 || altura <= 0){
        return;
    }

    // O mapeamento e calculado a partir do histograma de luminancia.
    std::vector<int> histograma_luminancia = hist(dados, largura, altura);
    std::vector<int> histograma_cumulativo(256, 0);
    unsigned char mapeamento[256];

    histograma_cumulativo[0] = histograma_luminancia[0];
    for(int tom = 1; tom < 256; tom++){
        histograma_cumulativo[tom] =
            histograma_cumulativo[tom - 1] + histograma_luminancia[tom];
    }

    int total_pixels = largura * altura;
    for(int tom = 0; tom < 256; tom++){
        double normalizado =
            static_cast<double>(histograma_cumulativo[tom]) / total_pixels;
        mapeamento[tom] = static_cast<unsigned char>(
            std::clamp(static_cast<int>(std::round(255.0 * normalizado)), 0, 255));
    }

    // O mesmo mapeamento de luminancia e aplicado independentemente a R, G e B.
    int total_bytes = total_pixels * 3;
    for(int i = 0; i < total_bytes; i++){
        dados[i] = mapeamento[dados[i]];
    }
}

void matching_histograma(unsigned char *dados, int largura, int altura,const unsigned char *referencia,int largura_referencia, int altura_referencia ){
    if(dados == nullptr || referencia == nullptr ||largura <= 0 || altura <= 0 ||largura_referencia <= 0 || altura_referencia <= 0){
        return;
    }

    const std::vector<int> histograma_origem = hist(dados, largura, altura);
    const std::vector<int> histograma_referencia =hist(referencia, largura_referencia, altura_referencia);

    double acumulado_origem[256] = {};
    double acumulado_referencia[256] = {};
    unsigned char mapeamento[256] = {};

    const double total_origem = static_cast<double>(largura) * altura;
    const double total_referencia =
        static_cast<double>(largura_referencia) * altura_referencia;

    acumulado_origem[0] = histograma_origem[0] / total_origem;
    acumulado_referencia[0] =
        histograma_referencia[0] / total_referencia;

    for(int tom = 1; tom < 256; tom++){
        acumulado_origem[tom] = acumulado_origem[tom - 1] +
                                histograma_origem[tom] / total_origem;
        acumulado_referencia[tom] = acumulado_referencia[tom - 1] +
                                    histograma_referencia[tom] / total_referencia;
    }

    // Para cada tom da origem, encontra o tom cuja distribuicao cumulativa  eh  a mais proxima na imagem de referencia.
    for(int tom_origem = 0; tom_origem < 256; tom_origem++){
        int melhor_tom = 0;
        double menor_diferenca = std::abs(
            acumulado_origem[tom_origem] - acumulado_referencia[0]);

        for(int tom_referencia = 1; tom_referencia < 256; tom_referencia++){
            const double diferenca = std::abs(
                acumulado_origem[tom_origem] -
                acumulado_referencia[tom_referencia]);

            if(diferenca < menor_diferenca){
                menor_diferenca = diferenca;
                melhor_tom = tom_referencia;
            }
        }

        mapeamento[tom_origem] = static_cast<unsigned char>(melhor_tom);
    }

    const int total_pixels = largura * altura;
    for(int pixel = 0; pixel < total_pixels; pixel++){
        const int pos = pixel * 3;
        const unsigned char novo_tom = mapeamento[dados[pos]];
        dados[pos] = novo_tom;
        dados[pos + 1] = novo_tom;
        dados[pos + 2] = novo_tom;
    }
}
