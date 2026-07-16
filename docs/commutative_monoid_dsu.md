---
title: Commutative Monoid Disjoint Set Union (可換モノイド付き素集合データ構造)
documentation_of: ../src/structure/dsu/commutative_monoid_dsu.hpp
---

各頂点の値を可換モノイドで集約しながら、頂点集合を併合する素集合データ構造。
連結成分ごとの積を代表元に保持する。

## テンプレート引数

```cpp
CommutativeMonoidDSU<Monoid, MAX_SIZE>
```

- `Monoid` は `structure/types/monoid.hpp` の `Monoid` と同じく、型 `S`、単位元 `e()`、二項演算 `op(a,b)` を持つオブジェクト
- `Monoid::op` は結合的かつ可換で、`Monoid::e()` は単位元
- S はデフォルト構築、コピー、代入ができる型
- `MAX_SIZE` は頂点数の上限で、正でなければならない

## コンストラクタ

### `CommutativeMonoidDSU(n)`

```cpp
explicit CommutativeMonoidDSU(int n = MAX_SIZE);
```

`n` 個の頂点を互いに異なる成分として作る。各頂点の値は `Monoid.e()` で初期化する。

### `CommutativeMonoidDSU(values)`

```cpp
explicit CommutativeMonoidDSU(const vector<S>& values);
```

`values[v]` を頂点 `v` の初期値として作る。

## `merge(first, second)`

2頂点の成分を併合する。異なる成分を併合した場合だけ `true`、既に同じ成分なら `false` を返す。
新しい成分の積は、併合前の2成分の積を `Monoid.op` で結合した値となる。

## `leader(vertex)` / `same(first, second)`

`leader` は成分の代表元を返す。`same` は2頂点が同じ成分に属するかを返す。

## `component_size(vertex)` / `size(vertex)`

`vertex` を含む成分の頂点数を返す。2つの API は同じ値を返す。

## `component_product(vertex)`

`vertex` を含む成分に属する全頂点の値を `Monoid.op` で集約した値を返す。

## `size()` / `groups()`

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

## API別の時間計算量・空間計算量

頂点数を $N$、逆Ackermann関数を $\alpha$ とする。

| API | 償却時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 各コンストラクタ | $O(MAX_SIZE)$ | $O(1)$ |
| `merge` | $O(\alpha(N))$ | $O(\log N)$ |
| `leader`, `same` | $O(\alpha(N))$ | $O(\log N)$ |
| `component_size`, `size(vertex)` | $O(\alpha(N))$ | $O(\log N)$ |
| `component_product` | $O(\alpha(N))$ | $O(\log N)$ |
| `size()`, `groups()` | $O(1)$ | $O(1)$ |

本体は固定配列として $O(\mathtt{MAX\_SIZE})$ の領域を持つ。追加空間は代表探索の再帰スタックを含む。

## 注意点

- `0 <= n <= MAX_SIZE` でなければコンストラクタは `runtime_error` を送出する。
- 頂点引数はすべて `[0,n)`。範囲外なら `runtime_error` を送出する。
- 演算が非可換の場合、union by size による結合順序に意味を持たせられないため使用できない。
- 頂点値は構築時に確定する。併合後に個別の頂点値を変更する API は持たない。

