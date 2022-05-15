// Linha de compilação utilizada: gcc -o main main.c -lpthread -Wall -Wextra
// -pedantic

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  uint16_t i;
  uint16_t n;
  uint16_t nt;
  int16_t **a;
  int16_t **b;
  int **m;
} arq;

void mult(uint16_t n, uint16_t inicio, uint16_t fim, int16_t **a, int16_t **b,
          int **m) {
  uint16_t i, j, k;
  for (i = inicio; i < fim; i++) {
    for (j = inicio; j < fim; j++) {
      for (k = 0; k < n; k++)
        m[i][j] += a[i][k] * b[k][j];
    }
  }
}

void *controlt(void *param) {
  arq pt = *((arq *)param);
  uint16_t inicio = pt.i * (pt.n / pt.nt);
  uint16_t fim = (pt.i == pt.nt - 1 ? pt.n - 1 : inicio + (pt.n / pt.nt) - 1);

  printf("Thread %u executando... ", pt.i);
  printf("Multiplicando do indice %i até %i\n", inicio, fim);

  mult(pt.n, inicio, fim, pt.a, pt.b, pt.m);

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Uso: nome-arquivo num-thread\n");
    return 0;
  }

  FILE *fw_bin = NULL;
  FILE *fl_bin = NULL;
  uint16_t n, nt;

  fl_bin = fopen(argv[1], "rb");
  if (fl_bin == NULL) {
    printf("Arquivo nao encontrado\n");
    return 0;
  }

  int16_t v;
  fread(&v, sizeof(int16_t), 1, fl_bin);
  n = v;
  // printf("%u\n", n);
  int16_t a[n][n];
  int16_t b[n][n];
  int m[n][n];

  for (uint16_t i = 0; i < n; i++) {
    for (uint16_t j = 0; j < n; j++) {
      m[i][j] = 0;
    }
  }

  for (uint16_t i = 0; i < n; i++) {
    for (uint16_t j = 0; j < n; j++) {
      fread(&v, sizeof(int16_t), 1, fl_bin);
      a[i][j] = v;
      // printf("%d ",a[i][j]);
    }
    // printf("\n");
  }
  for (uint16_t i = 0; i < n; i++) {
    for (uint16_t j = 0; j < n; j++) {
      fread(&v, sizeof(int16_t), 1, fl_bin);
      b[i][j] = v;
      // printf("%d ",b[i][j]);
    }
    // printf("\n");
  }
  printf("Arquivo lido\n");

  fclose(fl_bin);

  nt = atoi(argv[2]);

  pthread_t tid[nt];   // the thread identifier
  pthread_attr_t attr; // set of thread attributes
  arq *params = NULL;

  clock_t begin = clock(); // start clock

  // get the default attributes
  pthread_attr_init(&attr);

  // create the thread
  for (uint16_t i = 0; i < nt; i++) {
    params[i].i = i;
    params[i].n = n;
    params[i].nt = nt;
    params[i].a = (int16_t **)&a;
    params[i].b = (int16_t **)&b;
    params[i].m = (int **)&m;
    pthread_create(&tid[i], &attr, controlt, &params[i]);
  }

  // wait for the thread to exit
  for (uint16_t i = 0; i < nt; i++) {
    pthread_join(tid[i], NULL);
  }

  clock_t end = clock(); // end clock
  double te = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Tempo de execucao: %f seg\n", te);

  fw_bin = fopen("M.out", "wb");
  if (fw_bin == NULL) {
    perror("Erro ao criar o arquivo\n");
    return 0;
  }
  printf("vai\n");
  fwrite(&n, sizeof(uint16_t), 1, fw_bin);
  printf("se\n");
  for (uint16_t i = 0; i < n; i++) {
    for (uint16_t j = 0; j < n; j++) {
      fwrite(&m[i][j], sizeof(int), n, fw_bin);
    }
  }
  printf("fude\n");
  printf("Arquivo salvo\n");
  fclose(fw_bin);

  return 0;
}