---
title: Path Add Edge Get Tree (木上パス加算・辺取得) [GRASSPLA]
documentation_of: ../../../../src/algorithm/tree/construction/path_add_edge_get_tree.hpp
---

木上のパスに値を加え、指定した 1 辺の現在の値を取得する。
パスへの更新を、Euler tour 上の端点に対する差分（Fenwick Tree）へ変換して処理する。

## API

### コンストラクタ

```cpp
template<class T, int MAX_SIZE = -1>
class PathAddEdgeGetTree;
explicit PathAddEdgeGetTree(int vertex_count);
```

各辺の初期値が `T{}` である `vertex_count` 頂点の構造を作る。
`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使う。

```cpp
PathAddEdgeGetTree<long long> dynamic(vertex_count);
PathAddEdgeGetTree<long long, 100000> fixed(vertex_count);
```

### `size()`

```cpp
int size() const;
```

頂点数を返す。

### `add_edge(left, right)`

```cpp
void add_edge(int left, int right);
```

`build` の前に無向辺を追加する。

### `build(root)`

```cpp
void build(int root = 0);
```

入力した辺が作る木を `root` で根付き木にする。

### `path_add(left, right, delta)`

```cpp
void path_add(int left, int right, const T& delta);
```

`left` と `right` を結ぶ単純パスに含まれるすべての辺に `delta` を加える。

### `edge_get(left, right)`

```cpp
T edge_get(int left, int right) const;
```

隣接する頂点 `left`, `right` を結ぶ辺の現在の値を返す。

## API 別の時間計算量・空間計算量

$N$ を `vertex_count` とする。固定容量版ではコンストラクタに $O(\texttt{MAX_SIZE})$ が加わる。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$（固定容量版は $O(N+\texttt{MAX_SIZE})$） | $O(N)$（固定容量版は $O(N+\texttt{MAX_SIZE})$） |
| `size` | $O(1)$ | $O(1)$ |
| `add_edge` | 償却 $O(1)$ | 償却 $O(1)$ |
| `build` | $O(N)$ | $O(N)$ |
| `path_add` | $O(\log N)$ | $O(1)$ |
| `edge_get` | $O(\log N)$ | $O(1)$ |

## 注意点

- `vertex_count >= 0` が必要で、固定容量版では `vertex_count <= MAX_SIZE` も必要である。違反時は `std::out_of_range` を送出する。`MAX_SIZE` は `-1` または正の値でなければならない（`static_assert`）。
- `build` の前に連結な木を入力し、`build` は 1 回だけ呼ぶ。2 回目の `build` は `std::logic_error` を送出する。
- `build` の後に `add_edge` を呼ぶと `std::logic_error` を送出する。`build` の前に `path_add`・`edge_get` を呼んでも `std::logic_error` を送出する。
- `edge_get` の 2 頂点は同じ木の辺の両端でなければならない。そうでなければ `std::invalid_argument` を送出する。
- `T` は加減算とデフォルト構築ができ、すべての中間値を表現できる必要がある。
