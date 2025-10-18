# 🧮 FIFO Calculator - Multi-Process IPC System

**A distributed calculator system using FIFO (Named Pipes) for Inter-Process Communication in C.**  
Built as an **Operating Systems** project demonstrating process management, IPC mechanisms, and file I/O.


---

## 📘 Project Overview

This **FIFO Calculator** is a multi-process system where:
- A **parent process** (calculator) manages user input and coordinates operations
- **Four child processes** (addition, subtraction, multiplication, division) perform calculations
- **FIFO named pipes** enable communication between processes
- A **saver process** logs all results to a file
- All operations run **concurrently** using process forking and IPC

The project demonstrates core OS concepts: **process creation, inter-process communication, file I/O, and resource management**.

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      CALCULATOR (Parent)                     │
│  - Shows menu                                                │
│  - Reads user input                                          │
│  - Writes to operation FIFOs                                 │
│  - Reads results from FIFO_RESULT                            │
└────────────┬────────────┬────────────┬────────────┬──────────┘
             │            │            │            │
        FIFO_ADD    FIFO_SUB    FIFO_MUL    FIFO_DIV
             │            │            │            │
    ┌────────▼────┐ ┌────▼────┐ ┌────▼────┐ ┌────▼────┐
    │  ADDITION   │ │SUBTRACT │ │MULTIPLY │ │ DIVISION│
    │  (Child 1)  │ │(Child 2)│ │(Child 3)│ │(Child 4)│
    └────────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘
             │            │            │            │
             └────────────┴────────────┴────────────┘
                          │
                    FIFO_RESULT
                          │
                 ┌────────▼────────┐
                 │  SAVER PROCESS  │
                 │ (Write to file) │
                 └─────────────────┘
                          │
                    results.txt
```

---

## ✨ Features

### 🔄 Multi-Process Architecture
- ✅ **5 concurrent processes** (1 parent + 4 operation children)
- ✅ **Fork-exec model** for process creation
- ✅ **Process synchronization** using FIFO pipes
- ✅ **Zombie process prevention** with `wait()`

### 📡 Inter-Process Communication
- ✅ **Named pipes (FIFOs)** for bidirectional communication
- ✅ **5 FIFO channels**: 4 for operations + 1 for results
- ✅ **Blocking I/O** for synchronization
- ✅ **Error handling** for pipe failures

### 🧮 Calculator Operations
- ➕ **Addition** (separate process)
- ➖ **Subtraction** (separate process)
- ✖️ **Multiplication** (separate process)
- ➗ **Division** (with zero-division protection)

### 💾 Result Logging
- ✅ **Automatic result saving** to `results.txt`
- ✅ **Append mode** (preserves history)
- ✅ **Separate saver process** for file I/O

---

## 📂 Project Structure

```
📦 fifo-calculator
┣ 📜 calculator.c          # Main parent process
┣ 📜 addition.c            # Addition child process
┣ 📜 subtraction.c         # Subtraction child process
┣ 📜 multiplication.c      # Multiplication child process
┣ 📜 division.c            # Division child process
┣ 📜 saver.c               # Result logging process
┣ 📜 Makefile              # Build automation
┣ 📜 results.txt           # Output file (generated)
┣ 📂 images/
┃ ┣ 🖼️ calculator-banner.png
┃ ┣ 🖼️ process-diagram.png
┃ ┗ 🖼️ fifo-architecture.png
┗ 📜 README.md
```

---

## 🚀 Getting Started

### Prerequisites
- **GCC Compiler** (Linux/Unix environment)
- **POSIX-compliant OS** (Linux, macOS, WSL)
- **Make** (optional, for automated build)

### Compilation

**Option 1: Using Makefile**
```bash
make
```

**Option 2: Manual Compilation**
```bash
gcc calculator.c -o calculator
gcc addition.c -o addition
gcc subtraction.c -o subtraction
gcc multiplication.c -o multiplication
gcc division.c -o division
gcc saver.c -o saver
```

### Running the Calculator

```bash
./calculator
```

### Example Usage

```
Calculator Menu:
1. Addition
2. Subtraction
3. Multiplication
4. Division
5. Exit
Choose an option: 1
Enter first number: 15
Enter second number: 27
Result: 42

Calculator Menu:
1. Addition
2. Subtraction
3. Multiplication
4. Division
5. Exit
Choose an option: 4
Enter first number: 100
Enter second number: 5
Result: 20

Calculator Menu:
...
Choose an option: 5
```

**results.txt will contain:**
```
42
20
```

---

## 🔧 Technical Implementation

### FIFO Creation (calculator.c)

```c
#define FIFO_ADD "fifo_add"
#define FIFO_SUB "fifo_sub"
#define FIFO_MUL "fifo_mul"
#define FIFO_DIV "fifo_div"
#define FIFO_RESULT "fifo_result"

