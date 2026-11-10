---
title: Weighted Tree Distance Histogram (頂点重み付き木の距離分布) [frequency_table_of_tree_distance]
documentation_of: ../../../../src/algorithm/tree/dynamic_programming/tree_distance_histogram.hpp
---

木の頂点対を距離ごとに数える。

## API

```cpp
std::vector<long long> tree_distance_histogram(int n, const std::vector<std::pair<int, int>>& edges)
template<int MOD>
std::vector<Modint<MOD>> tree_distance_pair_sums(
    const std::vector<Modint<MOD>>& mass, const std::vector<std::pair<int, int>>& edges)
```

- `tree_distance_histogram(n, edges)` は、長さ $n$ の列を返し、添字 $d$ に距離がちょうど $d$ である相異なる頂点の非順序対の個数を格納する。添字 $0$ の値は $0$ である。辺は `(u, v)` で与える無向木で、距離は辺数で測る。
- `tree_distance_pair_sums<MOD>(mass, edges)` は、各頂点対の寄与を $1$ ではなく `mass[u] * mass[v]` にしたものを、法 `MOD` で返す。

重心分解と畳み込みで計算する。`tree_distance_histogram` は 2 つの法（$998244353$ と $469762049$）で求めた値を CRT で復元して厳密な個数を得る。

## 時間計算量

$O(N\log^2 N)$ 時間、$O(N)$ 空間である。

## 注意点

- 木でない入力では `std::invalid_argument`、頂点が範囲外の場合は `std::out_of_range` を送出する。`n < 0` の場合も `std::invalid_argument` を送出する。
- `MOD` は $2$ 以外の素数で、畳み込みに必要な長さの NTT が使えなければならない。
- 厳密な個数は $998244353 \times 469762049$ 未満でなければならない。
- 空木と 1 頂点の木も許す。
