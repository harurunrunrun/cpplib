---
title: Rope (ロープ)
documentation_of: ../../../src/structure/string/rope.hpp
---

implicit randomized balanced treeで可変stringを管理する。

## API

~~~cpp
Rope()
Rope(string_view text)
bool empty() const
size_t size() const
char at(size_t index) const
void insert(size_t position, string_view text)
void erase(size_t position, size_t length)
void set(size_t position, char value)
string substr(size_t position, size_t length)
string str() const
~~~

## 時間計算量

挿入文字数を $M$、出力文字数を $K$ とする。

- `empty`, `size`: $O(1)$
- `at`, `set`, `erase`: expected $O(\log(N+1))$
- `insert`: expected $O(M\log(N+M+1))$
- `substr`: expected $O(\log(N+1)+K)$
- `str`: $O(N)$

空間は $O(N)$。treapの偏りによるworst-caseは線形。

## 注意点

copy不可・move可能。position/length範囲外は `out_of_range`。
