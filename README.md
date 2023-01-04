# Gomoku-NegaScout-AI
An AI playing Gomoku 15x15 using Negamax algorithm and NegaScout algorithm with time limitation and with depth limitation; Iterative Deepening; Zobrist Hashing; Null Window written in C++

In order to play, you have to do the following steps:
  - First, download the zip file and unzip it.
  - Second, move terminal to the direction of the file by "cd" command.
  - Third, run the program by typing into the terminal
  ```
  g++ -std=c++17 negascout_depth_limited.cpp -o gomokuAI
  ```
  or
  ```
  g++ -std=c++17 negascout_time_limited.cpp -o gomokuAI
  ```
  Then, type:
  ```
  ./gomokuAI
  ```

Note: You can switch algorithm, which by defaut is NegaScout, to NegaMax by changing the name of the function inside main().
