---
title: Deque Palindromic Tree
documentation_of: ../src/algorithm/string/deque_palindromic_tree.hpp
---

文字列の両端への追加・削除を行いながら、異なる非空回文部分文字列の個数、
最長回文prefix、最長回文suffixを管理する両端回文木（double-ended eertree）。
surface recording法で各位置の必要な回文だけを保持し、quick linkで追加先を探す。

# テンプレート引数

```cpp
DequePalindromicTree<ALPHABET, MAX_NODES, MAX_LENGTH, OFFSET>
```

- `ALPHABET`: alphabetの文字数。
- `MAX_NODES`: 実行中に作られる回文nodeの総数の上限。長さ $-1,0$ のroot 2個を含む。
- `MAX_LENGTH`: 同時に保持できる文字列長の上限。
- `OFFSET`: alphabetの先頭文字。既定値は `'a'`。

文字は連続した範囲 `[OFFSET, OFFSET + ALPHABET)` に属する必要がある。
削除済みnodeの領域は再利用しないため、`MAX_NODES` は現在の異なる回文数ではなく、
全 `push_front` / `push_back` で新規作成され得るnode数に合わせる。
各pushで新規nodeは高々1個なので、push回数が $Q$ 以下なら `MAX_NODES >= Q+2` で十分。

# 構築

```cpp
DequePalindromicTree tree;
```

空文字列を表す回文木を構築する。固定長配列はheapに確保される。
copy constructor / copy assignmentは使用できない。moveは $O(1)$。

# 更新

```cpp
void push_front(char c);
void push_back(char c);
void pop_front();
void pop_back();
```

- `push_front(c)`: 文字列の先頭へ `c` を追加する。
- `push_back(c)`: 文字列の末尾へ `c` を追加する。
- `pop_front()`: 先頭の1文字を削除する。
- `pop_back()`: 末尾の1文字を削除する。

alphabet外の文字、`MAX_LENGTH` を超える追加、`MAX_NODES` を超えるnode作成、
空文字列からのpopでは `runtime_error` を送出する。

# 長さ

```cpp
int size() const;
bool empty() const;
```

`size()` は現在の文字列長、`empty()` は現在の文字列が空かを返す。

# 個別query

```cpp
int distinct_palindromes() const;
int longest_prefix_palindrome_length() const;
int longest_suffix_palindrome_length() const;
```

- `distinct_palindromes()`: 現在の文字列に含まれる異なる非空回文部分文字列の個数。
- `longest_prefix_palindrome_length()`: 最長回文prefixの長さ。空文字列では0。
- `longest_suffix_palindrome_length()`: 最長回文suffixの長さ。空文字列では0。

# 一括query

```cpp
DequePalindromeStatistics statistics() const;
DequePalindromeStatistics query() const;
```

`statistics()` と `query()` は同じ結果を返す。戻り値は次のfieldを持つ。

```cpp
struct DequePalindromeStatistics{
    int distinct_palindromes;
    int longest_prefix;
    int longest_suffix;
};
```

それぞれ異なる非空回文部分文字列数、最長回文prefix長、最長回文suffix長である。

# 時間計算量

$N$ を更新直前の文字列長、$A=ALPHABET$ とする。quick linkを用いるため、
追加先探索は最悪 $O(\log N)$、新規nodeの遷移配列初期化は $O(A)$ である。

| API | 時間計算量 |
| --- | --- |
| constructor | $O(MAX\_NODES\cdot A+MAX\_LENGTH)$ |
| move constructor / move assignment | $O(1)$ |
| `push_front(c)` / `push_back(c)` | 最悪 $O(\log(N+1)+A)$ |
| `pop_front()` / `pop_back()` | 最悪 $O(1)$ |
| `size()` / `empty()` | $O(1)$ |
| `distinct_palindromes()` | $O(1)$ |
| `longest_prefix_palindrome_length()` | $O(1)$ |
| `longest_suffix_palindrome_length()` | $O(1)$ |
| `statistics()` / `query()` | $O(1)$ |

`ALPHABET` を固定定数とみなす場合、pushは最悪 $O(\log(N+1))$。
保持領域は $O(MAX\_NODES\cdot A+MAX\_LENGTH)$ で、更新中の動的確保は行わない。

# 参考

- [Library Checker: Palindromes in a Deque](https://judge.yosupo.jp/problem/palindromes_in_deque)
- Qisheng Wang, Ming Yang, Xinrui Zhu,
  [Double-Ended Palindromic Trees in Linear Time](https://arxiv.org/abs/2210.02292)
