#pragma once

#include <stdio.h>

#define read_size 1000000
#define write_size 1000000

struct fastio {
private:
  char read_data[read_size];
  int read_pos = 0;
  int read_len = 0;

  char write_data[write_size];
  int write_pos = 0;

  int getch() {
    if (read_pos == read_len) {
      read_len = fread(read_data, 1, read_size, stdin);
      read_pos = 0;
      if (read_len == 0) return EOF;
    }
    return read_data[read_pos++];
  }

  void ungetch() {
    if (read_pos > 0) read_pos--;
  }

  void readspeoln() {
    int c;
    while (true) {
      c = getch();
      if (c == EOF) return;
      if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
        ungetch();
        return;
      }
    }
  }

  void flush() {
    if (write_pos != 0) {
      fwrite(write_data, 1, write_pos, stdout);
      write_pos = 0;
    }
  }

public:
  fastio() {}

  ~fastio() {
    flush();
  }

  void readint(int &x) {
    readspeoln();

    int c = getch();
    int sign = 1;
    x = 0;

    if (c == '-') {
      sign = -1;
      c = getch();
    }

    while ('0' <= c && c <= '9') {
      x = x * 10 + (c & 15);
      c = getch();
    }

    if (c != EOF) ungetch();

    x *= sign;
  }

  void readll(long long &x) {
    readspeoln();

    int c = getch();
    long long sign = 1;
    x = 0;

    if (c == '-') {
      sign = -1;
      c = getch();
    }

    while ('0' <= c && c <= '9') {
      x = x * 10 + (c & 15);
      c = getch();
    }

    if (c != EOF) ungetch();

    x *= sign;
  }

  // [a, z]
  void readstr(char *s) {
    readspeoln();

    int c = getch();
    
    while (c != EOF && c != ' ' && c != '\n' && c != '\r' && c != '\t') {
      *s++ = (char)c;
      c = getch();
    }

    if (c != EOF) ungetch();

    *s = '\0';
  }

  void write(char c) {
    if (write_pos == write_size) flush();
    write_data[write_pos++] = c;
  }

  void write(const char *s) {
    while (*s) write(*s++);
  }

  void writeint(int x) {
    if (x == 0) {
      write('0');
      return;
    }

    if (x < 0) {
      write('-');
      x = -x;
    }

    char s[20];
    int n = 0;

    while (x > 0) {
      s[n++] = char('0' + x % 10);
      x /= 10;
    }

    while (n--) write(s[n]);
  }

  void writell(long long x) {
    if (x == 0) {
      write('0');
      return;
    }

    if (x < 0) {
      write('-');
      x = -x;
    }

    char s[30];
    int n = 0;

    while (x > 0) {
      s[n++] = char('0' + x % 10);
      x /= 10;
    }

    while (n--) write(s[n]);
  }
};