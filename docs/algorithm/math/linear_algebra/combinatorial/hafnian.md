---
title: Hafnian (ハフニアン) [hafnian_of_matrix]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/combinatorial/hafnian.hpp
---

偶数次の対称行列 $A$ に対して、完全マッチングに対応する積の総和

$$
\operatorname{haf}(A)=\sum_M\prod_{\{i,j\}\in M}A_{i,j}
$$

を求める。ここで $M$ は頂点集合全体の完全マッチングを走る。対角成分は参照しない。

## 使い方

```cpp
using Mint = Modint998244353;
std::vector<std::vector<Mint>> values{ {0, 3}, {3, 0} };
auto from_vector = hafnian(values);
Matrix<Mint> matrix(values);
auto from_matrix = hafnian(matrix);
```

## `hafnian(vector)`

```cpp
T answer = hafnian(matrix);
```

`std::vector<std::vector<T>>` で与えた対称行列のハフニアンを返す。行列が正方行列でない場合、次数が奇数の場合、または対称でない場合は `std::invalid_argument` を送出する。$0\times0$ 行列に対しては `T(1)` を返す。

要素型 `T` には零元・単位元、加減算、乗算、等値比較が必要である（除算は使わない）。

## `hafnian(Matrix)`

```cpp
T answer = hafnian(matrix);
```

`Matrix<T, MAX_ROW, MAX_COL>` 用のオーバーロード。容量を省略した可変長の `Matrix<T>` も、容量を明示した固定長の `Matrix<T, R, C>` も渡せる。実際の行数と列数が一致しない場合は `std::invalid_argument` を送出する。内部で `std::vector` の行列にコピーしてから計算する。

## 時間計算量・空間計算量

$N$ を行列の次数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `hafnian(vector)` | $O(N^2 2^{N/2})$ | $O(N2^{N/2})$ |
| `hafnian(Matrix)` | $O(N^2 2^{N/2})$ | $O(N2^{N/2}+N^2)$ |

頂点を 2 個ずつ組にし、固定した組を含む 1 つの閉路（組を交互にたどる閉路）の重みを bit DP で求め、集合冪級数の指数関数を subset convolution で計算する。

## 注意点

- 要素型の演算でオーバーフローしないことは呼び出し側が保証する（剰余環 `Modint` などを想定する）。
- $N/2$ が `size_t` のビット幅以上になる場合は `std::length_error` を送出するが、実用上は $N$ が 40 程度までが現実的な範囲である。
