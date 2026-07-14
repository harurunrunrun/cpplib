---
title: Minimum Scuba Cylinder Weight (潜水ボンベの最小重量) [SCUBADIV]
documentation_of: ../src/algorithm/other/minimum_scuba_cylinder_weight.hpp
---

必要な酸素量と窒素量を満たすボンベ部分集合の最小重量を返す。

# ScubaCylinder

```cpp
struct ScubaCylinder { int oxygen, nitrogen, weight; };
```

# minimum_scuba_cylinder_weight

```cpp
int minimum_scuba_cylinder_weight(int required_oxygen, int required_nitrogen,
                                  const vector<ScubaCylinder>& cylinders);
```

達成不能なら `-1` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_scuba_cylinder_weight` | $O(NOA)$ | $O(OA)$ |

## 注意点

- 要求量と各ボンベ値は非負でなければならない。
