# Aplicação com Visualização por Anaglifo

* 30 de mar - 100 pontos | Armstrong Lohãns

---

##### Descrição da atividade:

> Desenvolva uma aplicação com Visualização 3D por Anaglifo.
>
> A escolha do objeto a ser utilizado é livre, por tanto que seja 3D.
>
> Um exemplo de objeto é o  de opengl.
> Como dito na aula, a estratégia é:
>
> * Gerar duas instancias de visualização do objeto: um na cor vermelha e outro na cor azul.
> * Transladar os objetos em sentido opostos a fim de encontrar o > valor da paralaxe ideal.
> * Rotacionar os objetos em sentido opostos (um horário (angulo positivo) e outro antihorário (angulo negativo)).
> Recomendo que o incremento(/decremento) das tranlações e rotações sejam feitas com valores muito pequenos.
>
> O trabalho será apresentado durante a aula do dia 12/04.

##### Como executar:

Executar o seguinte comando no terminal:

```bash
gcc anaglifo_opengl_armstrong.c -o atividade1-armstrong -lGL -lglut -lm -lGLU -lOpenGL && ./atividade1-armstrong
```
