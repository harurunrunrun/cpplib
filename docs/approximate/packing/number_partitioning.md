---
title: Number Partitioning Heuristics (数分割ヒューリスティック)
documentation_of: ../../../src/approximate/packing/number_partitioning.hpp
---

非負の重みを持つ品物を複数の部分へ分けるヒューリスティックを、`approximate::packing` 名前空間で提供する。
重みが同じ場合は、元の添字または部分番号が小さいものを先に選ぶ。

以下、品物数を $N$、部分数を $P$ とする。

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

`part_of_item[i]` は品物 $i$ が属する部分の番号、`part_sum[p]` は部分 $p$ の重みの和である。
時間計算量は `part_count` が $O(1)$、`maximum_part_sum` が $O(P)$ で、追加空間計算量はともに $O(1)$。
`part_sum` が空のとき、`maximum_part_sum` は `Weight{}` を返す。

## `greedy_load_balancing`

```cpp
greedy_load_balancing(item_weight, part_count, decreasing = false)
```

指定した順に、各品物を現在の和が最小の部分へ割り当てる。和が同じ部分の中では部分番号が最小のものを選ぶ。
`decreasing = true` のときは、重みの降順に安定ソートしてから割り当てる。
時間計算量は通常 $O(N \log P)$、降順指定時 $O(N \log N + N \log P)$ で、追加空間計算量は $O(N + P)$。

## `greedy_multiway_partition`

```cpp
greedy_multiway_partition(item_weight, part_count)
```

重みの降順に、現在の和が最小の部分へ割り当てる多分割の貪欲法である。
時間計算量は $O(N \log N + N \log P)$、追加空間計算量は $O(N + P)$。

## `greedy_number_partition`

```cpp
greedy_number_partition(item_weight)
```

重みの降順に、現在の和が小さい方へ割り当てる 2 分割の貪欲法である。
時間計算量は $O(N \log N)$、追加空間計算量は $O(N)$。

## `greedy_two_way_partition`

```cpp
greedy_two_way_partition(item_weight)
```

`greedy_number_partition` と同じ 2 分割の貪欲法を実行する。
時間計算量は $O(N \log N)$、追加空間計算量は $O(N)$。

## `round_robin_partition`

```cpp
round_robin_partition(item_weight, part_count, decreasing = false)
```

品物を部分 $0, 1, \ldots, P-1, 0, \ldots$ へ順番に割り当てる。
`decreasing = true` のときは、重みの降順に安定ソートしてから割り当てる。
時間計算量は通常 $O(N)$、降順指定時 $O(N \log N)$ で、追加空間計算量は $O(N + P)$。

## `largest_differencing_partition`

```cpp
largest_differencing_partition(item_weight)
```

最大の 2 値を取り出してその差で置き換える Largest Differencing Method で 2 分割する。
差分木を復元し、`part_of_item` に実際の割り当てを返す。
時間計算量は $O(N \log N)$、追加空間計算量は $O(N)$。

## `karmarkar_karp_partition`

```cpp
karmarkar_karp_partition(item_weight)
```

`largest_differencing_partition` と同じ Karmarkar–Karp 法を実行する。
時間計算量は $O(N \log N)$、追加空間計算量は $O(N)$。

## 注意点

- `Weight` は全順序を持ち、加減算ができる数値型とする。
- `part_count == 0`、負の重み、浮動小数点型での非有限値に対しては `std::invalid_argument` を送出する。
- 有界な整数型または浮動小数点型で部分和が表現範囲を超える場合は `std::overflow_error` を送出する。
- 差分木の節点数が `size_t` で表せないほど品物が多い場合、`largest_differencing_partition` は `std::length_error` を送出する。
- いずれもヒューリスティックであり、最適な分割や一定の近似比は保証しない。
