---
title: Minimum Enclosing Circle Aggregator (最小包含円集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/minimum_enclosing_circle.hpp
---

最小包含円の公開の結果型と、点列を入力とする API をまとめて読み込む後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `minimum_enclosing_circle_result.hpp` | `MinimumEnclosingCircleContainment`, `MinimumEnclosingCircleResult` |
| `minimum_enclosing_circle_points.hpp` | `minimum_enclosing_circle(points, seed)` |

必要な leaf header だけを直接 include することもできる。従来どおり `minimum_enclosing_circle.hpp` を include すれば全 API を使える。

## 集約される API

```cpp
MinimumEnclosingCircleResult minimum_enclosing_circle(
    std::vector<Point> points,
    std::uint64_t seed = 0x9e3779b97f4a7c15ULL
);
```

点集合をすべて含む半径最小の円を、その円を決定する高々 3 点の支持点（`support`, `support_size`）として返す。`seed` は点をシャッフルする乱数のシードである。

結果型は次のメンバ関数を持つ。

- `containment(point)`: `MinimumEnclosingCircleContainment::OUTSIDE(-1)`, `ON_BOUNDARY(0)`, `INSIDE(1)` のいずれかを返す。
- `contains(point)`: 円の内部または境界上なら `true` を返す。
- `on_boundary(point)`: 境界上なら `true` を返す。
- `circle()`: 円を `Circle` として返す。

## 計算量

点の個数を $N$ とする。

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `minimum_enclosing_circle(points, seed)` | 期待 $O(N)$、最悪 $O(N^3)$ | $O(N)$ |
| `containment` / `contains` / `on_boundary` / `circle` | $O(1)$ | $O(1)$ |
| `support`, `support_size` の参照 | $O(1)$ | $O(1)$ |

## 注意点

- 入力座標は有限でなければならない。
- 円を有限な `long double` で表せない場合は `std::overflow_error` を送出することがある。
- 同じ入力と `seed` に対しては同じ支持点を返す。
- 空集合の支持点は空で、`circle()` は中心が原点・半径 $0$ の円を返す。
