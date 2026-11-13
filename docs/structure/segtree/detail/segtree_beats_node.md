---
title: Segment Tree Beats Node Internal (セグメント木beatsノード内部実装)
documentation_of: ../../../../src/structure/segtree/detail/segtree_beats_node.hpp
---

Segment Tree Beats の各実装が共有する、ノードの集約と遅延作用である。

## 型

```cpp
template<class T>
struct segtree_beats_detail::Node;
```

区間和 `sum`、最大値・2 番目の最大値・最大値の個数、最小値・2 番目の最小値・最小値の個数、遅延加算 `add`、区間の長さ `length` を保持する。空のノードは `length == 0` である。

## API

- `leaf(value)`: 値 1 個のノードを作る。
- `merge(left, right)`: 隣接する 2 区間の集約を返す。
- `add(node, value)`: ノードの全要素に `value` を加える。
- `chmin(node, value)`: ノードの最大値だけを `value` まで下げる。Beats の適用条件（2 番目の最大値 `< value <` 最大値）は呼び出し側が保証する。
- `chmax(node, value)`: ノードの最小値だけを `value` まで上げる。Beats の適用条件は呼び出し側が保証する。
- `inherit(parent, child)`: 親の遅延加算・上限制約・下限制約を子に反映する。

## 時間計算量

すべての関数は、時間・追加空間計算量ともに $O(1)$ である。

## 注意点

- Segment Tree Beats 系クラスの内部部品であり、通常は直接使わない。
- `T` はデフォルト構築・比較・加減乗算ができる必要がある。
- `chmin` / `chmax` は任意の閾値に直接使う API ではない。2 番目の極値との関係を満たさない呼び出しでは、ノードの不変条件を保てない。
