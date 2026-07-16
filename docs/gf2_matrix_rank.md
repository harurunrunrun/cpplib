---
title: GF2 Matrix Rank (二元体行列の階数) [matrix_rank_mod_2]
documentation_of: ../src/algorithm/math/gf2_matrix_rank.hpp
---

`0` と `1` の文字列で与えた行列の $\mathbb{F}_2$ 上の階数を求める。

## `gf2_matrix_rank`

```cpp
size_t gf2_matrix_rank(
    const vector<string>& matrix,
    size_t column_count
)
```

`matrix` を行列の各行として、列数を明示して階数を返す。行数が0の場合も
`column_count` を保持できる。

```cpp
size_t gf2_matrix_rank(const vector<string>& matrix)
```

列数を先頭行の長さから求める。行数が0なら列数0として扱う。

## 時間計算量

行数を $R$、列数を $C$、階数を $K$ として
$O(RCK/64+RC)$。

## 空間計算量

bit-packed行列を含めて $O(RC/64+R)$。

## 注意点

- 全行の長さは指定列数と等しく、各文字は `0` または `1` でなければならない。
- 行長または文字の前提違反では `invalid_argument` を送出する。
- 入力行列は変更しない。
