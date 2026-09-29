# Laboratório de Processamento de Imagens

Aplicativo desktop desenvolvido em C++ e Qt 6 para aplicar e comparar operações de processamento digital de imagens. A interface exibe a imagem original e o resultado lado a lado, permitindo experimentar diferentes transformações e salvar o resultado em JPEG.

O projeto foi desenvolvido para a disciplina **INF01046 — Fundamentos de Processamento de Imagens**, da UFRGS.

## Funcionalidades

- Leitura e gravação de imagens JPEG.
- Visualização simultânea da imagem original e do resultado.
- Espelhamento horizontal e vertical.
- Conversão para tons de cinza por luminância.
- Quantização de tons.
- Ajustes de brilho e contraste.
- Negativo da imagem.
- Redução e ampliação com interpolação.
- Rotação de 90 graus nos sentidos horário e anti-horário.
- Geração e visualização de histogramas.
- Equalização de histograma.
- Correspondência de histogramas para imagens em tons de cinza.
- Convolução com kernels 3x3 arbitrários e filtros predefinidos, incluindo Gaussiano, Laplaciano, Prewitt e Sobel.

## Tecnologias

- C++17
- Qt 6 Widgets
- CMake 3.16 ou superior
- [stb_image](https://github.com/nothings/stb) e `stb_image_write` para leitura e gravação de imagens

As bibliotecas STB estão incluídas no repositório. Portanto, não é necessário instalar uma biblioteca JPEG separadamente.

## Pré-requisitos

Para compilar o projeto, é necessário ter um compilador com suporte a C++17, CMake e os arquivos de desenvolvimento do Qt 6.

No Ubuntu ou Debian:

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev
```

No Fedora:

```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel
```

## Compilação

Na raiz do repositório, execute:

```bash
cmake -S "Código" -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Depois, inicie o programa:

```bash
./build/Trab1
```

Para recompilar após uma alteração no código, normalmente basta executar novamente:

```bash
cmake --build build --parallel
```

## Como usar

1. Clique em **Abrir imagem** e selecione um arquivo JPEG.
2. Escolha uma operação na lista da barra de ferramentas.
3. Clique em **Aplicar** e informe os parâmetros solicitados, quando houver.
4. Compare a imagem original com o resultado.
5. Use **Restaurar original** para descartar as transformações atuais.
6. Clique em **Salvar resultado** para exportar a imagem processada em JPEG.

Na correspondência de histogramas, a imagem aberta é usada como origem e o programa solicita uma segunda imagem em tons de cinza como referência.

## Estrutura do projeto

```text
FPI/
├── Código/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   ├── funcoes.cpp
│   ├── funcoes.h
│   ├── stb_image.h
│   └── stb_image_write.h
├── INF01046_Trabalho_2.pdf
└── README.md
```

- `mainwindow.*`: interface gráfica e integração das operações.
- `funcoes.*`: algoritmos de processamento de imagens.
- `stb_image*.h`: dependências embarcadas para arquivos de imagem.
- `INF01046_Trabalho_2.pdf`: enunciado acadêmico que orientou a implementação.

## Distribuição

Quem clonar o código-fonte precisa instalar o Qt 6 para compilar. Um usuário final não precisa instalar o Qt se o aplicativo for distribuído com as bibliotecas de execução necessárias.

Para transformar o projeto em um download pronto para uso, uma evolução recomendada é publicar versões na seção **Releases** do GitHub, por exemplo como AppImage no Linux ou como pacote ZIP no Windows gerado com `windeployqt`.

## Contexto acadêmico

Este repositório demonstra implementação de algoritmos de processamento de imagens, manipulação direta de pixels, desenvolvimento de interfaces desktop com Qt e organização de builds com CMake.
