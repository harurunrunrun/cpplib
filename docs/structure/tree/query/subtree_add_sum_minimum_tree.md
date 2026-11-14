---
title: Subtree Add Sum Minimum Tree (部分木加算・総和・最小値木) [SALMAN]
documentation_of: ../../../../src/structure/tree/query/subtree_add_sum_minimum_tree.hpp
---

根付き木の各頂点に `long long` 値を持たせ、部分木への一括加算・部分木の総和・
部分木の最小値を処理する。

## API

### `SubtreeAddSumMinimumTree<MAX_SIZE>(vertex_count)`

```cpp
template<int MAX_SIZE = -1>
class SubtreeAddSumMinimumTree;
```

`vertex_count` 頂点の構造を作る。初期値はすべて 0 である。`MAX_SIZE` を省略すると可変長版になる。

### `size()`

```cpp
int size() const;
```

頂点数を返す。

### `set_value(vertex, value)`

```cpp
void set_value(int vertex, long long value);
```

`build` 前に頂点の初期値を設定する。同じ頂点に再設定できる。

### `add_edge(left, right)`

```cpp
void add_edge(int left, int right);
```

`build` 前に無向辺を追加する。

### `build(root)`

```cpp
void build(int root = 0);
```

入力した辺が作る木を `root` を根として根付け、オイラーツアー上の遅延セグメント木を構築する。

### `subtree_sum(vertex)`

```cpp
long long subtree_sum(int vertex) const;
```

`vertex` の部分木にある値の総和を返す。

### `subtree_minimum(vertex)`

```cpp
long long subtree_minimum(int vertex) const;
```

`vertex` の部分木にある値の最小値を返す。

### `subtree_add(vertex, delta)`

```cpp
void subtree_add(int vertex, long long delta);
```

`vertex` の部分木の全頂点に `delta` を加算する。

### `add_subtree_by_capped_minimum(vertex, cap)`

```cpp
long long add_subtree_by_capped_minimum(
    int vertex,
    long long cap
);
```

`delta = min(cap, subtree_minimum(vertex))` を部分木全体に加え、
実際に加えた `delta` を返す。

## API 別の時間計算量・空間計算量

$N$ を `vertex_count` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N+\texttt{MAX_SIZE})$（可変長版は $O(N)$） |
| `size`, `set_value`, `add_edge` | $O(1)$ | $O(1)$ |
| `build` | $O(N+\texttt{MAX_SIZE})$（可変長版は $O(N)$） | $O(\texttt{MAX_SIZE})$（可変長版は $O(N)$） |
| `subtree_sum`, `subtree_minimum` | $O(\log N)$ | $O(1)$ |
| `subtree_add` | $O(\log N)$ | $O(1)$ |
| `add_subtree_by_capped_minimum` | $O(\log N)$ | $O(1)$ |

## 注意点

- `0 <= vertex_count` が必要で、固定長版では `vertex_count <= MAX_SIZE` かつ `MAX_SIZE > 0` が必要である。
- `build` 前に、ちょうど 1 本の木になるように辺を入力する。
- 初期設定用の API（`set_value`, `add_edge`）は `build` 後には呼べず、`build` も 1 回だけ呼べる。
- クエリと更新は `build` 後に呼ぶ。
- 値・総和・遅延加算のすべての中間値は `long long` に収まる必要がある。
- 頂点数・頂点番号の範囲違反では `out_of_range`、呼び出し順の違反では `logic_error`、木でない入力では `runtime_error` を送出する。
- 容量を指定した固定配列版（`SubtreeAddSumMinimumTree<N>`）と可変長版（`SubtreeAddSumMinimumTree<>`）は同じヘッダで利用できる。
