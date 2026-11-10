---
title: Fast Determinant (高速行列式)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/fast_determinant.hpp
---

## 概要

体上の正方行列の行列式を、rank profile、ブロック Schur 補行列、Strassen 行列積で求める。先頭ブロックが正則になるよう列を決定的に並べ替えるため、先頭の主小行列が特異な行列にも使える。列置換の符号は結果に反映され、入力全体が特異なら `T()` を返す。

```cpp
using Mint = Modint998244353;
std::vector<std::vector<Mint>> values{ {1, 2}, {3, 4} };
auto from_vector = fast_determinant(values);
Matrix<Mint> matrix(values);
auto from_matrix = fast_determinant(matrix);
```

## 一次元配列

```cpp
template<class T>
T fast_determinant(
    const std::vector<T>& matrix,
    std::size_t size
);
```

行優先（row-major）で格納した `size x size` 行列の行列式を返す。`matrix.size() == size * size` を要求する。`size == 0` では空行列の行列式 `T(1)` を返す。

## 二次元配列

```cpp
template<class T>
T fast_determinant(const std::vector<std::vector<T>>& matrix);
```

各行の長さが `matrix.size()` である正方行列の行列式を返す。0 行の入力も許す。

## `Matrix`

```cpp
template<class T, int MAX_ROW, int MAX_COL>
T fast_determinant(const Matrix<T, MAX_ROW, MAX_COL>& matrix);
```

実際のサイズが正方である `Matrix` の行列式を返す。容量を省略した可変長の `Matrix<T>` も渡せる。容量 `MAX_ROW`, `MAX_COL` ではなく、`rows()` と `cols()` で表される実際のサイズだけを参照する。

## アルゴリズム

行列を上段 $k=\lfloor N/2\rfloor$ 行と下段に分ける。上段の ordered rank profile から列置換 $P$ を選び、

$$
AP=\begin{pmatrix}B&C\\D&E\end{pmatrix}
$$

の $B$ を正則にする。元の行列が正則なら任意の行の部分集合も一次独立なので、この $B$ は必ず存在する。次を再帰的に計算する。

$$
\det(A)=\operatorname{sgn}(P)\det(B)\det(E-DB^{-1}C)
$$

上段の rank が $k$ 未満なら、入力の行が一次従属なので直ちに 0 を返す。Schur 補行列の行列積とブロック逆行列には Strassen 法を使う。列置換は「新しい列 → 元の列」の置換として巡回分解し、$(-1)^{N-c}$（$c$ は巡回の個数）を掛ける。

## 時間計算量

$N$ を次数、$\omega=\log_2 7$ とし、`T` の各演算を $O(1)$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 一次元配列版 | $O(N^\omega)$ | $O(N^2)$ |
| 二次元配列版 | $O(N^\omega)$ | $O(N^2)$ |
| `Matrix` 版 | $O(N^\omega)$ | $O(N^2)$ |

$N \le 32$ では定数倍を小さくするため Gauss の消去法に切り替える。この閾値は漸近計算量を変えない。

## 注意点

- `T{}` が 0、`T(1)` が 1 を表し、加減乗除、単項マイナス、等値比較ができる正確な体を要求する。
- 浮動小数点数は丸め誤差のため、0 判定と rank 判定の前提を満たさない。実数近似の行列式には使わない。
- 一次元配列の要素数の不一致、二次元配列または `Matrix` の非正方の入力では `invalid_argument` を送出する。
- `size * size` が `size_t` に収まらない場合は `length_error` を送出する。
