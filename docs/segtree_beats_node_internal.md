---
title: Segment Tree Beats Node Internal (セグメント木beatsノード内部実装)
documentation_of: ../src/structure/segtree/detail/segtree_beats_node.hpp
---

Segment Tree Beats各実装が共有するnode集約と遅延作用。

## 型

```cpp
segtree_beats_detail::Node<T>
```

`sum`, 最大値・第2最大値・最大値個数、最小値・第2最小値・最小値個数、
遅延加算 `add`、区間長 `length` を保持する。空nodeは `length == 0`。

## API

- `leaf(value)`：値1個のnodeを作る。
- `merge(left,right)`：隣接する2区間の集約を返す。
- `add(node,value)`：nodeの全要素へ `value` を加える。
- `chmin(node,value)`：nodeの最大値だけを `value` 以下へ下げる。Beatsの適用可能条件を呼出側が保証する。
- `chmax(node,value)`：nodeの最小値だけを `value` 以上へ上げる。Beatsの適用可能条件を呼出側が保証する。
- `inherit(parent,child)`：親の遅延加算・上限制約・下限制約を子へ反映する。

## 時間計算量

すべての関数は時間・追加空間計算量ともに $O(1)$。

## 注意点

- Segment Tree Beats系クラスの内部部品であり、通常は直接使用しない。
- `T` は既定構築、比較、加減乗算に対応する必要がある。
- `chmin` / `chmax` を任意の閾値へ直接使うAPIではない。第2極値との関係を満たさない呼出しではnodeの不変条件を保てない。
