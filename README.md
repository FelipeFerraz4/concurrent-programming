# 🔐 Password Decryptor – Sequential vs Parallel (OpenMP)

This project is part of a university assignment for the **Concurrent Programming** course. It implements and compares two versions of a brute-force password decryptor: a **sequential version** and a **parallel version** using **OpenMP**.

The objective is to evaluate how parallelism (via OpenMP) affects performance in a CPU-intensive task such as password cracking.

The program attempts all possible combinations of characters up to a certain length using brute-force, comparing each attempt to a predefined hashed or plain password.

---

## ⚙️ Requirements

- C++ compiler with OpenMP support (e.g., `g++`, `clang++`, or MSVC)
- C++11 standard or newer
- Unix-like system or Windows with OpenMP support

---

## 🚀 How to Run the Code

Navigate to the `decrypt-password` folder in your terminal, then:

### 🔸 Sequential Version

```bash
cd decrypt-password/sequential
g++ -o decryptor main.cpp
./decryptor
```

```bash
cd decrypt-password/parallel
g++ -fopenmp -o decryptor main.cpp
./decryptor
```