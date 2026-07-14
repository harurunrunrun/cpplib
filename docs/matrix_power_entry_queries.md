---
title: Matrix Power Entry Queries (行列累乗要素クエリ) [MATEX]
documentation_of: ../src/algorithm/math/matrix_power_entry_queries.hpp
---

固定された正方行列について、64 bit 非負整数乗の指定要素を繰り返し求める。

# `MatrixPowerEntryQueries`

```cpp
template<uint32_t Modulus = 1'000'000'007>
class MatrixPowerEntryQueries;
```

`Modulus` を法とする行列累乗要素クエリを処理する。

## 型

```cpp
using value_type = uint32_t;
using input_matrix_type = vector<vector<int64_t>>;
```

`value_type` は返り値の型、`input_matrix_type` は構築時の行列型である。

## `MatrixPowerEntryQueries`

```cpp
explicit MatrixPowerEntryQueries(const input_matrix_type& matrix);
```

`matrix^(2^b)` を `0 <= b < 64` について前計算する。行列要素は `0..Modulus-1` に正規化される。

## `entry`

```cpp
value_type entry(size_t row, size_t column, uint64_t exponent);
```

`matrix^exponent` の `(row, column)` 要素を返す。`exponent == 0` では単位行列の要素を返す。

## API別の時間計算量・空間計算量

行列サイズを `D` とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 構築 | $O(64D^3)$ | $O(64D^2)$ |
| `entry` | $O(D^2\operatorname{popcount}(exponent))$ | $O(D)$ |

## 注意点

- `Modulus > 0` でなければならない。
- 行列は空でない正方行列でなければならない。不正な行列では `invalid_argument` または `length_error` を送出する。
- 添字は0始まりである。範囲外の添字では `out_of_range` を送出する。
- `entry` は内部の作業領域を再利用するため、同じオブジェクトに対する同時呼び出しはできない。
