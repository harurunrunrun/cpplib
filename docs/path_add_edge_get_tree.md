---
title: Path Add Edge Get Tree (木上パス加算・辺取得) [GRASSPLA]
documentation_of: ../src/algorithm/tree/path_add_edge_get_tree.hpp
---

木上のパスに値を加え、指定した1辺の現在値を取得します。
パス更新をEuler tour上の端点差分へ変換して処理します。

## API

### `PathAddEdgeGetTree<T, MAX_SIZE>(vertex_count)`

```cpp
template<class T, int MAX_SIZE>
class PathAddEdgeGetTree;
```

各辺の初期値が `T{}` である `vertex_count` 頂点の構造を作ります。

### `size()`

```cpp
int size() const;
```

頂点数を返します。

### `add_edge(left, right)`

```cpp
void add_edge(int left, int right);
```

`build` 前に無向辺を追加します。

### `build(root)`

```cpp
void build(int root = 0);
```

入力辺が作る木を `root` で根付き化します。

### `path_add(left, right, delta)`

```cpp
void path_add(int left, int right, const T& delta);
```

`left` と `right` の単純路に含まれる全辺へ `delta` を加えます。

### `edge_get(left, right)`

```cpp
T edge_get(int left, int right) const;
```

隣接頂点 `left`, `right` を結ぶ辺の現在値を返します。

## API別の時間計算量・空間計算量

$N=vertex\_count$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N+MAX\_SIZE)$ | $O(N+MAX\_SIZE)$ |
| `size` | $O(1)$ | $O(1)$ |
| `add_edge` | 償却 $O(1)$ | 償却 $O(1)$ |
| `build` | $O(N)$ | $O(N)$ |
| `path_add` | $O(\log N)$ | $O(1)$ |
| `edge_get` | $O(\log N)$ | $O(1)$ |

## 注意点

- `0 <= vertex_count <= MAX_SIZE` かつ `MAX_SIZE > 0` が必要です。
- `build` 前に連結な木を入力し、`build` は1回だけ呼んでください。
- `add_edge` は `build` 後に呼べません。更新と取得は `build` 後に呼びます。
- `edge_get` の2頂点は同じ木辺の両端でなければなりません。
- `T` は加減算とデフォルト構築ができ、全中間値を表現できる必要があります。
