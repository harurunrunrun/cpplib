---
title: Fenwick Tree of Ordered Sets (順序集合Fenwick木)
documentation_of: ../../../src/structure/fenwick_tree/fenwick_ordered_sets.hpp
---

位置と値の 2 次元の点の多重集合を、各ノードで値の候補を事前に座標圧縮した Fenwick Tree で管理する。

## API

```cpp
template<class Value> class FenwickOrderedSets;
```

- `FenwickOrderedSets(int n, const vector<pair<int, Value>>& possible_updates)`: 長さ $n$ の列を作り、更新され得る `(position, value)` の組を登録する。
- `int size() const`: 列の長さを返す。
- `void add(int position, const Value& value, int delta = 1)`: 点 `(position, value)` の個数を `delta` だけ増減する。
- `int prefix_count_less(int right, const Value& value) const`: 位置 $[0, \mathrm{right})$ で値が `value` 未満の点の個数を返す。
- `int count_less(int left, int right, const Value& value) const`: 位置 $[\mathrm{left}, \mathrm{right})$ で値が `value` 未満の点の個数を返す。
- `int count(int left, int right, const Value& lower, const Value& upper) const`: 位置 $[\mathrm{left}, \mathrm{right})$ で値が $[\mathrm{lower}, \mathrm{upper})$ に入る点の個数を返す。

## 時間計算量

$N$ を列の長さ、$K$ を登録した候補の個数とする。

- 構築: $O(N + K\log N\log K)$
- `add`、`prefix_count_less`、`count_less`、`count`: $O(\log N\log K)$
- `size`: $O(1)$

## 空間計算量

$O(N + K\log N)$。

## 注意点

- `add` する値は、構築時に同じ位置の候補として登録しておく必要がある。登録されていない値の `add` は `std::invalid_argument` を送出する。
- 位置・区間が範囲外の場合は `std::out_of_range`、`count` で $\mathrm{upper} \lt \mathrm{lower}$ の場合と `n` が負の場合は `std::invalid_argument` を送出する。
- 個数は `int` で数える。
