---
title: k-Median Common API (k-メディアン共通API)
documentation_of: ../../../src/approximate/facility/common.hpp
---

`approximate::facility` 名前空間の k-メディアン系 API で共通に使う結果型と評価関数である。以下、候補施設数を $F$、需要点数を $C$、施設集合の大きさを $k$ とする。

## `KMedianResult`

`facilities` は開設した施設の昇順列、`assignment[c]` は需要点 `c` に割り当てた施設の ID そのもの、`cost` は距離の和、`local_search_iterations` は採用した交換の回数である。距離が同じ場合は ID が小さい施設へ割り当てる。

## `evaluate_k_median(distance, facilities)`

指定した施設集合に対する割当と目的値を計算する。

- 時間計算量: $O(FC+kC)$（行列の検査と変換を含む）
- 追加空間計算量: $O(FC+C+k)$

## 注意点

- 施設集合は空でなく、添字は範囲内で相異なっていなければならない。
- 距離行列は `distance[facility][client]` 形式の長方形で、全要素が有限かつ非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 距離の和がオーバーフローした場合は例外を送出する。
