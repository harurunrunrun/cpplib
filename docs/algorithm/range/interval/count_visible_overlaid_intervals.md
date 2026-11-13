---
title: Visible Overlaid Intervals Count (重ね塗り区間の可視数) [POSTERS]
documentation_of: ../../../../src/algorithm/range/interval/count_visible_overlaid_intervals.hpp
---

閉区間を入力順に上へ重ねて塗ったとき、少なくとも 1 点が見えている（後から重ねた区間に完全には覆われない）区間の個数を返す。

## API

### `count_visible_overlaid_intervals(closed_intervals)`

```cpp
template<class Coordinate, int MAX_ATOMS = -1>
int count_visible_overlaid_intervals(
    const vector<pair<Coordinate, Coordinate>>& closed_intervals
);
```

端点が逆順の区間は内部で入れ替えて扱う。空入力では $0$ を返す。

`MAX_ATOMS` を省略すると、内部の双対セグメント木に実行時サイズの `std::vector` を使う可変長版になる。指定すると固定長版になる。

```cpp
auto count = count_visible_overlaid_intervals(intervals);
auto fixed = count_visible_overlaid_intervals<Coordinate, 400000>(intervals);
```

## 時間計算量

区間数を $N$ として $O(N\log N)$。

## 空間計算量

可変長版は $O(N)$、固定長版は $O(\texttt{MAX_ATOMS}+N)$。

## 注意点

- 相異なる端点の数を $M$ として、各端点と隣接する端点の間の開区間を別々の原子セルとして扱う（計 $2M-1$ 個）。
- 固定長版では $2M-1\le\texttt{MAX_ATOMS}$ でなければならない。
- 区間数が `INT_MAX / 4` を超えると `std::length_error` を送出する。
- `Coordinate` は全順序と等値比較を持たなければならない。
