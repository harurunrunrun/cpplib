---
title: Matrix (行列)
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

以降、$C=MAX\_ROW\cdot MAX\_COL$ とする。固定長の保存領域を
値初期化するため、実際の行列が小さくても構築には $C$ 要素分かかる。

| 操作 | 時間計算量 |
| --- | --- |
| `Matrix()` | $O(C)$ |
| `Matrix(rows, cols)` | $O(C)$ |
| `Matrix(values)` | $O(C+rows+rows\cdot cols)$ |
| コピー・move構築、コピー・move代入 | $O(C)$ |

# 要素アクセス

```cpp
T& operator()(int i, int j)
const T& operator()(int i, int j)
```

## 時間計算量

| 操作 | 時間計算量 |
| --- | --- |
| `operator()(i, j)` | $O(1)$ |

# rows / cols

```cpp
int rows()
int cols()
bool empty()
```

## 時間計算量

| 操作 | 時間計算量 |
| --- | --- |
| `rows()` | $O(1)$ |
| `cols()` | $O(1)$ |
| `empty()` | $O(1)$ |

# zero / comparison

```cpp
Matrix::zero(rows, cols)
A == B
A != B
```

| 操作 | 時間計算量 |
| --- | --- |
| `zero(rows, cols)` | $O(C)$ |
| `operator==`, `operator!=` | $O(rows\cdot cols)$（不一致時は途中で終了） |

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

左辺の実行時サイズを $r\times k$、右辺を $k\times c$ とする。
値を返す演算では、戻り値の固定長領域の初期化・コピーも含める。

| 操作 | 時間計算量 |
| --- | --- |
| `A += B`, `A -= B` | $O(rk)$ |
| `A + B`, `A - B` | $O(C+rk)$ |
| `A *= scalar`, `A /= scalar` | $O(rk)$ |
| `A * scalar`, `scalar * A`, `A / scalar` | $O(C+rk)$ |
| 単項 `+A` | $O(C)$ |
| 単項 `-A` | $O(C+rk)$ |
| `A * B` | $O(MAX\_ROW\cdot RHS\_MAX\_COL+rkc)$ |

# transposed

```cpp
A.transposed()
```

転置行列を返す。

## 時間計算量

| 操作 | 時間計算量 |
| --- | --- |
| `transposed()` | $O(C+rows\cdot cols)=O(C)$ |

# pow_entry_bmbm

```cpp
A.pow_entry_bmbm(m, i, j)
A.pow_entry_bmbm(m, k)
```

BM + Bostan-Mori で、正方行列 `A` の `A^m` の指定成分だけを求める。
`k` は行優先で数えた添字。

要素型 `T` は加減乗除と比較ができる体を想定している。典型的には `Modint`。

## 時間計算量

$n=rows=cols$、得られた線形漸化式の次数を $d\le n$ とすると、
どちらの overload も次の計算量になる。

| 処理 | 時間計算量 |
| --- | --- |
| $2n$ 項のサンプル列生成 | $O(n^3)$ |
| Berlekamp--Massey | $O(n^2)$ |
| Bostan--Mori | $O(d^2\log m)$ |
| `pow_entry_bmbm` 全体 | $O(n^3+d^2\log m)$ |

追加領域は $O(n+d)$。

上表は `T` の構築・コピー・四則演算・比較を $O(1)$ とした計算量である。一般の `T` では、各項に実行する `T` の操作コストを掛ける。

## 注意点

- constructorと`zero`は`0 <= rows <= MAX_ROW`, `0 <= cols <= MAX_COL`を要求する。
- vector入力は矩形でなければならない。shape/index違反は例外。
- 加減算・比較は同shape、行列積は左列数=右行数を要求し、違反時は例外。
- scalar除算の可否と0除算の扱いは`T::operator/`に従う。
- `pow_entry_bmbm`は非負指数・正方行列・有効な成分indexを要求し、違反時は例外。
- BMBMで使う除算が定義され、生成列上で必要な逆元が存在する必要がある。
- 各値返却演算は新しい固定容量`Matrix`を返し、compound演算は`*this`への参照を返す。

## 空間計算量

$C=\mathtt{MAX\_ROW}\cdot\mathtt{MAX\_COL}$ とする。

| API | 空間計算量（保存領域・追加領域） |
| --- | --- |
| 各constructor、copy / move | 戻り値またはobjectに $O(C)$ |
| `rows`, `cols`, `empty`, `operator()` | $O(1)$ |
| 比較、`+=`, `-=`, scalar `*=`, `/=` | 返却値以外 $O(1)$ |
| 値を返す加減算・scalar演算・単項演算・`zero`・`transposed` | 戻り値に $O(C)$ |
| 行列積 | 戻り値に $O(\mathtt{MAX\_ROW}\cdot RHS\_MAX\_COL)$ |
| `pow_entry_bmbm` | $O(n+d)$。$d$ は漸化式次数 |
