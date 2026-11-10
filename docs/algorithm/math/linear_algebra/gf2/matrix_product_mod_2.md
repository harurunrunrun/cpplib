---
title: Matrix Product (Mod 2) (二元体上の行列積) [matrix_product_mod_2]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/gf2/matrix_product_mod_2.hpp
---

$\mathbb{F}_2$ 上の行列積を、行を `std::bitset` で持って計算する。左辺の成分が 1 である行だけ右辺から xor するため、左辺が疎な場合に余分な xor を行わない。bitset の bit 番号を列番号として扱う。

## `matrix_product_mod_2`

```cpp
auto product = matrix_product_mod_2<MAX_INNER, MAX_COLUMNS>(
    lhs, rhs, inner_size, result_columns
);
```

`lhs` を $N\times M$ 行列、`rhs` を $M\times K$ 行列として、その積を返す。`inner_size` が $M$、`result_columns` が $K$ に対応する。戻り値は $N$ 行の `std::vector<std::bitset<MAX_COLUMNS>>` である。

`inner_size <= MAX_INNER`、`result_columns <= MAX_COLUMNS`、`rhs.size() == inner_size` でなければ `std::invalid_argument` を送出する。`lhs` の `inner_size` 以降の bit は参照しない。右辺の有効範囲外の bit は戻り値から除かれる。

```cpp
std::vector<std::vector<bool>> a{ {true, false}, {true, true} };
auto product = matrix_product_mod_2(a, a);   // 可変長版
```

容量のテンプレート引数を省略した可変長版は、入出力に二次元の `std::vector<bool>` を使う。`matrix_product_mod_2(lhs, rhs, inner_size, result_columns)` の形でも、サイズを省略して `rhs` の行数・先頭行の長さから決める形でも呼べる。形が合わなければ `std::invalid_argument` を送出する。

## 時間計算量・空間計算量

$Z$ を `lhs` の有効範囲にある 1 の個数、$w$ を機械語の bit 数、$L$ を `MAX_COLUMNS` とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `matrix_product_mod_2`（bitset 版） | $O(K+NM+(Z+N)\lceil L/w\rceil)$ | $O(N\lceil L/w\rceil)$ |
| `matrix_product_mod_2`（可変長版） | $O(NM\lceil K/64\rceil+NK+MK)$ | $O((N+M)\lceil K/64\rceil+NK)$ |

`MAX_COLUMNS` を $K$ に合わせたとき、bitset 版の最悪時間計算量は $O(NMK/w)$ となる。

## 注意点

- 結果の行数は `lhs.size()` で決まる。`lhs` の各行は少なくとも `inner_size` 列分の情報を持つ必要がある（可変長版では長さが足りなければ例外）。
