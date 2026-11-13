---
title: Tree Path Value Membership (木上パス値存在判定) [GOT]
documentation_of: ../../../../src/algorithm/tree/query/tree_path_value_membership.hpp
---

頂点に値を持つ静的な木で、パス上に指定した値が存在するかを返す。
内部では `TreePathOrderStatistics` を使う。

## API

### `TreePathValueMembership<Value, MAX_SIZE>(n)`

```cpp
template<class Value, int MAX_SIZE = -1>
struct TreePathValueMembership;
explicit TreePathValueMembership(int n);
```

頂点数 `n` の空の木を作る。時間計算量は $O(n\log n)$（固定容量版では $O(\texttt{MAX_SIZE} \log \texttt{MAX_SIZE})$）である。
`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使う。

```cpp
TreePathValueMembership<int> dynamic(vertex_count);
TreePathValueMembership<int, 100000> fixed(vertex_count);
```

### `add_edge(left, right)`

無向辺を追加する。時間計算量は $O(1)$ である。

### `build(values, root = 0)`

頂点の値と順序統計の索引を構築する。時間計算量は $O(n\log n)$、保持する領域は $O(n\log n)$ である。

### `contains(left, right, value)`

両端を含むパス上に `value` があれば `true` を返す。時間計算量は $O(\log n)$ である。

## 注意点

- `values.size() == n` でなければならない。固定容量版では `n <= MAX_SIZE` も必要である。辺集合は連結な木でなければならない。
- `build` 前の呼び出しや範囲外の頂点では `std::runtime_error` を送出する。
