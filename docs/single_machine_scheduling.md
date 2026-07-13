---
title: Single Machine Scheduling Orders
documentation_of: ../src/approximate/scheduling/single_machine.hpp
---

## `earliest_due_date_order`

```cpp
vector<size_t> order = earliest_due_date_order(due_date);
```

納期の昇順（EDD）にjob番号を返す。同値はjob番号順である。
最大遅れを最小化する。時間$O(N\log N)$、追加領域$O(N)$。

## `weighted_shortest_processing_time_order`

```cpp
vector<size_t> order = weighted_shortest_processing_time_order(
    processing_time, weight
);
```

$p_i/w_i$の昇順（Smith則、WSPT）を返し、単一機械の重み付き完了時刻和を
最小化する。同値はjob番号順である。64-bit以下の整数型は128-bit交差積で厳密に、
それ以外は`long double`へ変換して比較する。
時間$O(N\log N)$、追加領域$O(N)$。

配列長不一致、負の処理時間、非正の重みでは`std::invalid_argument`を送出する。
値は有限で`long double`へ表現可能でなければならない。
