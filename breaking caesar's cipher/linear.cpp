#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

using namespace std;

// Cifra de César (versão linear)
string caesar_encrypt(const string& input, int shift) {
    string encrypted = input;
    for (char& c : encrypted) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base + shift + 26) % 26 + base;
        }
    }
    return encrypted;
}

// Retorna a letra mais frequente (em maiúsculo)
char most_frequent_letter(const string& text) {
    unordered_map<char, int> freq;
    for (char c : text) {
        if (isalpha(c)) {
            freq[toupper(c)]++;
        }
    }

    char most_freq = ' ';
    int max_count = 0;
    for (const auto& pair : freq) {
        if (pair.second > max_count) {
            most_freq = pair.first;
            max_count = pair.second;
        }
    }

    return most_freq;
}

// Lista de palavras comuns em português
vector<string> common_words = {
    "QUE", "DE", "O", "A", "E", "DO", "DA", "EM", "UM",
    "PARA", "COM", "NÃO", "É", "SE", "OS", "AS", "NA", "NO"
};

// Heurística de validação do texto
int score_text(const string& text) {
    int score = 0;

    // Palavras comuns
    for (const string& word : common_words) {
        if (text.find(word) != string::npos) {
            score += 10;
        }
    }

    // Duplas boas
    vector<string> good_doubles = {"SS", "RR", "OO"};
    for (const string& dbl : good_doubles) {
        if (text.find(dbl) != string::npos) {
            score += 5;
        }
    }

    // Duplas ruins (penalização)
    vector<string> bad_doubles = {"LL", "MM", "NN", "EE"};
    for (const string& dbl : bad_doubles) {
        if (text.find(dbl) != string::npos) {
            score -= 5;
        }
    }

    // Letra mais frequente
    char most_freq = most_frequent_letter(text);
    if (most_freq == 'A') score += 4;
    else if (most_freq == 'E') score += 2;
    else if (most_freq == 'O') score += 1;
    else score -= 3;

    return score;
}

// Testa todas as chaves e retorna a melhor
int brute_force_caesar_linear_freq(const string& encrypted) {
    int best_key = -1;
    int best_score = -1;

    for (int key = 0; key < 26; ++key) {
        string attempt = caesar_encrypt(encrypted, 26 - key);
        int score = score_text(attempt);
        if (score > best_score) {
            best_score = score;
            best_key = key;
        }
    }

    return best_key;
}
