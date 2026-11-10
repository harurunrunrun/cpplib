---
title: Pick Theorem Aggregator (ピックの定理集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/pick_theorem.hpp
---

Pick の定理の結果型と、格子点の個数を計算する API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `pick_theorem_result.hpp` | `PickTheoremResult` |
| `pick_theorem_counts.hpp` | `pick_theorem(polygon)` |

## 集約される API

```cpp
struct PickTheoremResult {
    __uint128_t boundary_points;
    __uint128_t interior_points;
};

template<std::integral Coordinate>
PickTheoremResult pick_theorem(const std::vector<IntegerPoint<Coordinate>>& polygon);
```

単純な格子多角形について、境界上の格子点の個数と内部の格子点の個数を同時に返す。

## 計算量

頂点数を $N$、座標差の絶対値の最大値を $C$ とする。

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `pick_theorem(polygon)` | $O(N\log C)$ | $O(1)$ |
| 結果のフィールドの参照 | $O(1)$ | $O(1)$ |

## 注意点

入力は、3 頂点以上の整数座標からなる単純な格子多角形でなければならない。例外の条件は `lattice_polygon_interior_points` と同じである。
