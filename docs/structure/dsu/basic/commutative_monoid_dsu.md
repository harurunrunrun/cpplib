---
title: Commutative Monoid Disjoint Set Union (可換モノイド付き素集合データ構造)
documentation_of: ../../../../src/structure/dsu/basic/commutative_monoid_dsu.hpp
---

各頂点の値を可換モノイドで集約しながら頂点集合を併合する素集合データ構造。
連結成分ごとの積を代表元に保持する。

## テンプレート引数

```cpp
CommutativeMonoidDSU<Monoid, MAX_SIZE = -1>
```

- `Monoid`: `structure/types/monoid/monoid.hpp` の `Monoid` と同様に、型 `S`、単位元 `e()`、二項演算 `op(a, b)` を持つオブジェクト（非型テンプレート引数）
- `Monoid.op` は結合的かつ可換で、`Monoid.e()` は単位元でなければならない
- `S` はデフォルト構築・コピー・代入ができる型
- `MAX_SIZE`: 頂点数の上限（正）。省略（`-1`）すると、実行時の頂点数に合わせて `std::vector` を確保する可変長版になる

## コンストラクタ

```cpp
explicit CommutativeMonoidDSU(int n = MAX_SIZE);
explicit CommutativeMonoidDSU(const vector<S>& values);
```

1 つ目は $n$ 個の頂点をそれぞれ単独の成分として作り、各頂点の値を `Monoid.e()` で初期化する。可変長版では `n` のデフォルトは $0$ である。
2 つ目は `values[v]` を頂点 $v$ の初期値として、`values.size()` 頂点で作る。

## merge

```cpp
bool merge(int first, int second)
```

2 頂点の成分を併合する。異なる成分を併合した場合だけ `true` を、既に同じ成分なら `false` を返す。
新しい成分の積は、併合前の 2 成分の積を `Monoid.op` で結合した値となる。

## leader / same

```cpp
int leader(int vertex)
bool same(int first, int second)
```

`leader` は成分の代表元を返す。`same` は 2 頂点が同じ成分に属するかを返す。

## component_size / size(vertex)

```cpp
int component_size(int vertex)
int size(int vertex)
```

`vertex` を含む成分の頂点数を返す。2 つの関数は同じ値を返す。

## component_product

```cpp
S component_product(int vertex)
```

`vertex` を含む成分に属する全頂点の値を `Monoid.op` で集約した値を返す。

## size() / groups()

```cpp
int size() const
int groups() const
```

引数なしの `size()` は全頂点数、`groups()` は現在の連結成分数を返す。

## 使用例

```cpp
long long op(long long a, long long b){ return a + b; }
long long e(){ return 0; }
constexpr Monoid<op, e> sum_monoid;

CommutativeMonoidDSU<sum_monoid, 100> dsu(vector<long long>{3, -1, 4});
dsu.merge(0, 2);
assert(dsu.component_product(0) == 7);
```

## 時間計算量

頂点数を $N$、逆アッカーマン関数を $\alpha$ とする。

| API | 時間計算量（償却） | 追加空間計算量 |
| --- | --- | --- |
| 各コンストラクタ | $O(N)$（固定長版は配列の初期化に $O(\texttt{MAX_SIZE})$） | $O(1)$ |
| `merge` | $O(\alpha(N))$ | $O(\log N)$ |
| `leader`, `same` | $O(\alpha(N))$ | $O(\log N)$ |
| `component_size`, `size(vertex)` | $O(\alpha(N))$ | $O(\log N)$ |
| `component_product` | $O(\alpha(N))$ | $O(\log N)$ |
| `size()`, `groups()` | $O(1)$ | $O(1)$ |

追加空間は、代表探索の再帰スタックである。

## 空間計算量

固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(N)$ の領域を持つ。`merge` は新たな領域を消費しない。

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ でなければ、コンストラクタは `std::runtime_error` を送出する。
- 頂点引数はすべて $[0, n)$ でなければならない。範囲外なら `std::runtime_error` を送出する。
- 演算が非可換な場合は使用できない（union by size により結合順序が定まらないため）。
- 頂点値は構築時に確定する。併合後に個々の頂点値を変更する API は無い。
