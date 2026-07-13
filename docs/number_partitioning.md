---
title: Number Partitioning Heuristics (数分割ヒューリスティック)
documentation_of: ../src/approximate/packing/number_partitioning.hpp
---

非負の重みを複数の部分へ分けるheuristicを`approximate::packing`名前空間で提供する。
同じ重みを比較した場合は、元添字または部分番号が小さいものを先に選ぶ。

## `PartitionResult`

```cpp
template<class Weight>
struct PartitionResult {
    vector<size_t> part_of_item;
    vector<Weight> part_sum;
    size_t part_count() const;
    Weight maximum_part_sum() const;
};
```

`part_of_item[i]`はitem $i$の部分番号、`part_sum[p]`は部分$p$の重みの和を表す。
`part_count`は$O(1)$、`maximum_part_sum`は時間計算量は $O(P)$・追加空間計算量は $O(1)$。
空の`part_sum`に対する`maximum_part_sum`は`Weight{}`を返す。

## `greedy_load_balancing`

```cpp
greedy_load_balancing(item_weight, part_count, decreasing = false)
```

指定した順にitemを現在の和が最小の部分へ割り当てる。和が同じ部分では部分番号最小を選ぶ。
`decreasing=true`では重みの降順へ安定sortしてから割り当てる。
item数を$N$、部分数を$P$とすると、時間計算量は通常$O(N\log P)$、
降順指定時$O(N\log N+N\log P)$、追加空間計算量は$O(N+P)$。

## `greedy_multiway_partition`

```cpp
greedy_multiway_partition(item_weight, part_count)
```

重みの降順に、現在の和が最小の部分へ割り当てるmultiway greedy法。
時間計算量は$O(N\log N+N\log P)$、追加空間計算量は$O(N+P)$。

## `greedy_number_partition`

```cpp
greedy_number_partition(item_weight)
```

重みの降順に、現在の和が小さい方へ割り当てる2分割greedy法。
時間計算量は$O(N\log N)$、追加空間計算量は$O(N)$。

## `greedy_two_way_partition`

```cpp
greedy_two_way_partition(item_weight)
```

`greedy_number_partition`と同じ2分割greedy法を実行する。
時間計算量は$O(N\log N)$、追加空間計算量は$O(N)$。

## `round_robin_partition`

```cpp
round_robin_partition(item_weight, part_count, decreasing = false)
```

itemを部分$0,1,\ldots,P-1,0,\ldots$へ順番に割り当てる。
`decreasing=true`では重みの降順へ安定sortしてから割り当てる。
時間計算量は通常$O(N)$、降順指定時$O(N\log N)$、追加空間計算量は$O(N+P)$。

## `largest_differencing_partition`

```cpp
largest_differencing_partition(item_weight)
```

最大の2値を取り出して差へ置き換えるLargest Differencing Methodで2分割する。
差分木を復元するため、`part_of_item`に実際の割り当てを返す。
時間計算量は$O(N\log N)$、追加空間計算量は$O(N)$。

## `karmarkar_karp_partition`

```cpp
karmarkar_karp_partition(item_weight)
```

`largest_differencing_partition`と同じKarmarkar--Karp法を実行する。
時間計算量は$O(N\log N)$、追加空間計算量は$O(N)$。

`part_count == 0`、負の重み、浮動小数点数の非有限値は`std::invalid_argument`。
有界整数型の部分和、または浮動小数点型の部分和が表現範囲を超える場合は
`std::overflow_error`。`Weight`は全順序付きの加減算可能な数値型とする。

## 注意点

個数・寸法・重みは各APIの値域条件を満たし、中間の和や積も使用型で表現可能でなければならない。heuristicは、明記した場合を除いて最適packingや一定の近似比を保証しない。
