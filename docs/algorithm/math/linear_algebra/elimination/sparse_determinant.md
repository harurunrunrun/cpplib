---
title: Sparse Determinant (疎行列の行列式) [sparse_matrix_det]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/sparse_determinant.hpp
---

```cpp
template<class T> struct SparseMatrixEntry{ int row, column; T value; };

template<class T, int MAX_SIZE = -1>
T sparse_determinant(int size, const std::vector<SparseMatrixEntry<T>>& entries,
                     std::uint64_t seed = std::random_device{}());
```

`sparse_determinant<T>(N, entries, seed)` は、`entries` で非零要素を与えた $N \times N$ の疎行列の行列式を求める。同じ位置の要素が重複する場合は値を加算する。ランダムな対角行列による前処理と Wiedemann 法の列、Berlekamp–Massey 法を使う。

`MAX_SIZE` を省略する（`-1`）と内部配列を実行時のサイズに合わせて確保する。`MAX_SIZE` を指定すると、Berlekamp–Massey 法の内部で容量 `MAX_SIZE` の固定長配列を使い、$N$ がそれを超えると例外になる。

```cpp
auto value = sparse_determinant<Mint>(n, entries, seed);
auto fixed_value = sparse_determinant<Mint, 100000>(n, entries, seed);
```

## 時間計算量

非零要素数を $E$ として $O(N(E+N))$ 時間。追加空間は $O(N)$（固定長版では $O(N+\texttt{MAX_SIZE})$）。

## 注意点

- `T` は素数体の型で、静的メンバ関数 `get_mod()` と四則演算を持つ必要がある。
- Monte Carlo 法であり、有限体上での退化による誤判定の確率が残る。最小多項式の次数が $N$ に満たないときは独立な前処理でもう 1 回だけ試し、それでも満たなければ $0$ を返す（正則行列を誤って $0$ と判定しうる）。
- 実行を再現したい場合は `seed` を指定する。
- $N < 0$ または $N >$ `MAX_SIZE` なら `std::length_error`、範囲外の要素位置があれば `std::out_of_range` を送出する。
- $0 \times 0$ 行列の行列式は $1$、非零要素が無い場合は $0$ を返す。
