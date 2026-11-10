---
title: Pfaffian (パフィアン) [pfaffian_of_matrix]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/combinatorial/pfaffian.hpp
---

偶数次の交代行列 $A$ の Pfaffian を、対称な行・列の交換を伴う Gauss の消去法で求める。$\operatorname{Pf}(A)^2=\det(A)$ を満たす符号付きの値を直接返す。

## 使い方

```cpp
using Mint = Modint998244353;
std::vector<std::vector<Mint>> values{ {0, 3}, {-3, 0} };
auto from_vector = pfaffian(values);
Matrix<Mint> matrix(values);
auto from_matrix = pfaffian(matrix);
```

## `pfaffian(vector)`

```cpp
T value = pfaffian(matrix);
```

`std::vector<std::vector<T>>` を値渡しで受け取る。入力は偶数次の正方行列で、対角成分が 0 かつ `a[j][i] == -a[i][j]` を満たす交代行列でなければならない。形または交代性が不正なら `std::invalid_argument` を送出する。$0\times0$ 行列の Pfaffian は 1 である。

`T` は体で、0 判定、四則演算、単項マイナス、`T(1)` を提供する必要がある。

## `pfaffian(Matrix)`

```cpp
T value = pfaffian(matrix);
```

`Matrix<T, MAX_ROW, MAX_COL>` 用のオーバーロード。容量を省略した可変長の `Matrix<T>` も、容量を明示した固定長の `Matrix<T, R, C>` も渡せる。実際の行数・列数の分を `std::vector` へコピーし、同じ検査と消去を行う。正方行列でなければ `std::invalid_argument` を送出する。

## 時間計算量・空間計算量

$N$ を行列の次数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `vector` 版 | $O(N^3)$ | $O(N^2)$ |
| `Matrix` 版 | $O(N^3)$ | $O(N^2)$ |

2 行・2 列のピボットごとに逆元を 1 回だけ計算する。入力は値渡しまたはコピーされるため、呼び出し元の行列は変更しない。

## 注意点

- `T` は体であることを前提とし、0 でないピボットの逆元が存在する必要がある（合成数を法とする剰余環では正しく動かない）。
