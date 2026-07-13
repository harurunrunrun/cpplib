---
title: k-median共通API
documentation_of: ../src/approximate/facility/common.hpp
---

# k-median共通API

## `KMedianResult`

`facilities` は昇順の開設施設、`assignment[c]` は需要点 `c` に割り当てた施設IDそのもの、`cost` は距離和、`local_search_iterations` は採用した交換回数を表す。距離が同じ場合は施設IDが小さい方へ割り当てる。

## `evaluate_k_median(distance, facilities)`

指定施設集合の割当と目的値を計算する。

- 時間計算量: $O(FC+kC)$（行列検査と変換を含む）
- 追加空間計算量: $O(FC+C+k)$

施設集合は空でなく、添字は範囲内で相異なる必要がある。距離行列の形状、有限性、非負性も検査する。距離和のoverflowは例外とする。
