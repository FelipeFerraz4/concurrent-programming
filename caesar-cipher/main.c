#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <omp.h>

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Função para criptografar usando a cifra de César
void cifra_cesar(char frase[], int chave, int sequencial) {
    if(sequencial){
        #pragma omp parallel for
        for (int i = 0; frase[i] != '\0'; i++) {
            if (isupper(frase[i])) {
                frase[i] = ((frase[i] - 'A' + chave) % 26) + 'A';
            } else if (islower(frase[i])) {
                frase[i] = ((frase[i] - 'a' + chave) % 26) + 'a';
            }
        }
    } else {
        for (int i = 0; frase[i] != '\0'; i++) {
            if (isupper(frase[i])) {
                frase[i] = ((frase[i] - 'A' + chave) % 26) + 'A';
            } else if (islower(frase[i])) {
                frase[i] = ((frase[i] - 'a' + chave) % 26) + 'a';
            }
        }
    }
}

int main() {
    char frase[1000000]; // Frase grande para testar
    char frase_copia[1000000]; // Cópia da frase para reutilizar
    int chave;

    // Solicita ao usuário que insira a frase
    printf("Digite a frase para criptografá-la usando a cifra de César: ");
    fgets(frase, sizeof(frase), stdin);
    frase[strcspn(frase, "\n")] = '\0'; // Remove o caractere de nova linha
    strcpy(frase_copia, frase); // Faz uma cópia da frase original

    // Solicita ao usuário que insira a chave de deslocamento
    printf("Digite o deslocamento para criptografá-la usando a cifra de César: ");
    scanf("%d", &chave);

    // Medição do tempo para a versão sequencial
    double inicio_seq = get_time();
    cifra_cesar(frase, chave, 0);
    double fim_seq = get_time();
    double tempo_seq = fim_seq - inicio_seq;

    // Restaura a frase original para a versão paralela
    strcpy(frase, frase_copia);

    // Medição do tempo para a versão paralela
    double inicio_par = get_time();
    cifra_cesar(frase, chave, 1);
    double fim_par = get_time();
    double tempo_par = fim_par - inicio_par;

    // Calcula o speedup
    double speedup = tempo_seq / tempo_par;

    // Exibe os resultados
    printf("\n=== Resultados ===\n");
    printf("Frase Criptografada: %s\n", frase);
    printf("Tempo Sequencial: %.10f segundos\n", tempo_seq);
    printf("Tempo Paralelo: %.10f segundos\n", tempo_par);
    printf("Speedup: %.2f\n", speedup);

    return 0;
}
