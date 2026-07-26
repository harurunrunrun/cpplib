---
title: Minimum Enclosing Circle Aggregator (最小包含円集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/minimum_enclosing_circle.hpp
---

最小包含円の公開結果型と点列入力APIをまとめて読み込む後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `minimum_enclosing_circle_result.hpp` | `MinimumEnclosingCircleContainment`, `MinimumEnclosingCircleResult` |
| `minimum_enclosing_circle_points.hpp` | `minimum_enclosing_circle(points, seed)` |

必要なleafだけを直接includeできる。従来の `minimum_enclosing_circle.hpp` も
引き続き全APIを提供する。

## 集約されるAPI

```cpp
MinimumEnclosingCircleResult minimum_enclosing_circle(
    std::vector<Point> points,
    std::uint64_t seed = 0x9e3779b97f4a7c15ULL
);
```

点集合をすべて含む最小半径の円を、その円を決定する高々3点のsupportとして返す。
結果型は `containment`, `contains`, `on_boundary`, `circle` を提供する。

## API別の時間計算量・空間計算量

点数を $N$ とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `minimum_enclosing_circle(points, seed)` | 期待 $O(N)$、最悪 $O(N^3)$ | $O(N)$ |
| `containment` / `contains` / `on_boundary` / `circle` | $O(1)$ | $O(1)$ |
| `support`, `support_size` の参照 | $O(1)$ | $O(1)$ |

## 注意点

- 入力座標は有限でなければならない。
- 円を有限な `long double` で表せない場合は `std::overflow_error` を送出し得る。
- 同じ入力とseedには同じsupportを返す。
- 空集合のsupportは空で、`circle()` は中心原点・半径0を返す。
