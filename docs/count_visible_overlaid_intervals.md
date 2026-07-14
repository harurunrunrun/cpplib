---
title: Visible Overlaid Intervals Count (重ね塗り区間の可視数) [POSTERS]
documentation_of: ../src/algorithm/range/count_visible_overlaid_intervals.hpp
---

閉区間を入力順に重ねた後、どこか一点以上が見えている区間の個数を返します。

## API

### `count_visible_overlaid_intervals(closed_intervals)`

```cpp
template<class Coordinate, int MAX_ATOMS>
int count_visible_overlaid_intervals(
    const vector<pair<Coordinate, Coordinate>>& closed_intervals
);
```

端点が逆順の区間は内部で正規化します。空入力では $0$ を返します。

## 時間計算量

区間数を $N$ として $O(N\log N)$ です。

## 空間計算量

異なる端点数を $M$ として $O(MAX\_ATOMS+N)$ です。

## 注意点

- 端点と隣接端点間の開区間を別々の原子セルとして扱います。
- $2M-1 <= MAX_ATOMS$ が必要です。
- `Coordinate` は全順序と等値比較を持たなければなりません。
