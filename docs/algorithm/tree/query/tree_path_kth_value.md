---
title: Tree Path K-th Value (木上パス K 番目値) [COT]
documentation_of: ../../../../src/algorithm/tree/query/tree_path_kth_value.hpp
---

頂点に値を持つ静的な木のパス上で、$k$ 番目に小さい値を返す。

## API

### `TreePathKthValue<Value, MAX_SIZE>(n)`

```cpp
template<class Value, int MAX_SIZE = -1>
struct TreePathKthValue;
explicit TreePathKthValue(int n);
```

頂点数 `n` の空の木を作る。時間計算量は $O(n)$ である。
`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使う。

```cpp
TreePathKthValue<int> dynamic(vertex_count);
TreePathKthValue<int, 100000> fixed(vertex_count);
```

### `add_edge(left, right)`

無向辺を追加する。時間計算量は $O(1)$ である。

### `build(values, root = 0)`

頂点の値と、永続的な順序統計の索引を構築する。時間計算量は $O(n\log n)$、保持する領域は $O(n\log n)$ である。

### `kth_smallest(left, right, order)`

パス上で 0-indexed で `order` 番目に小さい値を返す。時間計算量は $O(\log n)$ である。

## 注意点

- `values.size() == n` でなければならない。固定容量版では `n <= MAX_SIZE` も必要である。辺集合は連結な木でなければならない。
- `build` 前の呼び出し、範囲外の頂点、パスの頂点数以上の `order` では `std::runtime_error` を送出する。
