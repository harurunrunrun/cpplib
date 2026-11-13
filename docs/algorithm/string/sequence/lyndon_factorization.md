---
title: Lyndon Factorization (Lyndon分解) [lyndon_factorization]
documentation_of: ../../../../src/algorithm/string/sequence/lyndon_factorization.hpp
---

文字列・列を、辞書順で非増加となる Lyndon 語の列へ一意に分解する（Duval 法）。

## `lyndon_factorization(first, last, compare)`

```cpp
template<std::random_access_iterator Iterator, class Compare = std::less<>>
std::vector<std::size_t> lyndon_factorization(Iterator first, Iterator last, Compare compare = Compare{});
```

ランダムアクセスイテレータの半開範囲 `[first, last)` を、`compare` が定める辞書順で
Lyndon 分解する。返り値を $\lbrace a_0,a_1,\ldots,a_k\rbrace$ とすると、$a_0=0$、$a_k=N$ であり、各因子は
`[first + a_i, first + a_{i+1})` である。

同値判定には `!compare(a, b) && !compare(b, a)` を使う。空範囲の返り値は `{0}` である。

## `lyndon_factorization(text)`

```cpp
std::vector<std::size_t> boundaries = lyndon_factorization(
    std::string_view(text)
);
```

文字の通常の昇順を使う文字列用のオーバーロードである。返す境界の意味はイテレータ版と同じ。

## 時間計算量・空間計算量

$N$ を `last - first` または `text.size()`、$K$ を因子の個数とする。Duval 法では各要素を定数回だけ比較する。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| イテレータ版 | $O(N)$ | 返り値 $O(K)$ |
| `string_view` 版 | $O(N)$ | 返り値 $O(K)$ |

## 注意点

- `compare` は狭義弱順序でなければならない。
- `last < first` の反転した範囲では `std::invalid_argument` を送出する。
