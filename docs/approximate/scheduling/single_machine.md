---
title: Single-Machine Scheduling Orders (単一機械スケジューリング順序)
documentation_of: ../../../src/approximate/scheduling/single_machine.hpp
---

ジョブ数を $N$ とする。

## `earliest_due_date_order`

```cpp
vector<size_t> order = earliest_due_date_order(due_date);
```

納期の昇順（EDD）にジョブ番号を並べて返す。納期が同じならジョブ番号順である。この順序は最大遅れを最小化する。
時間計算量は $O(N \log N)$、追加空間計算量は $O(N)$。

## `weighted_shortest_processing_time_order`

```cpp
vector<size_t> order = weighted_shortest_processing_time_order(
    processing_time, weight
);
```

$p_i / w_i$ の昇順（Smith 則、WSPT）に並べて返す。この順序は単一機械での重み付き完了時刻和を最小化する。同値ならジョブ番号順である。
64 bit 以下の整数型は 128 bit の交差積で厳密に比較し、それ以外の型は `long double` へ変換して比較する。
時間計算量は $O(N \log N)$、追加空間計算量は $O(N)$。

## 注意点

- 配列の長さの不一致、負の処理時間、正でない重みに対しては `std::invalid_argument` を送出する。
- 値は有限で、`long double` で表現できなければならない。
