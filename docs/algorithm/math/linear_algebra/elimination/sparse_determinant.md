---
title: Sparse Determinant (疎行列の行列式) [sparse_matrix_det]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/sparse_determinant.hpp
---

`sparse_determinant<T, MAX_SIZE>(N, entries, seed)` は疎な正方行列の行列式を求める。要素は `SparseMatrixEntry<T>{row,column,value}`。重複する位置の値は加算される。ランダムな対角前処理とWiedemann列、Berlekamp–Massey法を使う。

## 時間計算量

非零要素数を $E$ として $O(N(E+N))$、追加空間 $O(N+\mathrm{MAX\_SIZE})$。

## 注意点

素数体の型 `T` は `get_mod()` と四則演算を持つこと。Monte Carlo法であり、有限体上の退化が原因の誤判定確率が残る。次数不足時は独立な前処理でもう一度試す。再現実行には `seed` を指定する。既定容量は8192、範囲外のサイズ・要素位置は例外。空行列は1。

## 容量を省略する可変長版

容量テンプレート引数をすべて省略すると、実行時サイズに合わせた `std::vector` を使う。容量を明示する固定長版も同じヘッダにある。

```cpp
auto value = sparse_determinant<Mint>(n, entries, seed);
auto fixed_value = sparse_determinant<Mint, 100000>(n, entries, seed);
```

非零要素数 $E$ に対し $O(N(E+N))$ 時間、$O(N+E)$ 空間。容量省略により内部の漸化式推定配列も実寸に合わせる。確率的判定の性質は同じ。
