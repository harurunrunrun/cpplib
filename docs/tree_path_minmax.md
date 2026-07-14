---
title: Tree Path Minimum Maximum (木上パス最小最大) [DISQUERY]
documentation_of: ../src/algorithm/tree/tree_path_minmax.hpp
---

重み付き静的木のパス上で辺重みの最小値と最大値を同時に返す。

## API

### `tree_path_minmax_op<Weight>` / `tree_path_minmax_identity<Weight>`

最小値・最大値pairの結合と単位元を返す。どちらも時間計算量は $O(1)$。


### `TreePathMinmax<Weight, MAX_SIZE>(n)`

空の構造を作る。時間計算量は $O(n\log n)$。

### `add_edge(left, right, weight)`

重み付き無向辺を追加する。時間計算量は償却 $O(1)$。

### `build(root = 0)`

二分lifting索引を構築する。時間計算量は $O(n\log n)$。

### `path_minmax(left, right)`

`pair<minimum, maximum>` を返す。時間計算量は $O(\log n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下、辺集合は連結な木でなければならない。
- 同じ頂点の空パスでは `{numeric_limits<Weight>::max(), numeric_limits<Weight>::lowest()}` を返す。
- `build` 前や範囲外頂点では `runtime_error`。
