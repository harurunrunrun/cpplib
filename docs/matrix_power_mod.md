---
title: Dynamic Matrix Power Modulo (動的行列の法付き累乗) [MPOW]
documentation_of: ../src/algorithm/math/matrix_power_mod.hpp
---

実行時サイズの正方行列を、指定した法の下で整数乗する。

# `DynamicModMatrix`

```cpp
using DynamicModMatrix = vector<vector<int64_t>>;
```

行列の入出力型。

# `matrix_power_mod`

```cpp
DynamicModMatrix matrix_power_mod(
    DynamicModMatrix matrix,
    uint64_t exponent,
    int64_t modulus = 1'000'000'007
);
```

`matrix^exponent mod modulus` を返す。指数0では同じサイズの単位行列を返す。

## API別の時間計算量・空間計算量

行列サイズを `D` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `matrix_power_mod` | $O(D^3\log exponent)$ | $O(D^2)$ |

## 注意点

- 行列は正方形、`modulus > 0` でなければならない。不正な入力では `invalid_argument` を送出する。
- 負の要素も `0..modulus-1` に正規化する。
