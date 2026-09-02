cmake_minimum_required(VERSION 3.16)
project(Trab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Procura os componentes do Qt6 necessários
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Adiciona o executável com os arquivos modularizados
add_executable(Trab1
    main.cpp
    mainwindow.cpp
    mainwindow.h
    funcoes.cpp
    funcoes.h
)

# Conecta as bibliotecas do Qt ao projeto
target_link_libraries(Trab1 PRIVATE Qt6::Core Qt6::Widgets)
