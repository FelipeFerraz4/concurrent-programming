#include <iostream>
#include <string>
#include <chrono>
#include <omp.h>
using namespace std;
using namespace chrono;

// Função que aplica a cifra de César
string cifraCesar(string frase, int chave, bool paralelo = false) {
    string resultado = "";

    if (paralelo) {
        #pragma omp parallel for
        for (char c : frase) {
            if (isupper(c))
                resultado += char(int('A') + (int(c - 'A') + chave) % 26);
            else if (islower(c))
                resultado += char(int('a') + (int(c - 'a') + chave) % 26);
        }
    } else {
        for (char c : frase) {
            if (isupper(c))
                resultado += char(int('A') + (int(c - 'A') + chave) % 26);
            else if (islower(c))
                resultado += char(int('a') + (int(c - 'a') + chave) % 26);
        }
    }

    return resultado;
}

int main() {
    string frase, resultado, resultadoP;
    int chave;

    cout << "Digite a frase para criptografar: ";
    getline(cin, frase);

    cout << "Digite o deslocamento (chave): ";
    cin >> chave;


    // Medição do tempo - versão sequencial
    auto inicioSeq = high_resolution_clock::now();
    resultado = cifraCesar(frase, chave, false);
    auto fimSeq = high_resolution_clock::now();
    double tempoSeq = duration<double>(fimSeq - inicioSeq).count();


    // Medição do tempo - versão paralela
    auto inicioPar = high_resolution_clock::now();
    resultadoP = cifraCesar(frase, chave, true);
    auto fimPar = high_resolution_clock::now();
    double tempoPar = duration<double>(fimPar - inicioPar).count();

    // Speedup
    double speedup = tempoSeq / tempoPar;

    // Resultados
    cout << fixed;
    cout.precision(10);

    cout << "\n=== Resultados ===\n";
    cout << "Frase Criptografada (Sequencial): " << resultado << endl;
    cout << "Frase Criptografada (Paralelo): " << resultadoP << endl;
    cout << "Tempo Sequencial:    " << tempoSeq << " segundos\n";
    cout << "Tempo Paralelo:      " << tempoPar << " segundos\n";
    cout << "Speedup:             " << speedup << "\n";

    return 0;
}
