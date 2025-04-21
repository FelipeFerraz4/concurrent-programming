#include <iostream>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

using namespace std;

int size_password = 3;
atomic<bool> password_found(false); // Flag para indicar se a senha foi encontrada
mutex output_mutex; // Mutex para sincronizar a saída no console

class User {
public:
  string email;
  string password;

  User(const string& e, const string& p) : email(e), password(p) {}
};

vector<User> users;

void disable_echo() {
  termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void enable_echo() {
  termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag |= ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void clear_screen() {
  cout << "\033[2J\033[1;1H"; 
}

void print_header(const string& title) {
  cout << "==============================================" << endl;
  cout << "   " << title << endl;
  cout << "==============================================\n" << endl;
}

void wait_for_enter() {
  cout << "Press ENTER to continue...";
  cin.ignore();
  cin.get();
}

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

bool login_user(const string& input_email, const string& input_password) {
  for (const auto& user : users) {
    if (user.email == input_email && user.password == input_password) {
      return true;
    }
  }
  return false;
}

void brute_force_worker(const string& target_email, const string& charset, int length, int start, int step, atomic<int>& attempts, chrono::steady_clock::time_point start_time) {
  string attempt(length, charset[0]);

  for (int i = start; i < charset.size(); i += step) {
    attempt[0] = charset[i];
    while (true) {
      if (password_found) return;

      attempts++;
      if (login_user(target_email, attempt)) {
        password_found = true;
        auto end_time = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

        lock_guard<mutex> lock(output_mutex);
        cout << "\n[SUCCESS] Password found!\n";
        cout << "Email: " << target_email << "\n";
        cout << "Password: " << attempt << "\n";
        cout << "Attempts: " << attempts << "\n";
        cout << "Time taken: " << duration.count() << " ms\n\n";
        return;
      }

      int j = length - 1;
      while (j >= 0) {
        if (attempt[j] != charset.back()) {
          attempt[j] = charset[charset.find(attempt[j]) + 1];
          break;
        } else {
          attempt[j] = charset[0];
          j--;
        }
      }

      if (j < 0) break;
    }
  }
}

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

  auto start_time = chrono::steady_clock::now();
  atomic<int> total_attempts(0);

  vector<thread> threads;
  int num_threads = 4;
  vector<int> thread_attempts(num_threads, 0);

  for (int length = 1; length <= size_password; ++length) {
    password_found = false;

    for (int i = 0; i < num_threads; ++i) {
      threads.emplace_back([&, i]() {
        string attempt(length, charset[0]);
        for (int j = i; j < charset.size(); j += num_threads) {
          attempt[0] = charset[j];
          while (true) {
            if (password_found) return;

            thread_attempts[i]++;
            total_attempts++;
            if (login_user(target_email, attempt)) {
              password_found = true;
              auto end_time = chrono::steady_clock::now();
              auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

              lock_guard<mutex> lock(output_mutex);
              cout << "\n[SUCCESS] Password found!\n";
              cout << "Email: " << target_email << "\n";
              cout << "Password: " << attempt << "\n";
              cout << "Total Attempts: " << total_attempts << "\n";
              cout << "Time taken: " << duration.count() << " ms\n\n";
              return;
            }

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
        }
      });
    }

    for (auto& t : threads) {
      t.join();
    }

    threads.clear();

    if (password_found) {
      cout << "Thread Statistics:\n";
      for (int i = 0; i < num_threads; ++i) {
        cout << "  Thread " << i + 1 << ": " << thread_attempts[i] << " tasks\n";
      }
      cout << "\n";
      wait_for_enter();
      return;
    }
  }

  cout << "[FAILURE] Could not find password for " << target_email << "\n";
  wait_for_enter();
}

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
