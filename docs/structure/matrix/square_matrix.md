---
title: Square Matrix (正方行列) [pow_of_matrix]
documentation_of: ../../../src/structure/matrix/square_matrix.hpp
---

正方行列。実際の大きさ $n$ は実行時に持つ。固定長版の保存領域は `MAX_SIZE * MAX_SIZE` 個である。

## テンプレート引数

```cpp
SquareMatrix<T, MAX_SIZE = -1>
```

- `T`: 要素の型
- `MAX_SIZE`: 大きさの上限。省略（`-1`）すると、実行時の大きさに合わせて `std::vector` を確保する可変長版になる

```cpp
SquareMatrix<long long> matrix(n);       // 可変長版
SquareMatrix<long long, 100> fixed(n);   // 固定長版
```

## コンストラクタ

```cpp
SquareMatrix()
explicit SquareMatrix(int n)
explicit SquareMatrix(const vector<vector<T>>& values)
```

`SquareMatrix()` は $0 \times 0$ の行列、`SquareMatrix(n)` は全要素が `T{}` の $n \times n$ 行列を作る。

## identity / zero

```cpp
static SquareMatrix identity(int n)
static SquareMatrix zero(int n)
```

単位行列、零行列を返す。

## 要素アクセス

```cpp
T& operator()(int i, int j)
const T& operator()(int i, int j) const
```

## size / empty / 比較

```cpp
int size() const
bool empty() const
A == B
A != B
```

## 演算

```cpp
A + B
A - B
A * B
A * scalar
scalar * A
A / scalar
-A
A += B, A -= B, A *= B, A *= scalar, A /= scalar
A.pow(k)
```

値を返す演算は新しい `SquareMatrix` を返し、複合代入演算は `*this` への参照を返す。`pow(k)` は $A^k$ を返す（$k = 0$ では同じ大きさの単位行列）。

## transposed

```cpp
SquareMatrix transposed() const
```

転置行列を返す。

## 時間計算量

$C=\texttt{MAX_SIZE}^2$ とする。固定長版は保存領域を値初期化するため、実際の行列が小さくても構築に $C$ 要素分かかる。可変長版では、以下の $C$ は $n^2$ に置き換わる。
値を返す演算では、戻り値の固定長領域の初期化・コピーも含める。

| 操作 | 時間計算量 |
| --- | --- |
| `SquareMatrix()` | $O(C)$ |
| `SquareMatrix(n)` | $O(C)$ |
| `SquareMatrix(values)` | $O(C+n^2)$ |
| コピー・ムーブ構築、コピー・ムーブ代入 | $O(C)$ |
| `zero(n)` | $O(C)$ |
| `identity(n)` | $O(C+n)=O(C)$ |
| `operator()(i, j)`、`size()`、`empty()` | $O(1)$ |
| `operator==`、`operator!=` | $O(n^2)$（不一致なら途中で終了） |
| `A += B`、`A -= B` | $O(n^2)$ |
| `A + B`、`A - B` | $O(C+n^2)=O(C)$ |
| `A *= scalar`、`A /= scalar` | $O(n^2)$ |
| `A * scalar`、`scalar * A`、`A / scalar` | $O(C+n^2)=O(C)$ |
| 単項 `+A` | $O(C)$ |
| 単項 `-A` | $O(C+n^2)=O(C)$ |
| `A *= B`、`A * B` | $O(C+n^3)$ |
| `A.pow(k)` | $O(C+(C+n^3)\log(k+1))$ |
| `transposed()` | $O(C+n^2)=O(C)$ |

上の表は `T` の構築・コピー・四則演算・比較を $O(1)$ とした計算量である。一般の `T` では、各項に `T` の操作のコストを掛ける。

## 空間計算量

$C=\texttt{MAX_SIZE}^2$（可変長版では $n^2$）とする。

| API | 空間計算量（保存領域・追加領域） |
| --- | --- |
| 各コンストラクタ、コピー・ムーブ | 戻り値またはオブジェクトに $O(C)$ |
| `size`、`empty`、`operator()`、比較 | $O(1)$ |
| `+=`、`-=`、スカラーの `*=`、`/=` | $O(1)$ |
| 値を返す演算、行列の `*=`、`zero`、`identity`、`transposed` | 一時オブジェクトまたは戻り値に $O(C)$ |
| `pow(k)` | 累積用と底に $O(C)$ |

## 注意点

- コンストラクタ、`zero`、`identity` は $0 \le n \le \texttt{MAX_SIZE}$ を要求する。
- `vector` の入力は $n \times n$ でなければならない。大きさや添字の違反は `std::runtime_error` を送出する。
- 行列どうしの加減算・乗算は同じ実行時の大きさを要求し、違反すると `std::runtime_error` を送出する。
- `pow(k)` は非負の指数を要求し、負なら `std::runtime_error` を送出する。
- スカラーによる除算の可否と、$0$ による除算の扱いは `T::operator/` に従う。
