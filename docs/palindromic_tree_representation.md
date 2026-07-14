---
title: Palindromic Tree Representation (回文木の関係表現) [eertree]
documentation_of: ../src/algorithm/string/palindromic_tree_representation.hpp
---

文字列の相異なる非空回文と、各prefixの最長回文suffixを回文木のnode番号で表す。

# `PalindromicTreeRelation`

```cpp
struct PalindromicTreeRelation {
    int parent;
    int suffix_link;
};
```

`parent` は両端を1文字ずつ除いた回文、`suffix_link` は最長の真の回文suffixを
表すnode番号である。長さ0のrootは `0`、長さ-1のrootは `-1` とする。

# `PalindromicTreeRepresentation`

```cpp
struct PalindromicTreeRepresentation {
    vector<PalindromicTreeRelation> nodes;
    vector<int> longest_suffix;
};
```

- `nodes[i]` はnode `i+1` の関係を持つ。nodeは対応回文が初めて現れた順である。
- `longest_suffix[i]` は `text[0, i+1)` の最長回文suffixのnode番号である。

# `palindromic_tree_representation`

```cpp
template<int MAX_LENGTH, int ALPHABET = 26, char OFFSET = 'a'>
PalindromicTreeRepresentation palindromic_tree_representation(
    string_view text
)
```

`text` の関係表現を返す。空文字列では両vectorが空になる。

## 時間計算量

固定node配列の初期化を含めて
$O(ALPHABET\cdot MAX\_LENGTH+|text|)$ 償却。

## 空間計算量

固定node配列と戻り値を含めて
$O(ALPHABET\cdot MAX\_LENGTH+|text|)$。固定node配列はheap上に確保する。

## 注意点

- `text.size() <= MAX_LENGTH` が必要である。
- 各文字 `c` は `OFFSET <= c < OFFSET + ALPHABET` を満たさなければならない。
- 容量または文字範囲の違反では `runtime_error` を送出する。
