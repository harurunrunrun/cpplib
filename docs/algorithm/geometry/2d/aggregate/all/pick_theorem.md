---
title: Pick Theorem Aggregator (ピックの定理集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/pick_theorem.hpp
---

Pickの定理の結果型と格子点数計算APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `pick_theorem_result.hpp` | `PickTheoremResult` |
| `pick_theorem_counts.hpp` | `pick_theorem(polygon)` |

## 集約されるAPI

```cpp
template<std::integral Coordinate>
PickTheoremResult pick_theorem(
    const std::vector<IntegerPoint<Coordinate>>& polygon
);
```

単純格子多角形の境界上と内部の格子点数を同時に返す。

## API別の時間計算量・空間計算量

頂点数を $N$、座標差の最大絶対値を $C$ とする。

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `pick_theorem(polygon)` | $O(N\log C)$ | $O(1)$ |
| 結果fieldの参照 | $O(1)$ | $O(1)$ |

## 注意点

入力は3頂点以上の有限な整数座標からなる単純格子多角形でなければならない。
例外条件は `lattice_polygon_interior_points` と同じである。
