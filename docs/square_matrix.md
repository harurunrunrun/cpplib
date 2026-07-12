---
title: Square Matrix
documentation_of: ../src/structure/matrix/square_matrix.hpp
---

固定最大サイズの正方行列。

実際のサイズは実行時に持つ。保存領域は `MAX_SIZE * MAX_SIZE` 個。

# テンプレート引数

```cpp
SquareMatrix<T, MAX_SIZE>
```

- 要素型 `T`
- サイズの上限 `MAX_SIZE`

# コンストラクタ

```cpp
SquareMatrix()
SquareMatrix(int n)
SquareMatrix(vector<vector<T>> values)
```

## 時間計算量

- `SquareMatrix()`: $O(MAX\_SIZE^2)$
- `SquareMatrix(n)`: $O(MAX\_SIZE^2)$
- `SquareMatrix(values)`: $O(MAX\_SIZE^2 + n^2)$

# identity

```cpp
SquareMatrix::identity(int n)
SquareMatrix::zero(int n)
```

単位行列、零行列を返す。

## 時間計算量

- $O(MAX\_SIZE^2)$

# 要素アクセス

```cpp
T& operator()(int i, int j)
const T& operator()(int i, int j)
```

## 時間計算量

- $O(1)$

# 演算

```cpp
A + B
A - B
A * B
A * scalar
scalar * A
A / scalar
-A
A.pow(k)
```

## 時間計算量

- 加減算: $O(n^2)$
- 行列積: $O(n^3)$
- スカラー演算: $O(n^2)$
- `pow`: $O(n^3 \log k)$

# transposed

```cpp
A.transposed()
```

転置行列を返す。

## 時間計算量

- $O(n^2)$
