# AsteroidsGamePIF

# Contribuidores:
| <img src="https://github.com/victor-uen.png" alt="Imagem de perfil do victor-uen" style="border-radius: 50%; border: 1px solid #000; width: 100px; height: 100px;"> | <img src="https://github.com/Luiiizks.png" alt="Imagem de perfil do Luiiizks" width="100" height="100"> | <img src="https://github.com/gabrielcorreiapontes" alt="Imagem de perfil do gabrielcorreiapontes" width="100" height="100"> 
| :----------------------------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------------------------: 

[Victor Uen](https://github.com/victor-uen) 

[Luiz Flavius](https://github.com/Luiiizks)

[Gabriel Pontes](https://github.com/gabrielcorreiapontes)                                                                                                            

## 📖 Sobre o projeto
Jogo Asteroids desenvolvido na linguagem C utilizando a biblioteca CLI-LIB, jogo completo incluindo sistema de high scores, múltiplos níveis e interface gráfica em terminal. Projeto desenvolvido para a disciplina de Programação Imperativa e Funcional do CESAR School.

## 🎯 Objetivo do Jogo
O objetivo é controlar uma nave espacial (A) e destruir asteroides (*) que caem do topo da tela. O jogador deve sobreviver o máximo de tempo possível, acumulando pontos e avançando de nível. O jogo termina quando todas as vidas são perdidas.

## 🎮 Controles
- **A** : Move a nave para a esquerda
- **D** : Move a nave para a direita  
- **Espaço**: Atira
- **W/S** : Navega no menu
- **Enter**: Confirma seleção
- **ESC**: Volta ao menu / Sai do jogo

## 🏆 Funcionalidades

### Sistema de Pontuação
- **100 pontos** por asteroide destruído
- **Progressão de níveis** a cada 500 pontos
- **Sistema de vidas** (3 vidas iniciais)
- **High scores em arquivos** salvos em arquivo

### Mecânicas de Jogo
- **Spawn asteroids**: Asteroides aparecem mais frequentemente conforme o nível aumenta
- **Detecção de colisões**: Entre nave, tiros e asteroides
- **Interface colorida**: Diferentes cores para cada elemento do jogo

## 🛠️ Tecnologias Utilizadas
- **Linguagem**: C
- **Biblioteca**: CLI-LIB (screen, keyboard, timer)

## 📋 Requisitos Implementados
- ✅ **Estruturas personalizadas** (Nave, Asteroid, Tiro, Jogador)
- ✅ **Ponteiros e referências** (Listas encadeadas)
- ✅ **Alocação dinâmica de memória** (malloc, free)
- ✅ **Listas encadeadas** (Asteroides e tiros)
- ✅ **Matrizes bidimensionais** (Sistema de high scores)
- ✅ **Manipulação de arquivos** (High score)
- ✅ **Integração com biblioteca externa** (CLI-LIB)

## 🚀 Como Executar

### Pré-requisitos
- Sistema Linux/MacOS
- Compilador GCC

### Compilação e Execução

# Clone o repositório
- git clone https://github.com/Luiiiks/AsteroidsGamePIF.git
- cd AsteroidsGamePIF

# Compile o projeto
- make clean
- make

# Execute o jogo
- make run

# ScreenCast do Jogo

[ScreenCast GoogleDrive](https://drive.google.com/file/d/1iaggQmgk8Pck7_RbknjzOFmD-m2WMyif/view?usp=sharing)
