---
title: Matrix
documentation_of: ../src/structure/matrix/matrix.hpp
---

固定最大サイズの行列。

実際の行数・列数は実行時に持つ。保存領域は `MAX_ROW * MAX_COL` 個。

# テンプレート引数

```cpp
Matrix<T, MAX_ROW, MAX_COL>
```

- 要素型 `T`
- 行数の上限 `MAX_ROW`
- 列数の上限 `MAX_COL`

# コンストラクタ

```cpp
Matrix()
Matrix(int rows, int cols)
Matrix(vector<vector<T>> values)
```

## 時間計算量

- `Matrix()`: $O(MAX\_ROW MAX\_COL)$
- `Matrix(rows, cols)`: $O(MAX\_ROW MAX\_COL)$
- `Matrix(values)`: $O(MAX\_ROW MAX\_COL + rows \cdot cols)$

# 要素アクセス

```cpp
T& operator()(int i, int j)
const T& operator()(int i, int j)
```

## 時間計算量

- $O(1)$

# rows / cols

```cpp
int rows()
int cols()
bool empty()
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
```

## 時間計算量

- 加減算: $O(rows \cdot cols)$
- 行列積: $O(rows \cdot inner \cdot cols)$
- スカラー演算: $O(rows \cdot cols)$

# transposed

```cpp
A.transposed()
```

転置行列を返す。

## 時間計算量

- $O(rows \cdot cols)$
