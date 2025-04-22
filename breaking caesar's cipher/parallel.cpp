#include <omp.h>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <limits>
#include <iostream>

using namespace std;

#define NUM_THREADS 4

// Lista de palavras comuns em português
vector<string> common_words_parallel = {
    "QUE", "DE", "O", "A", "E", "DO", "DA", "EM", "UM",
    "PARA", "COM", "NÃO", "É", "SE", "OS", "AS", "NA", "NO"
};

// Duplas boas e ruins
vector<string> good_doubles = {"SS", "RR", "OO"};
vector<string> bad_doubles = {"LL", "MM", "NN", "EE"};

// Função para encontrar a letra mais frequente
char find_most_frequent_letter(const string& text) {
    unordered_map<char, int> freq_local;
    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel for
    for (int i = 0; i < text.size(); ++i) {
        char c = text[i];
        if (isalpha(c)) {
            char upper_c = toupper(c);
            #pragma omp critical
            freq_local[upper_c]++;
        }
    }

    // Encontra a letra mais frequente fora da região paralela
    char most_frequent = '\0';
    int max_count = 0;
    for (auto& pair : freq_local) {
        if (pair.second > max_count) {
            most_frequent = pair.first;
            max_count = pair.second;
        }
    }

    return most_frequent;
}

// Heurística com paralelismo + análise de frequência simplificada
int score_text_parallel(const string& text) {
    int score = 0;
    omp_set_num_threads(NUM_THREADS);

    // Palavras comuns
    #pragma omp parallel for reduction(+:score)
    for (int i = 0; i < common_words_parallel.size(); ++i) {
        if (text.find(common_words_parallel[i]) != string::npos) {
            score += 10;
        }
    }

    // Duplas boas
    #pragma omp parallel for reduction(+:score)
    for (int i = 0; i < good_doubles.size(); ++i) {
        if (text.find(good_doubles[i]) != string::npos) {
            score += 5;
        }
    }

    // Duplas ruins (penaliza)
    #pragma omp parallel for reduction(+:score)
    for (int i = 0; i < bad_doubles.size(); ++i) {
        if (text.find(bad_doubles[i]) != string::npos) {
            score -= 5;
        }
    }

    // Encontra a letra mais frequente
    char most_frequent = find_most_frequent_letter(text);

    // Atribui pontos com base na letra mais frequente
    if (most_frequent == 'A') {
        score += 4;
    } else if (most_frequent == 'E') {
        score += 2;
    } else if (most_frequent == 'O') {
        score += 1;
    }

    return score;
}

// Função para cifrar texto usando a cifra de César
string caesar_encrypt_parallel(const string& input, int shift) {
    string encrypted = input;
    int n = encrypted.size();

    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        char& c = encrypted[i];
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base + shift + 26) % 26 + base;
        }
    }

    return encrypted;
}

// Função de brute force com paralelismo para encontrar a chave
int brute_force_caesar_parallel_freq(const string& encrypted) {
    int best_key = -1;
    int best_score = numeric_limits<int>::min();

    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel
    {
        int local_best_score = numeric_limits<int>::min();
        int local_best_key = -1;

        #pragma omp for
        for (int key = 0; key < 26; ++key) {
            string attempt = caesar_encrypt_parallel(encrypted, 26 - key);
            int score = score_text_parallel(attempt);

            if (score > local_best_score) {
                local_best_score = score;
                local_best_key = key;
            }
        }

        #pragma omp critical
        {
            if (local_best_score > best_score) {
                best_score = local_best_score;
                best_key = local_best_key;
            }
        }
    }

    return best_key;
}
