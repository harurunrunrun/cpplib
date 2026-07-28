---
title: Matroid Partition (マトロイド分割)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/matroid/matroid_partition.hpp
---

各要素を高々1つのgroupへ割り当て、groupごとに異なるマトロイドで独立となる被覆要素数を最大化する。

## `MatroidPartitionResult`

`groups[i]` は第 $i$ matroidへ割り当てた独立集合、`uncovered` は割り当てられなかった要素。

## `matroid_partition(ground_set_size, independent)`

要素・groupのcopyを作り、partition matroidとdirect-sum matroidの交差へ帰着する。group数を $k$、copy ground set sizeを $N=nk$、最終被覆数を $r$、元oracle時間を $T$ とすると時間計算量 $O(rN^2(kT+n))$、空間計算量 $O(N+n+k)$。

## 注意点

`independent[i]` は第 $i$ groupの独立性oracle。matroidが0個なら全要素を `uncovered` として返す。copy数が `size_t` を超える場合は `std::length_error`。
