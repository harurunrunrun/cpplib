---
title: Subtree Add Sum Minimum Tree (部分木加算・総和・最小値木) [SALMAN]
documentation_of: ../src/structure/tree/subtree_add_sum_minimum_tree.hpp
---

根付き木の各頂点に `long long` 値を持たせ、部分木一括加算・部分木総和・
部分木最小値を処理します。

## API

### `SubtreeAddSumMinimumTree<MAX_SIZE>(vertex_count)`

```cpp
template<int MAX_SIZE>
class SubtreeAddSumMinimumTree;
```

`vertex_count` 頂点の構造を作ります。初期値はすべて0です。

### `size()`

```cpp
int size() const;
```

頂点数を返します。

### `set_value(vertex, value)`

```cpp
void set_value(int vertex, long long value);
```

`build` 前に頂点の初期値を設定します。同じ頂点へ再設定できます。

### `add_edge(left, right)`

```cpp
void add_edge(int left, int right);
```

`build` 前に無向辺を追加します。

### `build(root)`

```cpp
void build(int root = 0);
```

入力辺が作る木を `root` で根付き化し、部分木用の検索構造を構築します。

### `subtree_sum(vertex)`

```cpp
long long subtree_sum(int vertex) const;
```

`vertex` の部分木にある値の総和を返します。

### `subtree_minimum(vertex)`

```cpp
long long subtree_minimum(int vertex) const;
```

`vertex` の部分木にある値の最小値を返します。

### `subtree_add(vertex, delta)`

```cpp
void subtree_add(int vertex, long long delta);
```

`vertex` の部分木に `delta` を一括加算します。

### `add_subtree_by_capped_minimum(vertex, cap)`

```cpp
long long add_subtree_by_capped_minimum(
    int vertex,
    long long cap
);
```

`delta = min(cap, subtree_minimum(vertex))` を部分木全体へ加え、
実際に加えた `delta` を返します。

## API別の時間計算量・空間計算量

$N=vertex\_count$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N+MAX\_SIZE)$ |
| `size`, `set_value`, `add_edge` | $O(1)$ | $O(1)$ |
| `build` | $O(N+MAX\_SIZE)$ | $O(MAX\_SIZE)$ |
| `subtree_sum`, `subtree_minimum` | $O(\log N)$ | $O(1)$ |
| `subtree_add` | $O(\log N)$ | $O(1)$ |
| `add_subtree_by_capped_minimum` | $O(\log N)$ | $O(1)$ |

## 注意点

- `0 <= vertex_count <= MAX_SIZE` かつ `MAX_SIZE > 0` が必要です。
- `build` 前にちょうど1本の木を入力してください。
- 初期設定用APIは `build` 後に呼べず、`build` も1回だけ呼べます。
- queryと更新は `build` 後に呼んでください。
- 値・総和・遅延加算の全中間値は `long long` に収まる必要があります。
- サイズ・頂点範囲違反、木でない入力、呼出し順違反には対応する標準例外を
  送出します。
