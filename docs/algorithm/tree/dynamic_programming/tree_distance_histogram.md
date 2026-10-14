---
title: Weighted Tree Distance Histogram (頂点重み付き木の距離分布) [frequency_table_of_tree_distance]
documentation_of: ../../../../src/algorithm/tree/dynamic_programming/tree_distance_histogram.hpp
---

`tree_distance_histogram(n,edges)` は距離ごとの異なる頂点の非順序対数を `vector<long long>` で返す。
添字 0 は 0。辺は `(u,v)` の無向木で、距離は辺数。
`tree_distance_pair_sums<MOD>(mass,edges)` は各対の寄与を `mass[u]*mass[v]` に変更し、法 `MOD` の配列を返す。

## 時間計算量

重心分解と畳み込みにより $O(N\log^2 N)$ 時間、$O(N)$ 空間。
厳密な対数は二つの法で求めて CRT 復元する。

## 注意点

木以外の入力は `invalid_argument`、頂点範囲外は `out_of_range`。
`MOD` は 2 以外の素数で、畳み込みに必要な長さの NTT を使えること。
厳密な対数は `998244353*469762049` 未満でなければならない。空木と 1 頂点を許す。
