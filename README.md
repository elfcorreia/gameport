# Biblioteca Framebuffer

Trabalhar algoritmos gráficos em C++ é uma tarefa inviável sem a utilização de alguma biblioteca gráfica.

Vincular uma biblioteca gráfica a um programa C++ pode ser uma tarefa hercúlea por si só. Como as vezes usar uma biblioteca é mais difícil que desenvolver o algoritmo gráfico em si, desenvolvi essa biblioteca baseada em um único cabeçalho.

Outro ponto importante são as abstração fornecida pela biblioteca gráfica. Para não tirar o foco no desenvolvimento do algoritmo, optei pela abstração mais simples possível: o framebuffer.

O Framebuffer consiste de uma porção de memória que representa os pixeis a serem exibidos. A manipulação gráfica se dá pela manipulação das cores dos píxeis no buffer. A falta de abstrações geométricas como linhas e polígonos; de serviços gráficos comuns como preenchimento de áreas, além da ausência de estado do contexto da pintura, traz duas vantagens:

 - os algoritmos desenvolvidos não dependem de abstrações da biblioteca.
 - podem utilizar outros framebuffers de outras bibliotecas gráficas, ou até mesmo do hardware.

```
int* init(int width, int height);
int refresh();
```

