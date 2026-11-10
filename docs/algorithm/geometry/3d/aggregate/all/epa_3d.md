---
title: Expanding Polytope Algorithm 3D (3次元拡張ポリトープ法)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/epa_3d.hpp
---

## API

```cpp
EPAResult3 epa_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    const GJKResult3& gjk_result,
    long double tolerance = 1.0e-10L,
    std::size_t max_iterations = 128
);
EPAResult3 epa_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-10L,
    std::size_t max_iterations = 128
);
```

- `epa_3d(first, second, gjk_result, tolerance, max_iterations)`: GJK が終了したときの単体から Minkowski 差の境界を拡張し、貫通深度・接触法線・接触点を返す。
- `epa_3d(first, second, tolerance, max_iterations)`: 内部で GJK を実行する簡便版。

この集約ヘッダは、形状の対だけを受け取るオーバーロードと、GJK の結果も受け取るオーバーロードの両方の個別ヘッダを提供する。

## API 別の時間計算量・空間計算量

- `epa_3d(first, second, gjk_result, ...)`: support 写像の 1 回の評価は $O(V_1 + V_2)$。EPA が $I_E$ 回反復し、最大 $F$ 面を持つとき、時間 $O(I_E(V_1 + V_2 + F))$、空間 $O(I_E + F)$。初期単体が退化している場合の構築は、定数回の support 評価と高々定数個の 4 点組の検査で行う。
- `epa_3d(first, second, ...)`: 上記に GJK の $I_G$ 回の反復を加え、時間 $O(I_G(V_1 + V_2) + I_E(V_1 + V_2 + F))$、空間 $O(I_E + F)$。
- 初期ポリトープを作れず SAT にフォールバックする場合は、SAT の面数・辺数に依存する計算量が別途かかる。

## 注意点

- GJK で交差しないと判定されれば `intersects = false` を返す。
- 面同士・辺同士の接触など、正の体積を持つ初期単体を作れない場合は、同じ候補軸を備えた SAT で深さと法線を確定する。
- 許容誤差が正の有限値でない場合や、反復回数の上限が 0 の場合は `std::invalid_argument` を送出する。
- EPA の貫通深度は、三次元の内部を持つ凸多面体同士に対して定義する。どちらかが `affine_dimension != 3` なら、両オーバーロードとも `std::invalid_argument` を送出する。
- 両オーバーロードとも、共通の平行移動を除去し、全頂点の広がりで無次元化した同一の内部処理を用いる。`gjk_result` を受け取るオーバーロードは、support 点を同じ変換で正規化してから利用する。単体が空なら正規化座標上で GJK を補って実行し、非有限な support 点があれば `std::invalid_argument` を送出する。
- `tolerance` は形状の広がりに対する相対許容誤差である。反復回数の上限に達した場合は、その時点で最良の面を返し、`converged = false` とする。
- 貫通深度と接触点は入力の座標系に戻して返す。真の値が `long double` で表現できなければ `std::overflow_error` を送出する。交差する場合の `normal` は有限な単位ベクトルである。
