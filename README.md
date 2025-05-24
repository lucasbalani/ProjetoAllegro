# ProjetoAllegro


# 🎨 Projeto: Editor de Formas Gráficas com Allegro 5

## ✅ Descrição

Este projeto permite criar, selecionar, mover e redimensionar formas geométricas básicas (quadrado, retângulo, círculo e triângulo) através de uma interface gráfica utilizando a biblioteca **Allegro 5** em linguagem C.

---

## 🚀 **Funcionalidades principais:**

- Criar formas através de menu gráfico.
- Selecionar formas com clique do mouse.
- Mover formas com setas ou arrastando com o mouse.
- Redimensionar formas em tempo real com `+` e `-`.

---

## 🛠️ **Como preparar o ambiente do zero**

### ✅ 1. Instale um compilador C

**Windows:**

- Baixe e instale **MSYS2**:  
  [https://www.msys2.org/](https://www.msys2.org/)

- Abra o terminal `MSYS2 UCRT64` ou `MSYS2 MinGW 64-bit`.

- Atualize os pacotes:

```bash
pacman -Syu
```

- Feche e abra o terminal novamente.  
Repita o comando se pedir mais atualizações.

---

### ✅ 2. Instale o compilador GCC e Allegro

No terminal do MSYS2:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-allegro
```

Isso instalará:

✅ GCC (compilador C)  
✅ Allegro 5 com todos os módulos: `primitives`, `font`, `ttf`, `image`.

---

### ✅ 3. Configure o ambiente

Sempre que for compilar ou rodar o programa, abra o terminal:  
➡️ `MSYS2 UCRT64` ou `MinGW 64-bit`

---

### ✅ 4. Prepare o código

1. Copie o código fonte para um arquivo:  
➡️ `main.c`

2. Coloque na mesma pasta uma fonte `.ttf` (ex.: `arial.ttf`) que será carregada para desenhar os textos.

---

### ✅ 5. Compile o projeto

No terminal, navegue até a pasta do `main.c`:

```bash
cd /c/CAMINHO/DA/PASTA
```

Depois compile com:

```bash
gcc main.c -o main.exe -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_image
```

---

### ✅ 6. Execute o programa

No mesmo terminal:

```bash
./main.exe
```

---

## ✅ **Como usar:**

| Ação | Tecla/ação |
|-------|-----------|
| Abrir menu | `M` |
| Selecionar forma | Clique sobre ela |
| Mover forma | Setas ou arrastando com mouse |
| Redimensionar | `+` ou `-` |
| Criar forma | No menu, escolha com `↑` `↓` e confirme com `ENTER` |

---

## ✅ **Exemplo de fluxo:**

1. Pressione `M` → escolha uma forma e pressione `ENTER`.  
2. Clique sobre a forma para selecioná-la.  
3. Mova com as setas ou arrastando.  
4. Redimensione com `+` ou `-`.  
5. Crie mais formas ou redimensione todas como quiser!

---

## 🆘 **Dúvidas comuns:**

| Problema | Solução |
|----------|---------|
| `gcc: command not found` | Não instalou o compilador. Execute: `pacman -S mingw-w64-ucrt-x86_64-gcc` |
| `allegro not found` | Não instalou a Allegro. Execute: `pacman -S mingw-w64-ucrt-x86_64-allegro` |
| Fontes não aparecem | Coloque um arquivo `.ttf` na mesma pasta (ex.: `arial.ttf`) |
| Janela fecha rápido | Rode pelo terminal para ver as mensagens de erro |

---

## ✅ **Compatibilidade:**

- ✅ Windows → com **MSYS2**  
- ✅ Linux → `sudo apt install build-essential liballegro5-dev`  
- ✅ MacOS → `brew install allegro`

**Obs:** no Linux/Mac basta ajustar o comando de compilação conforme o sistema.

---

## ✅ **Dependências:**

- Allegro 5  
- GCC  

---

## ✅ **Licença:**

Este projeto é de uso livre para fins educacionais.
