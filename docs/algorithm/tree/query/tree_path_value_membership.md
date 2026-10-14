---
title: Tree Path Value Membership (木上パス値存在判定) [GOT]
documentation_of: ../../../../src/algorithm/tree/query/tree_path_value_membership.hpp
---

静的な頂点値付き木で、パス上に指定値が存在するかを返す。

## API

### `TreePathValueMembership<Value, MAX_SIZE>(n)`

空の木を作る。時間計算量は $O(n)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は $O(1)$。

### `build(values, root = 0)`

頂点値と順序統計索引を構築する。時間計算量は $O(n\log n)$。

### `contains(left, right, value)`

両端を含むパスに `value` があれば `true`。時間計算量は $O(\log n)$。

## 注意点

- `values.size()==n<=MAX_SIZE` で、辺集合は連結な木でなければならない。
- `build` 前や範囲外頂点では `runtime_error`。

## 容量を省略する可変長版

容量テンプレート引数をすべて省略すると、実行時サイズに合わせた `std::vector` を使う。容量を明示する固定長版も同じヘッダにある。

```cpp
TreePathValueMembership<int> dynamic(vertex_count);
TreePathValueMembership<int, 100000> fixed(vertex_count);
```

構築・前処理・保存領域は $O(N\log N)$、クエリは $O(\log N)$ 時間。
