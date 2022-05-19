// Linha de compilação utilizada: gcc -o main main.c -lpthread -Wall -Wextra -pedantic

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

uint16_t n;
uint16_t nt;
int16_t **a;
int16_t **b;
int **m;

void mult(uint16_t inicio, uint16_t fim) {
    uint16_t i, j, k;
    for (i = inicio; i < fim+1; i++) {
        for (j = 0; j < n; j++) {
            for (k = 0; k < n; k++){
                m[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void *controlt(void *param) {
    uint16_t i = *((uint16_t *)param);
    uint16_t inicio = i * (n / nt);
    uint16_t fim = (i == nt-1 ? n-1 : inicio+(n/nt)-1);

    printf("Thread %u executando... Multiplicando do indice %i até %i\n", i, inicio, fim);
    
    mult(inicio, fim);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: nome-arquivo num-thread\n");
        return 0;
    }

    FILE *fw_bin = NULL;
    FILE *fl_bin = NULL;

    fl_bin = fopen(argv[1], "rb");
    if (fl_bin == NULL) {
        printf("Arquivo nao encontrado\n");
        return 0;
    }

    uint16_t v;
    fread(&v, sizeof(uint16_t), 1, fl_bin);
    n = v;
    nt = atoi(argv[2]);
    if(nt>n){
        printf("num-thread maior que o devido\n");
        fclose(fl_bin);
        return 0;
    }
    
    a = (int16_t **)malloc(n * sizeof(int16_t *)); 
    b = (int16_t **)malloc(n * sizeof(int16_t *)); 
    m = (int **)malloc(n * sizeof(int *)); 

    for (uint16_t i = 0; i < n; ++i){
        a[i] = (int16_t *)malloc(n * sizeof(int16_t)); 
        b[i] = (int16_t *)malloc(n * sizeof(int16_t *)); 
        m[i] = (int *)malloc(n * sizeof(int *)); 
    }


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


    clock_t begin = clock(); // start clock

    pthread_t tid[nt];   // the thread identifier
    pthread_attr_t attr; // set of thread attributes
    uint16_t *params = NULL;

    params = (uint16_t *)malloc(sizeof(uint16_t) * nt);

    // get the default attributes
    pthread_attr_init(&attr);

    // create the thread
    for (uint16_t i = 0; i < nt; i++) {
        params[i] = i;
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
    fwrite(&n, sizeof(uint16_t), 1, fw_bin);
    for (uint16_t i = 0; i < n; i++) {
        for (uint16_t j = 0; j < n; j++) {
            printf("%d ",m[i][j]);
            fwrite(&m[i][j], sizeof(int), n, fw_bin);
        }
        printf("\n");
    }
    printf("Arquivo salvo\n");
    fclose(fw_bin);

    return 0;
}
