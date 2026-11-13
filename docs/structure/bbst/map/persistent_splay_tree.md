---
title: Persistent Splay Tree (完全永続スプレー木)
documentation_of: ../../../../src/structure/bbst/map/persistent_splay_tree.hpp
---

経路コピーによって各バージョンの根を保持する、永続 Splay 木による順序付き集合。同じキーは 1 個だけ保持する。バージョン 0 は空集合である。

## テンプレート引数

```cpp
PersistentSplaySet<Key, MAX_NODE = -1, MAX_VERSION = -1, Compare = std::less<Key>>
```

- `MAX_NODE`: 新規ノードとコピーしたノードの総数の上限
- `MAX_VERSION`: バージョン数（バージョン 0 を含む）の上限

`MAX_NODE`, `MAX_VERSION` を指定するとノードと根の表を固定確保する。省略（`-1`）すると `std::vector` で必要に応じて確保する可変長版になる（再確保のコストは償却され、各操作の計算量は変わらない）。

## コンストラクタ

```cpp
explicit PersistentSplaySet(const Compare& compare = Compare())
```

`Compare` による順序でキーを保持する。

## insert / erase

```cpp
int insert(const Key& x, int version = 0)
int erase(const Key& x, int version = 0)
```

バージョン `version` を更新した新しいバージョンを作り、その番号を返す。既に存在するキーの `insert`、存在しないキーの `erase` も、探索結果を splay した新しいバージョンを作る。

更新時は探索経路だけをコピーし、新しいバージョン上で splay する。古いバージョンのノードは変更しない。

## size / empty / versions / contains

```cpp
int size(int version = 0) const
bool empty(int version = 0) const
int versions() const
bool contains(const Key& x, int version = 0) const
```

## 順序

```cpp
int order_of_key(const Key& x, int version = 0) const
int order_of_key_upper(const Key& x, int version = 0) const
std::optional<Key> kth(int k, int version = 0) const
std::vector<Key> to_vector(int version = 0) const
```

`order_of_key` は `Compare` の順序で `x` より前にある要素数、`order_of_key_upper` は `x` と同値な要素も含めた個数を返す。`kth` は 0-indexed で、範囲外なら `std::nullopt` を返す。`to_vector` は `Compare` の順序で全要素を返す。

## 境界探索

```cpp
std::optional<Key> lower_bound(const Key& x, int version = 0) const
std::optional<Key> upper_bound(const Key& x, int version = 0) const
std::optional<Key> max_leq(const Key& x, int version = 0) const
std::optional<Key> max_less(const Key& x, int version = 0) const
```

`lower_bound` と `upper_bound` は `std::set<Key, Compare>` と同じ意味である。`max_leq` は `upper_bound(x)` の直前、`max_less` は `lower_bound(x)` の直前の要素を返す。該当する要素がなければ `std::nullopt` を返す。

読み取り操作はバージョンを作らないので splay を行わない。

## 時間計算量

対象バージョンの木の高さを $H$、要素数を $N$ とする。$H$ は最悪 $N$ になる。

- コンストラクタ: 固定長版は $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$、可変長版は $O(1)$
- `versions`, `size`, `empty`: $O(1)$
- `contains`, `order_of_key`, `order_of_key_upper`, `kth`, `lower_bound`, `upper_bound`, `max_leq`, `max_less`: $O(H)$
- `insert`, `erase`: $O(H)$。新しいバージョンを 1 個、経路コピーのノードを $O(H)$ 個作る
- `to_vector`: $O(N)$

読み取り操作は木を変形しないので、償却評価の対象外である。各更新が直前に作ったバージョンを使う一本道の操作列なら、更新は償却 $O(\log(N+1))$ になる。古いバージョンから分岐する操作列ではポテンシャルの議論が成り立たないので、各更新の保証は $O(H)$ である。

## 空間計算量

- 固定長版のノードと根の表: $O(\texttt{MAX_NODE}+\texttt{MAX_VERSION})$。可変長版は作ったノード数とバージョン数に比例
- `to_vector` の戻り値とスタック: $O(N)$

## 注意点

- `Compare` は狭義弱順序でなければならない。バージョンは $[0,\texttt{versions()})$ でなければならず、範囲外なら `std::runtime_error` を投げる。
- 更新でバージョン数またはノード数が上限を超える場合は `std::runtime_error` を投げ、既存のバージョンと使用ノード数は更新前の状態に保たれる。
- 更新は常にバージョンを 1 個作り、読み取り操作はバージョン・ノードを消費しない。
- 古いバージョンからの分岐を繰り返すと、木の高さが $\Theta(N)$ のまま残り得る。
