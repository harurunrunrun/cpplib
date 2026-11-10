---
title: Minimum Enclosing Sphere of 3D Point Set (三次元点集合の最小包含球)
documentation_of: ../../../../../../src/algorithm/geometry/3d/sphere/point_set/minimum_enclosing_sphere_points.hpp
---

乱択増分法により、有限な三次元点集合を含む半径最小の閉球を構築する。

## API

- `minimum_enclosing_sphere(points, seed = 0x9E3779B97F4A7C15ULL)`: 点集合 `points`（`std::vector<Point3>`、値渡し）の最小包含球を返す。`seed` を省略するとデフォルトのシードで、指定するとそのシードで処理順をシャッフルする。同じ入力とシードには同じ結果を返す。

## API別の時間計算量・空間計算量

点数を $N$ とする。シードが入力と独立なら期待時間 $O(N)$、最悪時間 $O(N^4)$。入力のコピーを含め、追加領域は $O(N)$。

## 注意点

- 空集合や非有限な座標には `std::invalid_argument` を送出する。有限な候補球を `long double` で表現できない場合は `std::overflow_error` を送出する。
- シードはデフォルト引数として実装されており、`points` だけを渡す呼び出しとシード付きの呼び出しは同じ関数を使う。
- 非再帰で実装されているため、大きな入力でもスタックを消費しない。
