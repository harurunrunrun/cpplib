---
title: Deque Palindromic Tree (両端回文木)
documentation_of: ../../../../src/algorithm/string/palindrome/deque_palindromic_tree.hpp
---

文字列の両端への追加・削除を行いながら、相異なる空でない回文部分文字列の個数、
最長の回文接頭辞、最長の回文接尾辞を管理する両端回文木（double-ended eertree）である。
surface recording 法で各位置に必要な回文だけを保持し、quick link で追加先を探す。

## テンプレート引数

```cpp
DequePalindromicTree<ALPHABET, MAX_NODES = -1, MAX_LENGTH = -1, OFFSET = 'a'>
```

- `ALPHABET`: アルファベットの文字数（正）。
- `MAX_NODES`: 実行中に作られる回文ノードの総数の上限（2 以上）。長さ $-1,0$ の根 2 個を含む。
- `MAX_LENGTH`: 同時に保持できる文字列長の上限（正）。
- `OFFSET`: アルファベットの先頭文字。

`MAX_NODES` と `MAX_LENGTH` のどちらかが `-1`（省略時）なら、実行時サイズに合わせて `std::vector` を伸ばす可変長版になる。

```cpp
DequePalindromicTree<26> dynamic;
DequePalindromicTree<26, 100002, 100000> fixed;
```

文字は連続した範囲 `[OFFSET, OFFSET + ALPHABET)` に属する必要がある。
削除済みノードの領域は再利用しないため、`MAX_NODES` は現在の相異なる回文の個数ではなく、
すべての `push_front` / `push_back` で新規に作られうるノード数に合わせる。
各 push で新規ノードは高々 1 個なので、push の回数が $Q$ 以下なら `MAX_NODES >= Q + 2` で十分である。

## 構築

```cpp
DequePalindromicTree<ALPHABET> tree;
```

空文字列を表す回文木を構築する。固定長版の配列はヒープ上に確保する。
コピーコンストラクタとコピー代入は使用できない。ムーブは $O(1)$ である。

## 更新

```cpp
void push_front(char c);
void push_back(char c);
void pop_front();
void pop_back();
```

- `push_front(c)`: 文字列の先頭に `c` を追加する。
- `push_back(c)`: 文字列の末尾に `c` を追加する。
- `pop_front()`: 先頭の 1 文字を削除する。
- `pop_back()`: 末尾の 1 文字を削除する。

## 長さ

```cpp
int size() const;
bool empty() const;
```

`size()` は現在の文字列長、`empty()` は現在の文字列が空かどうかを返す。

## 個別のクエリ

```cpp
int distinct_palindromes() const;
int longest_prefix_palindrome_length() const;
int longest_suffix_palindrome_length() const;
```

- `distinct_palindromes()`: 現在の文字列に含まれる相異なる空でない回文部分文字列の個数。
- `longest_prefix_palindrome_length()`: 最長の回文接頭辞の長さ。空文字列では 0。
- `longest_suffix_palindrome_length()`: 最長の回文接尾辞の長さ。空文字列では 0。

## 一括クエリ

```cpp
DequePalindromeStatistics statistics() const;
DequePalindromeStatistics query() const;
```

`statistics()` と `query()` は同じ結果を返す。戻り値は次のフィールドを持つ。

```cpp
struct DequePalindromeStatistics{
    int distinct_palindromes;
    int longest_prefix;
    int longest_suffix;
};
```

それぞれ相異なる空でない回文部分文字列の個数、最長の回文接頭辞の長さ、最長の回文接尾辞の長さである。

## 時間計算量

$N$ を更新直前の文字列長、$A$ を `ALPHABET` とする。quick link を用いるため、
追加先の探索は最悪 $O(\log N)$、新規ノードの遷移配列の初期化は $O(A)$ である。

| API | 時間計算量 |
| --- | --- |
| コンストラクタ | 固定長版 $O(\texttt{MAX_NODES}\cdot A+\texttt{MAX_LENGTH})$、可変長版 $O(A)$ |
| ムーブコンストラクタ / ムーブ代入 | $O(1)$ |
| `push_front(c)` / `push_back(c)` | 最悪 $O(\log(N+1)+A)$（可変長版では再確保を償却して含む） |
| `pop_front()` / `pop_back()` | 最悪 $O(1)$ |
| `size()` / `empty()` | $O(1)$ |
| `distinct_palindromes()` | $O(1)$ |
| `longest_prefix_palindrome_length()` | $O(1)$ |
| `longest_suffix_palindrome_length()` | $O(1)$ |
| `statistics()` / `query()` | $O(1)$ |

`ALPHABET` を定数とみなす場合、push は最悪 $O(\log(N+1))$ である。

## 空間計算量

固定長版は $O(\texttt{MAX_NODES}\cdot A+\texttt{MAX_LENGTH})$ で、更新中の動的確保は行わない。
可変長版は、作られたノード数 $S$、最大保持長 $L$ に対して $O(AS+L)$ である。

## 参考

- Qisheng Wang, Ming Yang, Xinrui Zhu,
  [Double-Ended Palindromic Trees in Linear Time](https://arxiv.org/abs/2210.02292)

## 注意点

- 範囲外の文字、`MAX_LENGTH` を超える追加、`MAX_NODES` を超えるノードの作成、空文字列からの pop では `std::runtime_error` を送出する。
