---
title: Fast Matrix Power Entry Queries (高速行列累乗要素クエリ)
documentation_of: ../src/algorithm/math/linear_algebra/fast_matrix_power_entry_queries.hpp
---

固定された正方整数行列について、64 bit 非負整数乗の指定要素を繰り返し求める。
通常の `MatrixPowerEntryQueries` とは別に、前計算の密行列積へ Strassen 法を使う
大きい行列向けの API である。

## `FastMatrixPowerEntryQueries`

```cpp
class FastMatrixPowerEntryQueries;

explicit FastMatrixPowerEntryQueries(
    const vector<vector<int64_t>>& matrix,
    int64_t modulus = 1'000'000'007
);
```

`matrix^(2^bit)` を `0 <= bit < 64` について前計算する。
行列要素は `0..modulus-1` へ正規化される。負の要素も渡せる。
空の $0\times0$ 行列も構築できる。

## `entry`

```cpp
int64_t entry(size_t row, size_t column, uint64_t exponent);
```

`matrix^exponent` の `(row, column)` 要素を返す。
`exponent == 0` では単位行列の対応要素を返す。

## API別の時間計算量・空間計算量

行列の一辺を $D$、$\omega=\log_2 7$、
$p=\operatorname{popcount}(exponent)$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 構築 | $O(64D^\omega)$ | $O(64D^2)$ |
| `entry` (`exponent > 0`) | $O(D^2p)$ | $O(D)$ |
| `entry` (`exponent == 0`) | $O(1)$ | $O(1)$ |

最初に使うべき2冪の行を $O(D)$ で取り出し、残りの立っているビットごとに
行ベクトルと行列を $O(D^2)$ で掛ける。前計算の小さい部分行列は通常積へ切り替える。

## 注意点

- `modulus > 0` が必要であり、違反時は `std::invalid_argument` を送出する。
- 法は正の `int64_t` 全域に対応する。積は `unsigned __int128` で計算する。
- 行列は正方形でなければならない。shape 違反では `std::invalid_argument`、
  面積計算の overflow では `std::length_error` を送出する。
- 添字は0始まりである。範囲外では `std::out_of_range` を送出する。
  空行列では全ての `entry` 呼び出しが範囲外になる。
- `entry` は内部作業領域を再利用するため、同じオブジェクトへの同時呼び出しはできない。
- 64枚の行列を保持する。小さい行列や問い合わせが少ない場合は、通常版または
  `fast_matrix_power_mod` の方が省メモリである。
