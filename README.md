# Blaze Studio Provs

**Editor de coreografias para jogos de dança com captura de movimentos por Inteligência Artificial.**

Este projeto é um editor visual que permite criar, editar e gerenciar coreografias para jogos de dança, utilizando tecnologia de visão computacional e IA para capturar e analisar movimentos em tempo real.

## Requisitos

- **Qt 6.10.2** ou superior
- **Qt Creator**
- **MSVC 2022 64-bit** (Microsoft Visual Studio Compiler)
- **OpenCV** (buildado para Windows)
- **CMake 3.16** ou superior

## Configuração do Ambiente

### 1. OpenCV

O projeto espera que o OpenCV esteja instalado em `C:\opencv\build`. Se você tem o OpenCV em outro local, edite o `CMakeLists.txt` e altere os caminhos:

```cmake
set(OpenCV_DIR "C:/opencv/build")
include_directories("C:/opencv/build/include")
```

### 2. Qt Creator

Certifique-se de ter o Qt Creator configurado com:

- Kit MSVC 2022 64-bit
- Qt 6.10.2 ou superior

## Como Buildar

1. Abra o projeto no **Qt Creator**
2. Selecione o kit **Desktop Qt 6.10.2 MSVC2022 64bit**
3. Escolha a configuração **Debug** ou **Release**
4. Clique em **Build** (Ctrl+B)

## Estrutura do Projeto

```
blazestudioprovs/
├── CMakeLists.txt      # Configuração do CMake
├── main.cpp            # Ponto de entrada da aplicação
├── mainwindow.cpp      # Implementação da janela principal
├── mainwindow.h        # Header da janela principal
├── mainwindow.ui       # Interface UI do Qt Designer
└── build/              # Diretório de build (gerado automaticamente)
```

## Funcionalidades

- **Editor de Coreografias**: Interface gráfica para criação e edição de sequências de dança
- **Captura de Movimentos por IA**: Utiliza OpenCV e algoritmos de visão computacional para detectar e rastrear movimentos do corpo
- **Análise de Poses**: Reconhecimento de poses e gestos em tempo real através da câmera
- **Timeline de Movimentos**: Visualização e edição temporal das coreografias
- **Suporte para Jogos de Dança**: Exportação de coreografias em formatos compatíveis com jogos

## Executar

Após o build, execute o projeto diretamente pelo Qt Creator (Ctrl+R) ou encontre o executável em:

```
build/Desktop_Qt_6_10_2_MSVC2022_64bit-Debug/blazestudioprovs.exe
```

## Versão do OpenCV

A aplicação exibe a versão do OpenCV no console ao iniciar.

## Licença

Este projeto está sob a licença especificada pelo desenvolvedor.
