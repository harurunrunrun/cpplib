---
title: Minimum Adjacent Modular Merge Cost (隣接剰余マージの最小コスト) [MIXTURES]
documentation_of: ../src/algorithm/other/minimum_adjacent_modular_merge_cost.hpp
---

隣接要素を順に併合して1要素にするときの最小総コストを求める。

## `minimum_adjacent_modular_merge_cost`

```cpp
long long minimum_adjacent_modular_merge_cost(
    const vector<long long>& values,
    long long modulus
);
```

各要素は `[0, modulus)` の剰余値である。値 $a,b$ の隣接要素を併合すると
コスト $ab$ が加算され、新しい値は $(a+b)\bmod\mathrm{modulus}$ になる。
併合順序を自由に選んだときの総コストの最小値を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_adjacent_modular_merge_cost` | $O(N^3)$ | $O(N^2)$ |

## 注意点

- 空列と1要素列では0を返す。
- `modulus > 0` が必要で、各値は正規化済みでなければならない。
- 前提違反または答えが `long long` に収まらない場合は `runtime_error` を送出する。
