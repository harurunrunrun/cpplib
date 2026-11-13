---
title: Weighted Parent Forest Distance (重み付き親森距離) [CORNET]
documentation_of: ../../../../src/structure/dsu/specialized/weighted_parent_forest_distance.hpp
---

各頂点が高々 1 つの親を持つ有向森について、根までの重み付き距離を、経路圧縮しながら管理する。

## API

### コンストラクタ

```cpp
template<class T, int MAX_SIZE = -1>
class WeightedParentForestDistance;

explicit WeightedParentForestDistance(int vertex_count);
```

全頂点がそれぞれ根で、根までの距離が `T{}` である森を作る。
`MAX_SIZE` は頂点数の上限で、省略（`-1`）すると上限を設けない。内部の領域は常に `std::vector` で実行時の頂点数に合わせて確保する。

```cpp
using Dynamic = WeightedParentForestDistance<long long>;
```

### size

```cpp
int size() const;
```

頂点数を返す。

### root

```cpp
int root(int vertex);
```

`vertex` が属する木の根を返し、通過した親へのポインタを根へ圧縮する。

### distance_to_root

```cpp
T distance_to_root(int vertex);
```

`vertex` から根までの辺の重みの和を返し、通過した親へのポインタを根へ圧縮する。

### link_root

```cpp
void link_root(int root_vertex, int parent_vertex, T edge_weight);
```

現在根である `root_vertex` の親を `parent_vertex` にし、その新しい辺の重みを `edge_weight` とする。異なる 2 本の木を接続する場合だけ成功する。

## 時間計算量

$N$ を頂点数、操作の前に辿る親ポインタの数を $h$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `size` | $O(1)$ | $O(1)$ |
| `root`, `distance_to_root` | $O(h)$ | $O(h)$ |
| `link_root` | $O(h)$ | $O(h)$ |

経路圧縮により、同じ経路に対する以降の取得は短くなる。

## 空間計算量

本体は $O(N)$。

## 注意点

- `0 <= vertex_count <= MAX_SIZE` が必要である。
- `root_vertex` は操作時点で根でなければならない。
- 同じ木の内部を接続して閉路を作ることはできない。
- `T` はデフォルト構築と加算ができ、すべての距離の和を表現できる必要がある。
- 頂点やサイズの範囲違反には `std::out_of_range` を、根でない頂点の接続や閉路を作る接続には `std::invalid_argument` を送出する。
