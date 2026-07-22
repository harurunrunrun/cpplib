---
title: Hemisphere Containment (半球包含判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/points_in_hemisphere.hpp
---

## API

```cpp
bool points_in_hemisphere(const vector<Point3>& points)
bool points_in_hemisphere_with_seed(
    const vector<Point3>& points,
    uint64_t random_seed
)
```

原点から各点へ向かう方向をすべて含む閉半球が存在するとき `true` を返す。
点までの距離は結果に影響せず、空集合には `true` を返す。

`points_in_hemisphere_with_seed` はSeidel法の制約挿入順を決めるseedを
明示する。同じ入力とseedには同じ結果を返す。期待線形時間の保証を使う場合は、
入力の値と並びから独立に一様に選んだseedを渡す。既存の
`points_in_hemisphere` は固定seed `0xD1B54A32D192ED03` を渡す
再現可能な互換APIである。

## API別の時間計算量・空間計算量

点数を $N$ とする。`points_in_hemisphere_with_seed` はseedが入力から独立な
一様乱数なら期待時間 $O(N)$、最悪時間 $O(N^2)$、追加領域 $O(N)$ である。
固定seedの `points_in_hemisphere` は最悪時間 $O(N^2)$、追加領域 $O(N)$ であり、
任意入力に対する期待線形時間は契約しない。
非零な法線は最大絶対値の座標を $+1$ または $-1$ に正規化できるため、6個の
二次元線形実行可能性問題へ帰着する。それぞれを Seidel のランダム化増分法で解く。
最終候補は全入力方向に対して検算する。

## 注意点

各点は有限な非零ベクトルでなければならず、違反時は `std::invalid_argument` を
送出する。境界の大円上にある方向も許す閉半球判定であり、開半球判定とは異なる。
単位化後の内積には `long double` の丸め誤差に応じた許容幅を使う。
LP内部では最終判定より狭い許容幅を使い、候補を最終許容幅で再検算する。
seedは判定結果の意味や許容誤差を変えず、実行可能性LPへ制約を挿入する順序だけを
変える。`random_seed ^ points.size()` を `std::mt19937_64` へ与えて
`std::shuffle` するため、入力から独立な一様seedは各固定入力長でも一様なseed
である。
