#include <iostream>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>

class User {
public:
  std::string email;
  std::string password;

  User(const std::string& e, const std::string& p) : email(e), password(p) {}
};

std::vector<User> users;

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
  std::cout << "\033[2J\033[1;1H"; // ANSI escape code for clearing screen (Linux/macOS)
}

void print_header(const std::string& title) {
  std::cout << "==============================================" << std::endl;
  std::cout << "   " << title << std::endl;
  std::cout << "==============================================\n" << std::endl;
}

void wait_for_enter() {
  std::cout << "Press ENTER to continue...";
  std::cin.ignore();
  std::cin.get();
}

void register_user() {
  clear_screen();
  std::string email, password;

  print_header("User Registration");
  std::cout << "Enter your email: ";
  std::cin >> email;

  do {
    std::cout << "Enter your password (max 3 characters): ";
    disable_echo();
    std::cin >> password;
    enable_echo();
    std::cout << "\n";
  } while (password.length() > 4);

  users.emplace_back(email, password);
  clear_screen();
  std::cout << "[SUCCESS] User successfully registered!\n\n";
  wait_for_enter();
}

bool login_user(const std::string& input_email, const std::string& input_password) {
  for (const auto& user : users) {
    if (user.email == input_email && user.password == input_password) {
      return true;
    }
  }
  return false;
}

void session_loop(const std::string& email) {
  clear_screen();
  print_header("User Session");
  std::cout << "[INFO] Logged in as: " << email << "\n\n";
  wait_for_enter();
}

void standard_login() {
  clear_screen();
  std::string email, password;

  print_header("Login");
  std::cout << "Enter your email: ";
  std::cin >> email;

  std::cout << "Enter your password: ";
  disable_echo();
  std::cin >> password;
  enable_echo();
  std::cout << "\n";

  clear_screen();
  if (login_user(email, password)) {
    std::cout << "[SUCCESS] Login successful! Welcome!\n\n";
    session_loop(email);
  } else {
    std::cout << "[ERROR] Incorrect credentials. Please try again.\n\n";
  }
}

void brute_force_login() {
  // falta isso aqui
}

void login() {
  clear_screen();
  int option;

  print_header("Login Mode");
  std::cout << "1 - Standard Login\n";
  std::cout << "2 - Brute-force simulation\n\n";
  std::cout << "Your choice: ";
  std::cin >> option;

  switch (option) {
    case 1:
      standard_login();
      break;
    case 2:
      brute_force_login();
      break;
    default:
      std::cout << "[ERROR] Invalid option! Please try again.\n";
  }
}

void view_users() {
  clear_screen();
  print_header("Registered Users");

  if (users.empty()) {
    std::cout << "[INFO] No users registered.\n\n";
  } else {
    for (const auto& user : users) {
      std::cout << "Email: " << user.email << "\n";
    }
    std::cout << "\n";
  }

  wait_for_enter();
}

int main() {
  while (true) {
    int option;

    clear_screen();
    print_header("Welcome to the Registration/Login System");

    std::cout << "Choose an option:\n";
    std::cout << "  [1] Register\n";
    std::cout << "  [2] Login\n";
    std::cout << "  [3] View registered users\n";
    std::cout << "  [4] Exit\n\n";
    std::cout << "Your choice: ";
    std::cin >> option;

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
        std::cout << "[INFO] Exiting...\n";
        return 0;
      default:
        std::cout << "[ERROR] Invalid option! Please try again.\n";
    }
  }
  return 0;
}
