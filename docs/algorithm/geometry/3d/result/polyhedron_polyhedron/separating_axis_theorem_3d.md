---
title: Separating Axis Theorem 3D (3次元分離軸定理)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_polyhedron/separating_axis_theorem_3d.hpp
---

分離軸定理により、2 つの三次元凸多面体の分離・重なりを判定する。

## API

- `separating_axis_theorem_3d(first, second, tolerance = 1e-12L)`: 両形状の面法線と、すべての辺方向の組の外積を候補軸として検査し、`SATResult3` を返す。結果は交差の有無 `intersects`、分離距離 `separation`（分離時、最大の隙間）、貫通深度 `penetration_depth`（交差時、最小の重なり）、その軸 `axis`、検査した軸の数 `tested_axes` を持つ。

## API別の時間計算量・空間計算量

面数を $F_i$、辺数を $E_i$、頂点数を $V_i$、候補軸の数を $C = F_1 + F_2 + E_1 E_2$、重複除去後の軸の数を $A$ とする。

- 球面グリッドのハッシュ操作を期待 $O(1)$ とすると、期待時間は $O(C + A(V_1+V_2))$（辺の列挙に $O(F_i \log F_i)$ が加わる）、空間は $O(A + V_1 + V_2 + F_1 + F_2)$（入力を正規化したコピーを含む）。
- ハッシュテーブルの衝突を含めた決定的な最悪時間は $O(CA + A(V_1+V_2))$ である。

## 注意点

- 三角形面が外向きである必要はないが、両入力は面と辺を持つ三次元凸多面体でなければならない。空の入力、低次元（`affine_dimension != 3`）の入力、非正・非有限の `tolerance`、非有限な頂点には `std::invalid_argument` を、範囲外の面添字には `std::out_of_range` を、非零の候補軸が得られない場合や非多様体の辺には `std::domain_error` を送出する。
- すべての候補軸の生成と射影は、共通の平行移動を除去し全頂点の広がりで無次元化した座標で行う。`tolerance` はその座標での相対許容誤差であり、一様な拡大縮小と共通の平行移動に依存しない。
- 重複除去には幅 `tolerance` の球面グリッドを用い、候補の近傍セルと反対向きの近傍セルだけを調べたうえで、最後は外積の長さによる平行判定を行う。このため、セル境界や反平行な軸でも許容誤差の意味は変わらない。同一セルに残る非平行な単位軸の数は、球面上の詰め込みの議論により期待定数個である。
- `separation` と `penetration_depth` は入力の座標系に戻して返す。真の値が `long double` で表現できない場合は `std::overflow_error` を送出する。結果が定義される場合の `axis` は有限な単位ベクトルである。
