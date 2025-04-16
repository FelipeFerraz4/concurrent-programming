#include <iostream>
#include <string>
#include <chrono>
#include <omp.h> // Compilar com -fopenmp

using namespace std;
using namespace chrono;

// Função que aplica a cifra de César
void cifraCesar(string& frase, int chave, bool paralelo = false) {
    int tamanho = frase.size();

    if (paralelo) {
        #pragma omp parallel for
        for (int i = 0; i < tamanho; i++) {
            if (isupper(frase[i]))
                frase[i] = ((frase[i] - 'A' + chave) % 26) + 'A';
            else if (islower(frase[i]))
                frase[i] = ((frase[i] - 'a' + chave) % 26) + 'a';
        }
    } else {
        for (int i = 0; i < tamanho; i++) {
            if (isupper(frase[i]))
                frase[i] = ((frase[i] - 'A' + chave) % 26) + 'A';
            else if (islower(frase[i]))
                frase[i] = ((frase[i] - 'a' + chave) % 26) + 'a';
        }
    }
}

int main() {
    string frase;
    int chave;

    cout << "Digite a frase para criptografar: ";
    getline(cin, frase);

    cout << "Digite o deslocamento (chave): ";
    cin >> chave;

    string fraseCopia = frase;

    // Medição do tempo - versão sequencial
    auto inicioSeq = high_resolution_clock::now();
    cifraCesar(frase, chave, false);
    auto fimSeq = high_resolution_clock::now();
    double tempoSeq = duration<double>(fimSeq - inicioSeq).count();

    // Restaura a frase original
    frase = fraseCopia;

    // Medição do tempo - versão paralela
    auto inicioPar = high_resolution_clock::now();
    cifraCesar(frase, chave, true);
    auto fimPar = high_resolution_clock::now();
    double tempoPar = duration<double>(fimPar - inicioPar).count();

    // Speedup
    double speedup = tempoSeq / tempoPar;

    // Resultados
    cout << fixed;
    cout.precision(10);

    cout << "\n=== Resultados ===\n";
    cout << "Frase Criptografada: " << frase << endl;
    cout << "Tempo Sequencial:    " << tempoSeq << " segundos\n";
    cout << "Tempo Paralelo:      " << tempoPar << " segundos\n";
    cout << "Speedup:             " << speedup << "\n";

    return 0;
}
