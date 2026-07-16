---
title: Weighted Parent Forest Distance (重み付き親森距離) [CORNET]
documentation_of: ../src/structure/dsu/weighted_parent_forest_distance.hpp
---

各頂点が高々1つの親を持つ有向森について、根への重み付き距離を経路圧縮しながら
管理します。

## API

### `WeightedParentForestDistance<T, MAX_SIZE>(vertex_count)`

```cpp
template<class T, int MAX_SIZE>
class WeightedParentForestDistance;
```

全頂点がそれぞれ根で、根までの距離が `T{}` の森を作ります。

### `size()`

```cpp
int size() const;
```

頂点数を返します。

### `root(vertex)`

```cpp
int root(int vertex);
```

`vertex` が属する木の根を返し、通過した親pointerを根へ圧縮します。

### `distance_to_root(vertex)`

```cpp
T distance_to_root(int vertex);
```

`vertex` から根までの辺重み和を返し、通過した親pointerを根へ圧縮します。

### `link_root(root_vertex, parent_vertex, edge_weight)`

```cpp
void link_root(
    int root_vertex,
    int parent_vertex,
    T edge_weight
);
```

現在根である `root_vertex` の親を `parent_vertex` にし、その新しい辺の
重みを `edge_weight` とします。異なる2本の木を接続する場合だけ成功します。

## API別の時間計算量・空間計算量

操作前に辿る親pointer数を $h$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `size` | $O(1)$ | $O(1)$ |
| `root`, `distance_to_root` | $O(h)$ | $O(h)$ |
| `link_root` | $O(h)$ | $O(h)$ |

経路圧縮により、同じ経路に対する後続の取得は短縮されます。

## 注意点

- `0 <= vertex_count <= MAX_SIZE` が必要です。
- `root_vertex` は操作時点で根でなければなりません。
- 同じ木の内部を接続して閉路を作ることはできません。
- `T` はデフォルト構築と加算ができ、全距離和を表現できる必要があります。
- 頂点・サイズ違反には `out_of_range`、根でない頂点や閉路を作る接続には
  `invalid_argument` を送出します。
