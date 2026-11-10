---
title: Tree Path Minimum Maximum (木上パス最小最大) [DISQUERY]
documentation_of: ../../../../src/algorithm/tree/query/tree_path_minmax.hpp
---

辺に重みを持つ静的な木のパス上で、辺の重みの最小値と最大値を同時に返す。
内部では `StaticTreePathFold` を使う。

## API

### `tree_path_minmax_op<Weight>` / `tree_path_minmax_identity<Weight>`

（最小値, 最大値）の組の結合と単位元を返す。どちらも時間計算量は $O(1)$ である。

### `TreePathMinmax<Weight, MAX_SIZE>(n)`

```cpp
template<class Weight, int MAX_SIZE = -1>
struct TreePathMinmax;
explicit TreePathMinmax(int n);
```

頂点数 `n` の空の構造を作る。時間計算量は $O(n\log n)$ である。
`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使う。

```cpp
TreePathMinmax<long long> dynamic(vertex_count);
TreePathMinmax<long long, 100000> fixed(vertex_count);
```

### `add_edge(left, right, weight)`

重み付きの無向辺を追加する。時間計算量は償却 $O(1)$ である。

### `build(root = 0)`

ダブリングの索引を構築する。時間計算量は $O(n\log n)$、保持する領域は $O(n\log n)$ である。

### `path_minmax(left, right)`

`std::pair<Weight, Weight>`（最小値, 最大値）を返す。時間計算量は $O(\log n)$ である。

## 注意点

- 辺集合は連結な木でなければならない。固定容量版では頂点数が `MAX_SIZE` 以下でなければならない。
- 同じ頂点を指定した空のパスでは `{std::numeric_limits<Weight>::max(), std::numeric_limits<Weight>::lowest()}` を返す。
- `build` 前の呼び出しや範囲外の頂点では `std::runtime_error` を送出する。
