---
title: Point Set Maximum Area Triangle Result (点集合内最大面積三角形の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/point_set_maximum_area_triangle_result.hpp
---

点集合から選んだ最大面積の三角形を保持する結果型。

## API

```cpp
struct PointSetMaximumAreaTriangleResult {
    std::array<Point, 3> vertices{};
    long double doubled_area = 0.0L;
    bool valid = false;
    long double area() const noexcept;
};
```

- `vertices`: 選ばれた 3 頂点。
- `doubled_area`: 三角形の面積の 2 倍。
- `valid`: 3 点以上の入力から非退化な三角形を選べたかを表す。
- `area()`: 面積（`doubled_area / 2`）を返す。

## 計算量

各メンバの参照と `area()` は、時間・追加空間とも $O(1)$。

## 注意点

`valid == false` のとき、`vertices` と面積の値は有効な三角形を表さない。
