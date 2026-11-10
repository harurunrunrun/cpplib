---
title: Minimum Enclosing Circle from Points (点列の最小包含円)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_circle_points.hpp
---

点集合をすべて含む半径最小の円を、シードを固定した乱択逐次構成法（randomized incremental 法）で求める。

## API

```cpp
MinimumEnclosingCircleResult minimum_enclosing_circle(
    std::vector<Point> points,
    std::uint64_t seed = 0x9e3779b97f4a7c15ULL
);
```

点列を値渡しで受け取り、`seed` から決まる順序で決定的にシャッフルしてから円を構築する。同じ入力と `seed` に対しては同じ支持点（support）を返す。支持点の個数 `support_size` は、空集合では 0、1 点では 1、2 点を直径とする円では 2、非共線な 3 点の外接円では 3 となる。入力に重複点を含んでもよい。

円そのものは返り値の `circle()` で、点の包含判定は `contains(point)` / `on_boundary(point)` で得られる。

## 時間計算量

点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `minimum_enclosing_circle(points, seed)` | 期待 $O(N)$、最悪 $O(N^3)$ | $O(N)$ |

期待計算量は、入力順をシャッフルする乱択逐次構成法としての評価である。

## 注意点

- 入力座標は有限でなければならない。
- 円を有限な `long double` で表せない場合は `std::overflow_error` を送出することがある。
- 3 点の支持点が許容誤差内で共線になった場合は、最も離れた 2 点を支持点とする円に縮約する。

## 使用例

```cpp
std::vector<Point> points{
    {-1, 0}, {1, 0}, {0, 0},
};
const MinimumEnclosingCircleResult result = minimum_enclosing_circle(points);
```
