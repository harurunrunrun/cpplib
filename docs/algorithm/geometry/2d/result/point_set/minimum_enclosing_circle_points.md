---
title: Minimum Enclosing Circle from Points (点列の最小包含円)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_circle_points.hpp
---

点集合をすべて含む最小半径の円を固定seedのrandomized incremental法で求める。

## API

```cpp
MinimumEnclosingCircleResult minimum_enclosing_circle(
    std::vector<Point> points,
    std::uint64_t seed = 0x9e3779b97f4a7c15ULL
);
```

点列を値渡しで受け取り、決定的にshuffleしてから円を構築する。同じ入力とseedには
同じsupportを返す。空集合ではsupport数0、1点では1、直径円では2、非共線な
3点の外接円では3となる。重複点を含められる。

## 時間計算量

点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `minimum_enclosing_circle(points, seed)` | 期待 $O(N)$、最悪 $O(N^3)$ | $O(N)$ |

期待計算量は入力順をshuffleするrandomized incremental法に対するもの。

## 注意点

- 入力座標は有限でなければならない。
- 円を有限な `long double` で表せない場合は `std::overflow_error` を送出し得る。
- 3点supportが許容誤差内で共線なら、最遠2点supportへ縮約する。

## 使用例

```cpp
std::vector<Point> points{
    {-1, 0}, {1, 0}, {0, 0},
};
const MinimumEnclosingCircleResult result = minimum_enclosing_circle(points);
```
