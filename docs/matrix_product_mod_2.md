---
title: Matrix Product (Mod 2) (二元体上の行列積) [matrix_product_mod_2]
documentation_of: ../src/algorithm/math/matrix_product_mod_2.hpp
---

$\mathbb{F}_2$上の行列積を、行を`std::bitset`で持って計算する。
左辺の成分が1である行だけ右辺からxorするため、左辺が疎な場合にも余分なxorを行わない。
bitsetのbit番号を列番号として扱う。

## `matrix_product_mod_2`

```cpp
auto product = matrix_product_mod_2<MAX_INNER, MAX_COLUMNS>(
    lhs, rhs, inner_size, result_columns
);
```

`lhs`を$N\times M$行列、`rhs`を$M\times K$行列として、その積を返す。
`inner_size`が$M$、`result_columns`が$K$に対応する。
戻り値は$N$行の`std::vector<std::bitset<MAX_COLUMNS>>`である。

`inner_size <= MAX_INNER`、`result_columns <= MAX_COLUMNS`、
`rhs.size() == inner_size`でなければ`std::invalid_argument`を送出する。
`lhs`の`inner_size`以降のbitは参照しない。右辺の有効範囲外のbitは戻り値から除かれる。

## API別の時間計算量・空間計算量

$Z$を`lhs`の有効範囲にある1の個数、$w$を機械語長、$L=\texttt{MAX_COLUMNS}$とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `matrix_product_mod_2` | $O(K+NM+(Z+N)\lceil L/w\rceil)$ | $O(N\lceil L/w\rceil)$ |

`MAX_COLUMNS`を$K$に合わせたとき、最悪時間計算量は$O(NMK/w)$となる。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