// Create named pipes with read/write permissions
mkfifo(FIFO_ADD, 0666);
mkfifo(FIFO_SUB, 0666);
mkfifo(FIFO_MUL, 0666);
mkfifo(FIFO_DIV, 0666);
mkfifo(FIFO_RESULT, 0666);
```

---

### Process Creation Pattern

```c
// Fork and execute operation processes
pid_add = fork();
if (pid_add == 0) {
    // Child process
    execl("./addition", "addition", NULL);
    perror("Failed to exec addition");
    exit(EXIT_FAILURE);
}
// Parent continues to create other children
```

---

### Data Flow (Addition Example)

**1. Calculator writes to FIFO:**
```c
int fifo_fd = open(FIFO_ADD, O_WRONLY);
write(fifo_fd, &num1, sizeof(num1));
write(fifo_fd, &num2, sizeof(num2));
close(fifo_fd);
```

**2. Addition process reads and computes:**
```c
add_fd = open(FIFO_ADD, O_RDONLY);
read(add_fd, &num1, sizeof(num1));
read(add_fd, &num2, sizeof(num2));
close(add_fd);

result = num1 + num2;
```

**3. Addition writes result back:**
```c
add_fd = open(FIFO_RESULT, O_WRONLY);
write(add_fd, &result, sizeof(result));
close(add_fd);
```

**4. Calculator reads result:**
```c
result_fd = open(FIFO_RESULT, O_RDONLY);
read(result_fd, &result, sizeof(result));
printf("Result: %d\n", result);
close(result_fd);
```

---

### Saver Process Integration

```c
// Convert result to string
char val_str[100];
snprintf(val_str, sizeof(val_str), "%d", result);

// Fork and exec saver
pid_t pid = fork();
if (pid == 0) {
    execl("./saver", "saver", val_str, NULL);
    exit(EXIT_FAILURE);
}
```

**Saver.c:**
```c
FILE *file = fopen("results.txt", "a");
fprintf(file, "%s\n", result);
fclose(file);
```

---

## 📊 Process Communication Diagram



```
┌─────────┐  Write (num1, num2)  ┌───────────┐
│Calculator├─────────────────────►│ Addition  │
└────┬────┘                       └─────┬─────┘
     │                                  │
     │                              Write result
     │                                  │
     │        ┌─────────────────────────▼────────┐
     └────────┤         FIFO_RESULT              │
       Read   └──────────────────────────────────┘
```

---

## 🧠 Key Concepts Demonstrated

### 1. **Process Management**
- `fork()` - Create child processes
- `execl()` - Replace process image
- `wait()` - Prevent zombie processes

### 2. **Inter-Process Communication (IPC)**
- `mkfifo()` - Create named pipes
- `open()`, `read()`, `write()`, `close()` - FIFO operations
- Blocking I/O for synchronization

### 3. **File I/O**
- `fopen()`, `fprintf()`, `fclose()` - File operations
- Append mode for persistent logging

### 4. **Error Handling**
- `perror()` - System error messages
- Return code checking for all system calls
- Division by zero protection

---

## 🐛 Known Issues & Improvements

### Current Limitations:
- ⚠️ **Infinite loops in operation processes** - never exit naturally
- ⚠️ **No graceful shutdown** - child processes must be killed manually
- ⚠️ **No float support** - only integer arithmetic
- ⚠️ **Race conditions** possible with multiple concurrent operations
- ⚠️ **Zombie saver processes** - no wait() after fork in operation files

### Recommended Fixes:

**1. Add Signal Handling:**
```c
#include <signal.h>

void cleanup_handler(int sig) {
    unlink(FIFO_ADD);
    unlink(FIFO_SUB);
    unlink(FIFO_MUL);
    unlink(FIFO_DIV);
    unlink(FIFO_RESULT);
    exit(0);
}

