---
title: Maximum Downward Grid Path Sum (下向き格子経路の最大和) [BYTESM2]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/profile_grid/maximum_downward_grid_path_sum.hpp
---

数値の格子の最上段から最下段まで進む経路について、通るマスの値の和の最大値を求める。

## `maximum_downward_grid_path_sum`

```cpp
long long maximum_downward_grid_path_sum(
    const std::vector<std::vector<int>>& grid
);
```

各行から、次の行の同じ列・左隣の列・右隣の列のいずれかに移動できる。
出発する最上段のマスと、到着する最下段のマスは自由に選べる。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_downward_grid_path_sum` | $O(HW)$ | $O(W)$ |

## 注意点

- 空の格子、または幅 $0$ の格子では $0$ を返す。
- 行の長さが一致しない場合は `std::runtime_error` を送出する。
- 和は `long long` で計算し、オーバーフローは検査しない。
