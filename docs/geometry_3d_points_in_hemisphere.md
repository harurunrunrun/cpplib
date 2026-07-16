---
title: Hemisphere Containment (半球包含判定)
documentation_of: ../src/algorithm/geometry/3d/points_in_hemisphere.hpp
---

## API

```cpp
bool points_in_hemisphere(const vector<Point3>& points)
```

原点から各点へ向かう方向をすべて含む閉半球が存在するとき `true` を返す。
点までの距離は結果に影響せず、空集合には `true` を返す。

## API別の時間計算量・空間計算量

点数を $N$ として、最悪時間 $O(N^2\log N)$、追加領域 $O(N)$。
各入力方向を境界上に置いたときの法線角を調べ、ほかの方向が禁止する円弧の端点を
ソートする。候補法線を点ごとに全走査する処理も $O(N^2)$ の範囲に収まる。
確率的な処理は使わないため、期待計算量と最悪計算量は同じ上界になる。

## 注意点

各点は有限な非零ベクトルでなければならず、違反時は `std::invalid_argument` を
送出する。境界の大円上にある方向も許す閉半球判定であり、開半球判定とは異なる。
単位化後の内積には `long double` の丸め誤差に応じた許容幅を使う。
