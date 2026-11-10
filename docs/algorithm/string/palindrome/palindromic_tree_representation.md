---
title: Palindromic Tree Representation (回文木の関係表現) [eertree]
documentation_of: ../../../../src/algorithm/string/palindrome/palindromic_tree_representation.hpp
---

文字列の相異なる空でない回文と、各接頭辞の最長の回文接尾辞を、回文木のノード番号で表す。

## `PalindromicTreeRelation`

```cpp
struct PalindromicTreeRelation {
    int parent;
    int suffix_link;
};
```

`parent` は両端を 1 文字ずつ除いた回文、`suffix_link` は最長の真の回文接尾辞を
表すノード番号である。長さ 0 の根は `0`、長さ $-1$ の根は `-1` で表す。

## `PalindromicTreeRepresentation`

```cpp
struct PalindromicTreeRepresentation {
    vector<PalindromicTreeRelation> nodes;
    vector<int> longest_suffix;
};
```

- `nodes[i]` はノード `i + 1` の関係を持つ。ノードは対応する回文が初めて現れた順に番号付けされる。
- `longest_suffix[i]` は `text` の先頭 $i+1$ 文字（半開区間 `[0, i + 1)`）の最長の回文接尾辞のノード番号である。

## `palindromic_tree_representation`

```cpp
template<int MAX_LENGTH = -1, int ALPHABET = 26, char OFFSET = 'a'>
PalindromicTreeRepresentation palindromic_tree_representation(
    string_view text
)
```

`text` の関係表現を返す。空文字列では両方の `std::vector` が空になる。
`MAX_LENGTH` を省略（`-1`）すると、内部の回文木は可変長版になる。

```cpp
auto result = palindromic_tree_representation(text);
auto fixed = palindromic_tree_representation<100000>(text);
```

## 時間計算量

$N=\lvert \mathrm{text}\rvert$ とする。固定長版は固定ノード配列の初期化を含めて
$O(\texttt{ALPHABET}\cdot\texttt{MAX_LENGTH}+N)$（償却）、可変長版は $O(\texttt{ALPHABET}\cdot N)$ である。

## 空間計算量

固定長版は固定ノード配列と戻り値を含めて $O(\texttt{ALPHABET}\cdot\texttt{MAX_LENGTH}+N)$、可変長版は $O(\texttt{ALPHABET}\cdot N)$ である。
内部の回文木はヒープ上に確保する。

## 注意点

- `MAX_LENGTH` を指定した場合は `text.size() <= MAX_LENGTH` が必要である。
- 各文字 `c` は `OFFSET <= c < OFFSET + ALPHABET` を満たさなければならない。
- 容量または文字範囲の違反では `std::runtime_error` を送出する。
