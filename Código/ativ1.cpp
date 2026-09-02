#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <filesystem>

#include <iostream>

int main() {
    int largura, altura, canais;

    // Lê a imagem
    unsigned char* dados = stbi_load("test_images/Gramado_72k.jpg",&largura,&altura,&canais,3);

    if (dados == nullptr) {
        std::cout << "Erro ao abrir a imagem.\n";
        return 1;
    }

    // Regrava em JPEG com outro nome
    int resultado = stbi_write_jpg("regravada2.jpg",largura,altura,3,dados,10);

    std::cout << std::filesystem::current_path() << std::endl;
    if (resultado == 0) {
        std::cout << "Erro ao salvar a imagem.\n";
    } else {
        std::cout << "Imagem regravada com sucesso!\n";
    }

    stbi_image_free(dados);

    return 0;
}