---
title: Persistent Reversible Splay Tree (完全永続反転可能スプレー木)
documentation_of: ../../../../src/structure/bbst/sequence/persistent_reversible_splay_tree.hpp
---

経路コピーを行う完全永続な暗黙キー（位置で索引する）Splay 木。列の挿入、削除、区間反転、区間積を扱う。更新元のバージョンは変更しない。

## テンプレート引数

```cpp
PersistentReversibleSplayTree<Monoid, MAX_NODE = -1, MAX_VERSION = -1>
```

`Monoid` は次を持つ。

```cpp
using S = ...;
S op(S a, S b);
S e();
```

`op(a, b)` は `a` の後ろに `b` を連結した順序で呼ばれる。可換である必要はない。

`MAX_NODE` は初期列と全バージョンの経路コピーが共有するノード数の上限、`MAX_VERSION` はバージョン 0 を含むバージョン数の上限である。

`MAX_NODE`, `MAX_VERSION` を指定するとノードと根の表をオブジェクト内に固定確保する。省略（`-1`）すると `std::vector` で必要に応じて確保する可変長版になる（再確保のコストは償却され、各操作の計算量は変わらない）。

## コンストラクタ

```cpp
PersistentReversibleSplayTree()
explicit PersistentReversibleSplayTree(const std::vector<S>& values)
```

空、または `values` を初期列とするバージョン 0 を作る。

## 更新

```cpp
int insert(int p, const S& x, int version = 0)
int push_front(const S& x, int version = 0)
int push_back(const S& x, int version = 0)
int erase(int p, int version = 0)
int set(int p, const S& x, int version = 0)
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

取得では splay せず、新しいバージョンも作らない。

## 時間計算量

対象バージョンの木の高さを $H$、列長を $N$ とする。$H$ は最悪 $N$。モノイド演算は $O(1)$ とする。

- デフォルトコンストラクタ: 固定長版は固定長領域の初期化 $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$、可変長版は $O(1)$
- `std::vector` からのコンストラクタ: 上記に加え長さ $N$ の平衡なバージョン 0 の構築 $O(N)$
- `versions`, `size`, `empty`, `all_prod`: $O(1)$
- `insert`, `push_front`, `push_back`, `erase`, `set`, `reverse`: $O(H)$、新規バージョン $O(1)$、経路コピーノード $O(H)$
- `get`, `prod`: $O(H)$。木を変形せずバージョンも生成しない
- `to_vector`: 出力要素数 $N$ に対して $O(N)$

更新は内部の定数回の分割・併合を含む。直前バージョンだけを更新する一本道では償却 $O(\log(N+1))$。古いバージョンから分岐する操作列にはこの償却評価を共有できず、各更新の保証は $O(H)$。

## 空間計算量

- 固定長版のノードと根の表: $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$。可変長版は作ったノード数とバージョン数に比例
- `to_vector` の戻り値と再帰スタック: $O(N)$

## 注意点

- バージョンは `[0,versions())`、`insert` の位置は `[0,size(version)]`、点添字は `[0,size(version))`。
- 半開区間は `0 <= l <= r <= size(version)`。違反すると `std::runtime_error` を投げる。
- 更新でバージョン数・ノード数の上限を超える場合は `std::runtime_error` を投げ、既存のバージョンを変えない。
- 更新は内容が変わらない場合もバージョンを 1 個作る。取得操作はバージョン・ノードを消費しない。
- `prod` は左から右の順で合成し、空区間では単位元を返す。
- コピー・ムーブともに可能（暗黙に定義される）。コピーは全ノードを複製する。固定長版はノード配列をオブジェクト内に持つので、大きな容量ではグローバル変数や `static` 変数として置く。
