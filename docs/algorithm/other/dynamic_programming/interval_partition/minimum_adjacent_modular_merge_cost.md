---
title: Minimum Adjacent Modular Merge Cost (隣接剰余マージの最小コスト) [MIXTURES]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/minimum_adjacent_modular_merge_cost.hpp
---

隣接する要素を順に併合して 1 要素にするときの、最小の総コストを区間 DP で求める。

## `minimum_adjacent_modular_merge_cost`

```cpp
long long minimum_adjacent_modular_merge_cost(
    const std::vector<long long>& values,
    long long modulus
);
```

各要素は $[0, \mathrm{modulus})$ の剰余である。値が $a, b$ の隣接する要素を併合するとコスト $ab$ が加算され、新しい値は $(a+b)\bmod\mathrm{modulus}$ になる。併合の順序を自由に選んだときの総コストの最小値を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_adjacent_modular_merge_cost` | $O(N^3)$ | $O(N^2)$ |

## 注意点

- 空の列と 1 要素の列では $0$ を返す。
- `modulus > 0` が必要で、各値は $[0, \mathrm{modulus})$ に正規化済みでなければならない。
- 前提に違反した場合、要素数が `int` の範囲を超える場合、または答えが `long long` に収まらない場合は `std::runtime_error` を送出する。
