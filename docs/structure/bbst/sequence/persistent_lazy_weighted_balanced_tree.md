---
title: Persistent Lazy Weighted Balanced Tree (完全永続遅延重み平衡木)
documentation_of: ../../../../src/structure/bbst/sequence/persistent_lazy_weighted_balanced_tree.hpp
---

重みを部分木サイズとする平衡条件を保ち、経路コピーを行う完全永続な暗黙キー（位置で索引する）重み平衡木。列の挿入、削除、区間反転、区間作用、区間積を扱う。更新元のバージョンは変更しない。

## テンプレート引数

```cpp
PersistentLazyWeightedBalancedTree<MonoidActLen, MAX_NODE = -1, MAX_VERSION = -1>
```

`MonoidActLen` は次を持つ。

```cpp
using S = ...;
using T = ...;
S op(S a, long long len_a, S b, long long len_b);
S e();
S mapping(T f, S x, long long len);
T composition(T f, T g);
T id();
```

`op(a, len_a, b, len_b)` は `a` の後ろに `b` を連結した順序で呼ばれる。可換である必要はない。`mapping(f, x, len)` は区間中の各要素へ同じ作用 `f` を適用した集約値を返す。`composition(f, g)` は `g` の後に `f` を適用する合成を返す。作用は連結と反転後の集約値の双方に適用できる必要がある。

`MAX_NODE` は初期列と全バージョンの経路コピーが共有するノード数の上限、`MAX_VERSION` はバージョン 0 を含むバージョン数の上限である。

`MAX_NODE`, `MAX_VERSION` を指定するとノードと根の表をオブジェクト内に固定確保する。省略（`-1`）すると `std::vector` で必要に応じて確保する可変長版になる（再確保のコストは償却され、各操作の計算量は変わらない）。

## コンストラクタ

```cpp
PersistentLazyWeightedBalancedTree()
explicit PersistentLazyWeightedBalancedTree(const std::vector<S>& values)
```

空、または `values` を初期列とするバージョン 0 を作る。

## 更新

```cpp
int insert(int p, const S& x, int version = 0)
int erase(int p, int version = 0)
int set(int p, const S& x, int version = 0)
int apply(int l, int r, const T& f, int version = 0)
int reverse(int l, int r, int version = 0)
```

`version` から新しいバージョンを作り、その番号を返す。区間は半開区間 `[l, r)` である。

ノードまたはバージョンの容量を超えた場合は `std::runtime_error` を投げ、バージョン数、使用ノード数、既存バージョンを更新前の状態に保つ。

## 取得

```cpp
int versions() const
int size(int version = 0) const
bool empty(int version = 0) const
S get(int p, int version = 0) const
S prod(int l, int r, int version = 0) const
S all_prod(int version = 0) const
std::vector<S> to_vector(int version = 0) const
```

取得は木を変更せず、新しいバージョンも作らない。

## 時間計算量

$N$ を対象バージョンの列長とし、モノイド演算・作用を $O(1)$ とする。

- デフォルトコンストラクタ: 固定長版は固定長領域の初期化 $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$、可変長版は $O(1)$
- `std::vector` からのコンストラクタ: 上記に加え長さ $N$ の平衡なバージョン 0 の構築 $O(N)$
- `versions`, `size`, `empty`, `all_prod`: $O(1)$
- `insert`, `erase`, `set`, `apply`, `reverse`: 最悪 $O(\log(N+1))$、新規バージョン $O(1)$、経路コピーノード $O(\log(N+1))$
- `get`, `prod`: 最悪 $O(\log(N+1))$
- `to_vector`: 出力要素数 $N$ に対して $O(N)$

空区間更新と、全区間への `apply` / `reverse` は $O(1)$ ノードでバージョンを生成する。平衡化は部分木の大きさ を用い、期待計算量ではない。

## 空間計算量

- 固定長版のノードと根の表: $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$。可変長版は作ったノード数とバージョン数に比例
- `to_vector` の戻り値と再帰スタック: $O(N)$

## 注意点

- バージョン、`insert` の位置、点添字、半開区間は対象バージョンの長さの範囲内でなければならない。
- 範囲違反、バージョン・ノードの容量超過は `std::runtime_error` を投げ、既存バージョンを変えない。
- 更新は内容が変わらない場合もバージョンを 1 個作り、取得操作は容量を消費しない。
- `prod` は左から右の順で合成し、空区間では単位元を返す。
- `composition(f,g)` は先に `g`、後に `f`。
- コピー・ムーブともに可能（暗黙に定義される）。コピーは全ノードを複製する。固定長版はノード配列をオブジェクト内に持つので、大きな容量ではグローバル変数や `static` 変数として置く。
