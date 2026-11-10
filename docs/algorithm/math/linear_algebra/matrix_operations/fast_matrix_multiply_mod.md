---
title: Fast Matrix Multiplication Modulo (高速法付き行列積)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/matrix_operations/fast_matrix_multiply_mod.hpp
---

実行時に与える正の法に対する密な整数行列積を Strassen 法で計算する。小さい行列と細長い行列では、演算量を比較して通常の三重ループに切り替える。

## `fast_matrix_multiply_mod`

```cpp
vector<int64_t> fast_matrix_multiply_mod(
    const vector<int64_t>& left,
    size_t left_rows,
    size_t inner_size,
    const vector<int64_t>& right,
    size_t right_columns,
    int64_t modulus = 1'000'000'007
);
```

`left` を `left_rows` 行 `inner_size` 列、`right` を `inner_size` 行 `right_columns` 列の行優先配列として、積を行優先配列で返す。各入力要素を `0..modulus-1` に正規化するため、負の要素も渡せる。

## 時間計算量・空間計算量

$r$ を `left_rows`、$k$ を `inner_size`、$c$ を `right_columns`、$D=\max(r,k,c)$、$\omega=\log_2 7$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `fast_matrix_multiply_mod` | $O(D^\omega)$ | $O(D^2)$ |

入力の正規化と結果の変換に $O(rk+kc+rc)$ かかる。Strassen 法に進む場合は各辺を 2 の冪まで 0 で埋めるが、その大きさは $2D$ 未満である。

## 注意点

- `modulus > 0` が必要であり、違反時は `std::invalid_argument` を送出する。
- 法は正の `int64_t` 全域に対応する。積は `unsigned __int128` で計算し、Strassen 法の加減算も毎回法で正規化する。
- 配列の長さが指定した形と一致しない場合は `std::invalid_argument`、形の面積の計算がオーバーフローする場合は `std::length_error` を送出する。
- 0 行・0 列を含む形にも対応する。
- 小さい行列では通常の三重ループの行列積の方が速いことがある。
