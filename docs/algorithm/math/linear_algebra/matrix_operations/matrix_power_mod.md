---
title: Dynamic Matrix Power Modulo (動的行列の法付き累乗) [MPOW]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/matrix_operations/matrix_power_mod.hpp
---

実行時にサイズが決まる正方行列の冪を、指定した法の下で求める。

## `DynamicModMatrix`

```cpp
using DynamicModMatrix = vector<vector<int64_t>>;
```

行列の入出力型。

## `matrix_power_mod`

```cpp
DynamicModMatrix matrix_power_mod(
    DynamicModMatrix matrix,
    uint64_t exponent,
    int64_t modulus = 1'000'000'007
);
```

$\mathrm{matrix}^{\mathrm{exponent}} \bmod \mathrm{modulus}$ を返す。指数が 0 なら同じサイズの単位行列を返す。

## 時間計算量・空間計算量

行列の一辺を $D$、$e$ を `exponent` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `matrix_power_mod` | $O(D^3\log(e+1))$ | $O(D^2)$ |

## 注意点

- 行列は正方、`modulus > 0` でなければならない。不正な入力では `invalid_argument` を送出する。
- 負の要素も `0..modulus-1` に正規化する。積は `__int128` で計算するため、`modulus` は正の `int64_t` 全域を使える。
