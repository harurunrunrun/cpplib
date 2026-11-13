---
title: Distance-Matrix Facility Location Heuristics (距離行列による施設配置ヒューリスティック)
documentation_of: ../../../src/approximate/facility/facility_location.hpp
---

`approximate::clustering` の既存の施設配置の実装を、`distance[facility][client]` 形式の行列から呼び出すためのアダプタである。アルゴリズム本体を重複して実装してはいない。以下、候補施設数を $F$、需要点数を $C$、採用した操作の回数を $I$ とする。

## `greedy_facility_location(distance, opening_cost)`

最良の単一施設から始め、開設費用と割当距離の合計を改善する施設を貪欲に追加する。戻り値の `FacilityLocationResult` の `labels[c]` は施設 ID ではなく、`facilities` 内での位置を表す。`KMedianResult::assignment` が施設 ID そのものを格納するのとは異なる。

- 時間計算量: 最悪 $O(F^3C)$
- 追加空間計算量: $O(FC+F+C)$

## `add_drop_facility_location(distance, opening_cost, maximum_iterations = 100)`

貪欲解から始め、施設 1 個の追加または削除で費用が改善する操作を繰り返す。全ての施設を閉じる操作は行わない。

- 時間計算量: $O(F^3C+IF^2C)$
- 追加空間計算量: $O(FC+F+C)$

## 注意点

- 行列は 1 行以上の長方形で、距離と開設費用は有限かつ非負でなければならない。開設費用は候補施設ごとに 1 個必要である。違反時は `std::invalid_argument` を送出する。
- 目的値が `long double` の範囲を超える場合は `std::overflow_error` を送出する。
- 一般の距離行列では近似比を保証しない。
