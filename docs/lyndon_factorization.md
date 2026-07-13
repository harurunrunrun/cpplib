---
title: Lyndon Factorization
documentation_of: ../src/algorithm/string/lyndon_factorization.hpp
---

文字列・列を辞書順で非増加となるLyndon word列へ一意に分解するDuval法。

## `lyndon_factorization(first, last, compare)`

```cpp
auto boundaries = lyndon_factorization(first, last, compare);
```

random access iteratorの半開範囲 `[first,last)` を、`compare` が定める辞書順で
Lyndon factorizationする。返り値を
`{a_0,a_1,...,a_k}` とすると、`a_0 == 0`、`a_k == N`であり、各factorは
`[first + a_i, first + a_{i+1})` である。

同値判定には`!compare(a,b) && !compare(b,a)`を使う。`compare`はstrict weak
orderingでなければならない。`last < first`の反転範囲は
`std::invalid_argument`を送出する。空範囲の返り値は`{0}`。

## `lyndon_factorization(text)`

```cpp
std::vector<std::size_t> boundaries = lyndon_factorization(
    std::string_view(text)
);
```

文字の通常の昇順を使う文字列用overload。返す境界の意味はiterator版と同じ。

## 計算量

`N = last - first`または`text.size()`とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| iterator版 | $O(N)$ | 返り値 $O(K)$ |
| `string_view`版 | $O(N)$ | 返り値 $O(K)$ |

`K`はfactor数。Duval法では各要素を定数回だけ比較する。