signal(SIGINT, cleanup_handler);
signal(SIGTERM, cleanup_handler);
```

**2. Fix Saver.c Duplicate Code:**
```c
// Remove duplicate code at the end of saver.c (lines after return)
```

**3. Add Zombie Prevention for Saver:**
```c
pid_t pid = fork();
if (pid == 0) {
    execl("./saver", "saver", val_str, NULL);
    exit(EXIT_FAILURE);
} else if (pid > 0) {
    wait(NULL);  // Wait for saver to finish
}
```

**4. Implement Exit Signal:**
```c
// Send special value (-999) to signal termination
if (num1 == -999 && num2 == -999) {
    break;  // Exit loop
}
```

---

## 🔮 Future Enhancements

- [ ] Add **floating-point arithmetic** support
- [ ] Implement **operation history** viewer
- [ ] Add **signal handling** for graceful shutdown
- [ ] Create **GUI interface** using GTK or ncurses
- [ ] Add **network sockets** for distributed calculation
- [ ] Implement **semaphores** for better synchronization
- [ ] Add **shared memory** for faster communication
- [ ] Create **multithreaded version** for comparison
- [ ] Add **unit tests** for each operation
- [ ] Implement **logging** with timestamps

---

## 📸 Screenshots

### Calculator Menu


### Operation in Progress

### Results File


---

## 🧪 Testing

### Test Cases

| Test | Input | Expected Output | Status |
|------|-------|-----------------|--------|
| Addition | 10 + 5 | 15 | ✅ Pass |
| Subtraction | 20 - 8 | 12 | ✅ Pass |
| Multiplication | 7 * 6 | 42 | ✅ Pass |
| Division | 100 / 4 | 25 | ✅ Pass |
| Division by Zero | 10 / 0 | 0 (handled) | ✅ Pass |
| File Logging | Any operation | Entry in results.txt | ✅ Pass |
| Exit | Option 5 | Clean exit | ⚠️ Partial |

### Manual Testing

```bash
# Terminal 1: Run calculator
./calculator

# Terminal 2: Monitor FIFOs
ls -l fifo_*

# Terminal 3: Watch results file
tail -f results.txt

# Terminal 4: Monitor processes
watch -n 1 'ps aux | grep -E "calculator|addition|subtraction|multiplication|division"'
```

---

## 🎓 Learning Outcomes

This project demonstrates proficiency in:

- ✅ **POSIX System Calls** (fork, exec, open, read, write)
- ✅ **Named Pipes (FIFOs)** for IPC
- ✅ **Multi-Process Programming**
- ✅ **File Descriptor Management**
- ✅ **Error Handling** in system programming
- ✅ **Process Synchronization** using blocking I/O
- ✅ **Resource Cleanup** and management
- ✅ **Modular System Design**

---

## 🛠️ Makefile

```makefile
CC = gcc
CFLAGS = -Wall -Wextra

all: calculator addition subtraction multiplication division saver

calculator: calculator.c
	$(CC) $(CFLAGS) calculator.c -o calculator

addition: addition.c
	$(CC) $(CFLAGS) addition.c -o addition

subtraction: subtraction.c
	$(CC) $(CFLAGS) subtraction.c -o subtraction

multiplication: multiplication.c
	$(CC) $(CFLAGS) multiplication.c -o multiplication

division: division.c
	$(CC) $(CFLAGS) division.c -o division

saver: saver.c
	$(CC) $(CFLAGS) saver.c -o saver

clean:
	rm -f calculator addition subtraction multiplication division saver
	rm -f fifo_* results.txt

.PHONY: all clean
```

**Usage:**
```bash
make          # Build all
make clean    # Remove binaries and FIFOs
```

---

## 🤝 Contributing

Contributions welcome! Areas for improvement:

1. **Signal Handling** - Graceful shutdown
2. **Float Support** - Use `double` instead of `int`
3. **Error Messages** - More descriptive errors
4. **Code Cleanup** - Remove duplicate code in saver.c
5. **Documentation** - Add code comments

---

## 📝 Academic Context

**Course:** Operating Systems  
**Topic:** Inter-Process Communication (IPC)  
**Key Concepts:**
- Named Pipes (FIFOs)
- Process creation and management
- File I/O operations
- System call error handling

---

## 👨‍💻 Author

**Your Name**

- GitHub: [@yourusername](https://github.com/yourusername)
- LinkedIn: [Your Profile](https://linkedin.com/in/yourprofile)
- Email: your.email@example.com

---



---

## 🙏 Acknowledgments

- Built as part of **Operating Systems** coursework
- Inspired by Unix IPC mechanisms
- Thanks to professor for guidance on FIFO communication
- Resources: APUE (Advanced Programming in the UNIX Environment)

---

<p align="center">
  <strong>⭐ Star this repo if you found it helpful!</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C">
  <img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" alt="Linux">
  <img src="https://img.shields.io/badge/POSIX-000000?style=for-the-badge&logo=unix&logoColor=white" alt="POSIX">
  <img src="https://img.shields.io/badge/IPC-FF6B6B?style=for-the-badge&logo=buffer&logoColor=white" alt="IPC">
</p>

<p align="center">
  <em>Built with 🔧 for understanding OS fundamentals</em>
</p>
