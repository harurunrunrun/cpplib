---
title: Maximum Downward Grid Path Sum (下向き格子経路の最大和) [BYTESM2]
documentation_of: ../src/algorithm/other/dynamic_programming/maximum_downward_grid_path_sum.hpp
---

数値格子の最上段から最下段まで進む経路の要素和の最大値を求める。

## `maximum_downward_grid_path_sum`

```cpp
long long maximum_downward_grid_path_sum(
    const vector<vector<int>>& grid
);
```

各行から次の行の同じ列、左隣、右隣のいずれかへ移動できる。
到達可能な最上段・最下段のセルは自由に選べる。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_downward_grid_path_sum` | $O(HW)$ | $O(W)$ |

## 注意点

- 空の格子または幅0の格子では0を返す。
- 行長が不一致なら `runtime_error` を送出する。
