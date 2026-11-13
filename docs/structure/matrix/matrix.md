---
title: Matrix (行列) [matrix_product]
documentation_of: ../../../src/structure/matrix/matrix.hpp
---

行列。実際の行数・列数は実行時に持つ。固定長版の保存領域は `MAX_ROW * MAX_COL` 個である。

## テンプレート引数

```cpp
Matrix<T, MAX_ROW = -1, MAX_COL = -1>
```

- `T`: 要素の型
- `MAX_ROW`: 行数の上限
- `MAX_COL`: 列数の上限

上限を省略（`-1`）すると、実行時の大きさに合わせて `std::vector` を確保する可変長版になる。固定長版との積も計算できる。

```cpp
Matrix<long long> matrix(rows, cols);         // 可変長版
Matrix<long long, 100, 100> fixed(rows, cols); // 固定長版
```

## コンストラクタ

```cpp
Matrix()
Matrix(int rows, int cols)
explicit Matrix(const vector<vector<T>>& values)
```

`Matrix()` は $0 \times 0$ の行列、`Matrix(rows, cols)` は全要素が `T{}` の行列を作る。

## 要素アクセス

```cpp
T& operator()(int i, int j)
const T& operator()(int i, int j) const
```

## rows / cols / empty

```cpp
int rows() const
int cols() const
bool empty() const
```

行数、列数と、行数または列数が $0$ かどうかを返す。

## zero / 比較

```cpp
static Matrix zero(int rows, int cols)
A == B
A != B
```

`zero` は零行列を返す。

## 演算

```cpp
A + B
A - B
A * B
A * scalar
scalar * A
A / scalar
-A
A += B, A -= B, A *= scalar, A /= scalar
```

値を返す演算は新しい `Matrix` を返し、複合代入演算は `*this` への参照を返す。
行列積 `A * B` の結果の型は `Matrix<T, MAX_ROW, RHS_MAX_COL>`（`RHS_MAX_COL` は右辺の列数の上限）である。

## transposed

```cpp
Matrix<T, MAX_COL, MAX_ROW> transposed() const
```

転置行列を返す。

## pow_entry_bmbm

```cpp
T pow_entry_bmbm(long long m, int i, int j) const
T pow_entry_bmbm(long long m, int k) const
```

Berlekamp--Massey と Bostan--Mori により、正方行列 $A$ について $A^m$ の指定した成分だけを求める。
`k` は行優先で数えた添字である。要素の型 `T` は四則演算と比較ができる体を想定している（典型的には `Modint`）。

## 時間計算量

$C = \texttt{MAX_ROW}\cdot \texttt{MAX_COL}$ とする。固定長版は保存領域を値初期化するため、実際の行列が小さくても構築に $C$ 要素分かかる。可変長版では、以下の $C$ は実際の要素数 $\mathrm{rows} \cdot \mathrm{cols}$ に置き換わる。

| 操作 | 時間計算量 |
| --- | --- |
| `Matrix()` | $O(C)$ |
| `Matrix(rows, cols)` | $O(C)$ |
| `Matrix(values)` | $O(C+\mathrm{rows}\cdot\mathrm{cols})$ |
| コピー・ムーブ構築、コピー・ムーブ代入 | $O(C)$ |
| `operator()(i, j)`、`rows()`、`cols()`、`empty()` | $O(1)$ |
| `zero(rows, cols)` | $O(C)$ |
| `operator==`、`operator!=` | $O(\mathrm{rows}\cdot\mathrm{cols})$（不一致なら途中で終了） |

演算については、左辺の実行時の大きさを $r\times k$、右辺を $k\times c$ とする。値を返す演算では、戻り値の固定長領域の初期化・コピーも含める。

| 操作 | 時間計算量 |
| --- | --- |
| `A += B`、`A -= B` | $O(rk)$ |
| `A + B`、`A - B` | $O(C+rk)$ |
| `A *= scalar`、`A /= scalar` | $O(rk)$ |
| `A * scalar`、`scalar * A`、`A / scalar` | $O(C+rk)$ |
| 単項 `+A` | $O(C)$ |
| 単項 `-A` | $O(C+rk)$ |
| `A * B` | $O(\texttt{MAX_ROW}\cdot \texttt{RHS_MAX_COL}+rkc)$ |
| `transposed()` | $O(C+\mathrm{rows}\cdot\mathrm{cols})=O(C)$ |

`pow_entry_bmbm` は、$n = \mathrm{rows} = \mathrm{cols}$、得られた線形漸化式の次数を $d\le n$ とすると、どちらのオーバーロードも次の計算量になる。

| 処理 | 時間計算量 |
| --- | --- |
| $2n$ 項のサンプル列の生成 | $O(n^3)$ |
| Berlekamp--Massey | $O(n^2)$ |
| Bostan--Mori | $O(d^2\log m)$ |
| `pow_entry_bmbm` 全体 | $O(n^3+d^2\log m)$ |

いずれも `T` の構築・コピー・四則演算・比較を $O(1)$ とした計算量である。一般の `T` では、各項に `T` の操作のコストを掛ける。

## 空間計算量

$C=\texttt{MAX_ROW}\cdot\texttt{MAX_COL}$（可変長版では実際の要素数）とする。

| API | 空間計算量（保存領域・追加領域） |
| --- | --- |
| 各コンストラクタ、コピー・ムーブ | 戻り値またはオブジェクトに $O(C)$ |
| `rows`、`cols`、`empty`、`operator()` | $O(1)$ |
| 比較、`+=`、`-=`、スカラーの `*=`、`/=` | $O(1)$ |
| 値を返す加減算・スカラー演算・単項演算・`zero`・`transposed` | 戻り値に $O(C)$ |
| 行列積 | 戻り値に $O(\texttt{MAX_ROW}\cdot \texttt{RHS_MAX_COL})$ |
| `pow_entry_bmbm` | $O(n+d)$ |

## 注意点

- コンストラクタと `zero` は $0 \le \mathrm{rows} \le \texttt{MAX_ROW}$、$0 \le \mathrm{cols} \le \texttt{MAX_COL}$ を要求する。
- `vector` の入力は矩形でなければならない。大きさや添字の違反は `std::runtime_error` を送出する（行数・列数が `int` に収まらない場合は `std::length_error`）。
- 加減算・比較は同じ大きさ、行列積は左辺の列数 $=$ 右辺の行数を要求し、違反すると `std::runtime_error` を送出する。
- スカラーによる除算の可否と、$0$ による除算の扱いは `T::operator/` に従う。
- `pow_entry_bmbm` は非負の指数・正方行列・有効な成分の添字を要求し、違反すると `std::runtime_error` を送出する。
- `pow_entry_bmbm` で使う除算が定義され、生成した列の上で必要な逆元が存在する必要がある。
