---
title: Minimum Enclosing Sphere of 3D Point Set (三次元点集合の最小包含球)
documentation_of: ../../../../src/algorithm/geometry/3d/minimum_enclosing_sphere_points.hpp
---

乱択増分法で有限な三次元点集合を含む半径最小の閉球を構築する。

## API

- `minimum_enclosing_sphere(points, seed = 0x9E3779B97F4A7C15ULL)`:
  点集合の最小包含球を返す。引数を一つだけ渡すと既定seed、二つ渡すと
  指定seedで処理順をshuffleする。同じ入力とseedには同じ結果を返す。

## API別の時間計算量・空間計算量

点数を $N$ とする。seedが入力と独立なら期待時間 $O(N)$、最悪時間
$O(N^4)$。入力copyを含む追加領域は $O(N)$。

## 注意点

空集合または非有限座標には `std::invalid_argument`。有限候補球を
`long double` で表せない場合は `std::overflow_error`。
既定引数は公開overloadの一部であり、vector-only呼出しとseed付き呼出しは
同じ関数を使う。
