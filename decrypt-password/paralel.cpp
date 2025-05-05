#include <iostream>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <atomic>
#include <mutex>
#include <omp.h>  // OpenMP para paralelismo
#include <numeric> // Para usar accumulate

using namespace std;

int size_password = 3;
atomic<bool> password_found(false); // Flag para indicar se a senha foi encontrada
mutex output_mutex; // Mutex para sincronizar a saída no console

// Classe User representando um usuário com email e senha
class User {
public:
    string email;
    string password;

    User(const string& e, const string& p) : email(e), password(p) {}
};

vector<User> users;

// Função para desabilitar a exibição dos caracteres digitados
void disable_echo() {
    termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Função para habilitar a exibição dos caracteres digitados
void enable_echo() {
    termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Função para limpar a tela
void clear_screen() {
    cout << "\033[2J\033[1;1H"; 
}

// Função para exibir um cabeçalho com título
void print_header(const string& title) {
    cout << "==============================================\n";
    cout << "   " << title << "\n";
    cout << "==============================================\n";
}

// Função para aguardar o usuário pressionar ENTER
void wait_for_enter() {
    cout << "Press ENTER to continue...";
    cin.ignore();
    cin.get();
}

// Função para registrar um novo usuário
void register_user() {
    clear_screen();
    string email, password;

    print_header("User Registration");
    cout << "Enter your email: ";
    cin >> email;

    do {
        cout << "Enter your password (max " << size_password << " characters): ";
        disable_echo();
        cin >> password;
        enable_echo();
        cout << "\n";
    } while (password.length() > size_password);

    users.emplace_back(email, password);
    clear_screen();
    cout << "[SUCCESS] User successfully registered!\n\n";
    wait_for_enter();
}

// Função para fazer login de um usuário
bool login_user(const string& input_email, const string& input_password) {
    for (const auto& user : users) {
        if (user.email == input_email && user.password == input_password) {
            return true;
        }
    }
    return false;
}

// Função para tentar força bruta no login
void brute_force_login() {
    clear_screen();
    print_header("Brute-force Attack Simulation");

    if (users.empty()) {
        cout << "[ERROR] No users registered!\n\n";
        wait_for_enter();
        return;
    }

    string target_email;
    cout << "Enter the email to brute-force: ";
    cin >> target_email;

    string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    // Medindo o tempo de início fora do bloco paralelo
    auto start_time = chrono::steady_clock::now();

    // Variável de controle para sinalizar que a senha foi encontrada
    atomic<bool> local_password_found(false);

    int num_threads = 4;  // Número de threads a ser utilizado pelo OpenMP
    omp_set_num_threads(num_threads);  // Define o número de threads

    // Vetor para armazenar o número de tentativas feitas por cada thread (sem atomic ou critical)
    vector<int> thread_attempts(num_threads, 0);

    // Use um vetor local para tentativas em cada thread
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();  // Obtém o ID da thread atual
        string attempt(size_password, charset[0]);
        int local_thread_attempts = 0;  // Contador local de tentativas para a thread

        for (int length = 1; length <= size_password; ++length) {
            if (local_password_found) continue;

            for (int i = thread_id; i < charset.size(); i += num_threads) {
                attempt[0] = charset[i];

                // Loop para tentar todas as combinações
                while (true) {
                    if (local_password_found) break;  // Sair do loop se a senha for encontrada

                    local_thread_attempts++;  // Incrementa as tentativas dessa thread

                    if (login_user(target_email, attempt)) {
                        local_password_found = true;  // Atualiza a variável para sinalizar que a senha foi encontrada
                        break;  // Sair do loop interno para não continuar tentando
                    }

                    // Lógica para incrementar a senha (como um contador)
                    int k = length - 1;
                    while (k >= 0) {
                        if (attempt[k] != charset.back()) {
                            attempt[k] = charset[charset.find(attempt[k]) + 1];
                            break;
                        } else {
                            attempt[k] = charset[0];
                            k--;
                        }
                    }

                    if (k < 0) break;
                }

                if (local_password_found) break;
            }

            if (local_password_found) break;
        }

        // Armazena o número de tentativas da thread
        thread_attempts[thread_id] = local_thread_attempts;
    }

    // Medindo o tempo total após o bloco paralelo
    auto end_time = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    // Exibir o resultado final
    if (local_password_found) {
        cout << "\n[SUCCESS] Password found!\n";
        cout << "Email: " << target_email << "\n";
        cout << "Total Attempts: " << accumulate(thread_attempts.begin(), thread_attempts.end(), 0) << "\n";
        cout << "Time taken: " << duration.count() << " ms\n\n";

        cout << "Thread Statistics:\n";
        for (int i = 0; i < num_threads; ++i) {
            cout << "  Thread " << i + 1 << ": " << thread_attempts[i] << " attempts\n";
        }
        cout << "\n";
    } else {
        cout << "[FAILURE] Could not find password for " << target_email << "\n";
        cout << "Time taken: " << duration.count() << " ms\n\n";
    }

    wait_for_enter();
}

// Função para exibir o menu de login
void login() {
    clear_screen();
    int option;

    print_header("Login Mode");
    cout << "1 - Standard Login\n";
    cout << "2 - Brute-force simulation\n\n";
    cout << "Your choice: ";
    cin >> option;

    switch (option) {
        case 1:
            clear_screen();
            cout << "[INFO] Standard login not implemented in this version.\n";
            wait_for_enter();
            break;
        case 2:
            brute_force_login();
            break;
        default:
            cout << "[ERROR] Invalid option! Please try again.\n";
    }
}

// Função para exibir os usuários registrados
void view_users() {
    clear_screen();
    print_header("Registered Users");

    if (users.empty()) {
        cout << "[INFO] No users registered.\n\n";
    } else {
        for (const auto& user : users) {
            cout << "Email: " << user.email << "\n";
        }
        cout << "\n";
    }

    wait_for_enter();
}

// Função principal para controle do fluxo do programa
int main() {
    while (true) {
        int option;

        clear_screen();
        print_header("Welcome to the Registration/Login System");

        cout << "Choose an option:\n";
        cout << "  [1] Register\n";
        cout << "  [2] Login\n";
        cout << "  [3] View registered users\n";
        cout << "  [4] Exit\n\n";
        cout << "Your choice: ";
        cin >> option;

        switch (option) {
            case 1:
                register_user();
                break;
            case 2:
                login();
                break;
            case 3:
                view_users();
                break;
            case 4:
                cout << "[INFO] Exiting...\n";
                return 0;
            default:
                cout << "[ERROR] Invalid option! Please try again.\n";
        }
    }
    return 0;
}