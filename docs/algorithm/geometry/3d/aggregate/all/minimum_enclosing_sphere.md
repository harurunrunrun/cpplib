---
title: 3D Minimum Enclosing Sphere (三次元最小包含球)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/minimum_enclosing_sphere.hpp
---

有限な三次元点集合をすべて含む、半径最小の閉球を求める。1〜4 点のオーバーロードは支持球を定数時間で求め、`std::vector` のオーバーロードはそれらを使う乱択増分法を実行する。

このヘッダは 1 点・2 点・3 点・4 点・点列を引数とする個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。必要な引数の種類だけを使う場合は、`minimum_enclosing_sphere_one_point.hpp`、`minimum_enclosing_sphere_two_points.hpp`、`minimum_enclosing_sphere_three_points.hpp`、`minimum_enclosing_sphere_four_points.hpp`、`minimum_enclosing_sphere_points.hpp` のうち対応するものを直接 include すればよい。

## API

いずれも戻り値は `Sphere3` である。

- `minimum_enclosing_sphere(point)`: 1 点の最小包含球（半径 0）を返す。
- `minimum_enclosing_sphere(a, b)`: 2 点の最小包含球を返す。
- `minimum_enclosing_sphere(a, b, c)`: 3 点の最小包含球を返す。
- `minimum_enclosing_sphere(a, b, c, d)`: 4 点の最小包含球を返す。
- `minimum_enclosing_sphere(points, seed = 0x9E3779B97F4A7C15ULL)`: `std::vector<Point3>` 全体の最小包含球を返す。`seed` は処理順のシャッフル（`std::mt19937_64` と `std::shuffle`）を再現するために使う。

1〜4 点のオーバーロードは、1〜4 点からなるすべての部分集合が定める有限な候補球を列挙し、入力をすべて含む最小の候補を選ぶ。このため、重複・共線・共面・鈍角三角形を個別の分岐なしで扱える。

## API 別の時間計算量・空間計算量

- 1〜4 点のオーバーロード: 時間・追加空間ともに $O(1)$。
- 点列のオーバーロード: シャッフル後の期待時間 $O(N)$、最悪時間 $O(N^4)$。入力のコピーを含む追加空間 $O(N)$。

## 注意点

- 空集合に対しては有限で一意な最小球が定まらないため、`std::invalid_argument` を送出する。
- 座標は有限でなければならない。違反時は `std::invalid_argument` を送出する。
- 途中の値がオーバーフローして有限な候補を構成できない場合は `std::overflow_error` を送出する。
- 点の差を局所スケールで正規化してから候補球を構成する。したがって、符号が反対の巨大な座標同士の差が直接表現できない場合や、非常に小さい点集合も扱える。真の中心または半径が有限でも `long double` で表現できない場合は `std::overflow_error` を送出する。
- 球に含まれるかの判定の許容誤差は、球と点の距離の局所スケールに対する相対誤差である。絶対誤差によって微小な点集合を半径 0 とみなすことはない。
- `std::shuffle` の結果は標準ライブラリの実装に依存するため、同じ `seed` による再現性は同一の実装内に限られる。
