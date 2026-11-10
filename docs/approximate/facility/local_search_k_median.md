---
title: Local Search k-Median (局所探索k-メディアン)
documentation_of: ../../../src/approximate/facility/local_search_k_median.hpp
---

以下、候補施設数を $F$、需要点数を $C$、開設する施設数を $k$、採用した交換の回数を $I$ とする。

## `local_search_k_median(distance, k, maximum_iterations = 100)`

ADD 法の初期解から始め、全ての 1 交換近傍を最良改善で探索する。`add_drop_k_median` と同じ探索を、k-メディアンの局所探索として呼び出す API である。

- 時間計算量: $O(kFC+Ik(F-k)(C+k\log k))$
- 追加空間計算量: $O(FC+C+F)$

## 注意点

- 距離空間上の k-メディアンで知られている、複数の施設を同時に交換する局所探索による定数近似とは異なり、本 API は 1 交換だけを行う。一般の行列では近似比を保証しない。
- `1 <= k <= F` でなければならない。
- 距離行列は `distance[facility][client]` 形式の長方形で、全要素が有限かつ非負でなければならない。違反時は `std::invalid_argument` を送出する。
