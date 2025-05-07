#include <iostream>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <chrono>

using namespace std;

int size_password = 4;

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

void session_loop(const string& email) {
  clear_screen();
  print_header("User Session");
  cout << "[INFO] Logged in as: " << email << "\n\n";
  wait_for_enter();
}

void standard_login() {
  clear_screen();
  string email, password;

  print_header("Login");
  cout << "Enter your email: ";
  cin >> email;

  cout << "Enter your password: ";
  disable_echo();
  cin >> password;
  enable_echo();
  cout << "\n";

  clear_screen();
  if (login_user(email, password)) {
    cout << "[SUCCESS] Login successful! Welcome!\n\n";
    session_loop(email);
  } else {
    cout << "[ERROR] Incorrect credentials. Please try again.\n\n";
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

  auto start = chrono::steady_clock::now();
  int attempts = 0;

  for (int length = 1; length <= size_password; ++length) {
    string attempt(length, charset[0]);

    while (true) {  
      attempts++;
      if (login_user(target_email, attempt)) {
        auto end = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "\n[SUCCESS] Password found!\n";
        cout << "Email: " << target_email << "\n";
        cout << "Password: " << attempt << "\n";
        cout << "Attempts: " << attempts << "\n";
        cout << "Time taken: " << duration.count() << " ms\n\n";
        wait_for_enter();
        return;
      }

      
      int i = length - 1;
      while (i >= 0) {
        if (attempt[i] != charset.back()) {
          attempt[i] = charset[charset.find(attempt[i]) + 1];
          break;
        } else {
          attempt[i] = charset[0];
          i--;
        }
      }

      if (i < 0) break; 
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
      standard_login();
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
