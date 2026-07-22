#ifndef CPPLIB_SRC_STRUCTURE_OTHER_FASTIO_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_FASTIO_HPP_INCLUDED

#include <limits>
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
    bool negative = false;
    unsigned int value = 0;

    if (c == '-') {
      negative = true;
      c = getch();
    }

    while ('0' <= c && c <= '9') {
      value = value * 10U + static_cast<unsigned int>(c & 15);
      c = getch();
    }

    if (c != EOF) ungetch();

    if (negative && value == static_cast<unsigned int>(
        std::numeric_limits<int>::max()) + 1U) {
      x = std::numeric_limits<int>::min();
    } else {
      x = negative ? -static_cast<int>(value) : static_cast<int>(value);
    }
  }

  void readll(long long &x) {
    readspeoln();

    int c = getch();
    bool negative = false;
    unsigned long long value = 0;

    if (c == '-') {
      negative = true;
      c = getch();
    }

    while ('0' <= c && c <= '9') {
      value = value * 10ULL + static_cast<unsigned long long>(c & 15);
      c = getch();
    }

    if (c != EOF) ungetch();

    if (negative && value == static_cast<unsigned long long>(
        std::numeric_limits<long long>::max()) + 1ULL) {
      x = std::numeric_limits<long long>::min();
    } else {
      x = negative ? -static_cast<long long>(value) : static_cast<long long>(value);
    }
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

    unsigned int value;
    if (x < 0) {
      write('-');
      value = 0U - static_cast<unsigned int>(x);
    } else {
      value = static_cast<unsigned int>(x);
    }

    char s[20];
    int n = 0;

    while (value > 0) {
      s[n++] = char('0' + value % 10U);
      value /= 10U;
    }

    while (n--) write(s[n]);
  }

  void writell(long long x) {
    if (x == 0) {
      write('0');
      return;
    }

    unsigned long long value;
    if (x < 0) {
      write('-');
      value = 0ULL - static_cast<unsigned long long>(x);
    } else {
      value = static_cast<unsigned long long>(x);
    }

    char s[30];
    int n = 0;

    while (value > 0) {
      s[n++] = char('0' + value % 10ULL);
      value /= 10ULL;
    }

    while (n--) write(s[n]);
  }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_FASTIO_HPP_INCLUDED
