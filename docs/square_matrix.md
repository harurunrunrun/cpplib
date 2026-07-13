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

以降、$C=MAX\_SIZE^2$ とする。固定長の保存領域を値初期化するため、
実際の行列が小さくても構築には $C$ 要素分かかる。

| 操作 | 時間計算量 |
| --- | --- |
| `SquareMatrix()` | $O(C)$ |
| `SquareMatrix(n)` | $O(C)$ |
| `SquareMatrix(values)` | $O(C+n^2)$ |
| コピー・move構築、コピー・move代入 | $O(C)$ |

# identity

```cpp
SquareMatrix::identity(int n)
SquareMatrix::zero(int n)
```

単位行列、零行列を返す。

## 時間計算量

| 操作 | 時間計算量 |
| --- | --- |
| `zero(n)` | $O(C)$ |
| `identity(n)` | $O(C+n)=O(C)$ |

# 要素アクセス

```cpp
T& operator()(int i, int j)
const T& operator()(int i, int j)
```

## 時間計算量

| 操作 | 時間計算量 |
| --- | --- |
| `operator()(i, j)` | $O(1)$ |

# size / empty / comparison

```cpp
A.size()
A.empty()
A == B
A != B
```

| 操作 | 時間計算量 |
| --- | --- |
| `size()`, `empty()` | $O(1)$ |
| `operator==`, `operator!=` | $O(n^2)$（不一致時は途中で終了） |

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

値を返す演算では、戻り値の固定長領域の初期化・コピーも含める。

| 操作 | 時間計算量 |
| --- | --- |
| `A += B`, `A -= B` | $O(n^2)$ |
| `A + B`, `A - B` | $O(C+n^2)=O(C)$ |
| `A *= scalar`, `A /= scalar` | $O(n^2)$ |
| `A * scalar`, `scalar * A`, `A / scalar` | $O(C+n^2)=O(C)$ |
| 単項 `+A` | $O(C)$ |
| 単項 `-A` | $O(C+n^2)=O(C)$ |
| `A *= B`, `A * B` | $O(C+n^3)$ |
| `A.pow(k)` | $O(C+(C+n^3)\log(k+1))$ |

# transposed

```cpp
A.transposed()
```

転置行列を返す。

## 時間計算量

| 操作 | 時間計算量 |
| --- | --- |
| `transposed()` | $O(C+n^2)=O(C)$ |

上表は `T` の構築・コピー・四則演算・比較を $O(1)$ とした計算量である。一般の `T` では、各項に実行する `T` の操作コストを掛ける。
