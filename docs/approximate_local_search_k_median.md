---
title: Local Search k-Median
documentation_of: ../src/approximate/facility/local_search_k_median.hpp
---

# Local Search k-Median

## `local_search_k_median(distance, k, maximum_iterations)`

ADD初期解から全1交換近傍を最良改善で探索する。`add_drop_k_median` と同じ探索を、k-median局所探索として呼び出すAPI。

- 時間計算量: $O(kFC+Ik(F-k)(C+k\log k))$
- 追加空間計算量: $O(FC+C+F)$
- 近似: metric k-medianで複数交換まで行う既知の定数近似とは異なり、本APIは1交換だけである。一般行列では近似比を保証しない。
