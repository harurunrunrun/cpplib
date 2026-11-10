---
title: Minkowski Sum 3D (3次元Minkowski和)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/minkowski_sum_3d.hpp
---

2 つの凸形状の Minkowski 和を `ConvexPolyhedron3` として構築する。

このヘッダはデフォルト版・シード指定版・直接構築版の 3 つの個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。必要な経路だけを使う場合は `minkowski_sum_3d_default.hpp`、`minkowski_sum_3d_with_seed.hpp`、`minkowski_sum_3d_direct.hpp` のいずれかを直接 include すればよい。

## API

```cpp
ConvexPolyhedron3 minkowski_sum_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
);

ConvexPolyhedron3 minkowski_sum_3d_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    std::uint64_t random_seed
);

ConvexPolyhedron3 minkowski_sum_3d_direct(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double general_position_margin = 1.0e-9L
);
```

- `minkowski_sum_3d`: 両入力が `affine_dimension == 3` で、頂点と面をそれぞれ 4 個以上持つ場合は、まず `minkowski_sum_3d_direct` と同じ直接構築（`general_position_margin = 1.0e-9L`）を試みる。直接構築が一般位置の条件を満たさず `std::domain_error` となった場合、および入力がこの条件を満たさない場合は、すべての頂点対の和 $a+b$ を候補点として列挙し、その凸包を返す。候補点が 256 個以下なら検証付きの quickhull を試し、それ以外（または quickhull が失敗した場合）は決定的な `convex_hull_3d` を使う。結果は実行ごとに再現可能である。
- `minkowski_sum_3d_with_seed`: 両入力が上の条件を満たす場合は `minkowski_sum_3d` と同じ処理を行い、`random_seed` は使わない。それ以外の場合は、同じ候補点集合の凸包を、増分順序を `random_seed` で指定した `convex_hull_3d_with_seed` で構築する。
- `minkowski_sum_3d_direct`: 共面な三角形を 1 つの facet に統合し、第 1 項の facet と第 2 項の支持頂点、第 2 項の facet と第 1 項の支持頂点、および両者の幾何的な辺の法線錐が横断的に交差してできる四角形を直接つなぐ。全頂点対の候補点集合や、その三次元凸包は構築しない。

## API 別の時間計算量・空間計算量

頂点数を $V_1, V_2$、候補数を $P = V_1 V_2$、出力サイズを $K$ とする。厳密な述語に使う多倍長整数のビット計算量は除く。

直接版では、三角形面の数を $T_i$、統合後の facet 数と幾何的な辺の数を $F_i, E_i$ とする。三角形分割の内部にある共面な対角線は $E_i$ に含めない。また、入力サイズを $N_i = V_i + T_i + E_i + F_i$ とする。

- `minkowski_sum_3d_direct`: facet の統合が $O(V_1 + V_2 + T_1 + T_2)$、支持頂点の探索と法線 fan の重ね合わせが $O(F_1 V_2 + F_2 V_1 + E_1 E_2)$、出力の構築が期待 $O(K)$。したがって全体の期待時間は $O(T_1 + T_2 + F_1 V_2 + F_2 V_1 + E_1 E_2 + K) = O(N_1 N_2 + K)$、追加空間は $O(V_1 + V_2 + T_1 + T_2 + K)$ である。出力構築に使うハッシュテーブルで衝突が集中した場合、最悪時間は線形にならない。
- `minkowski_sum_3d`: 直接構築が成功すれば上と同じ。候補点の経路では、候補点の生成が時間・空間ともに $O(P)$、決定的な凸包が最悪時間 $O(P \log P)$・追加空間 $O(P)$ であり、三次元の入力では直接構築を試みた分（期待 $O(N_1 N_2)$）が加わる。
- `minkowski_sum_3d_with_seed`: 三次元の入力では `minkowski_sum_3d` と同じ。それ以外では、シードを入力と独立に選べば期待時間 $O(P \log P + K)$、期待追加空間 $O(P + K)$。固定シードに対する最悪時間は $O(P^2 \log P + K)$、最悪追加空間は $O(P^2 + K)$。

## 注意点

- 両入力は空でない凸形状でなければならない。空の入力には `std::invalid_argument` を送出する。
- 候補数が `std::vector<Point3>::max_size()` を超える場合は `std::length_error` を送出する。
- 非有限な座標には、凸包の構築時に `std::invalid_argument` を送出する。
- 重複点と退化した次元は凸包側で処理する。
- `minkowski_sum_3d_direct` は、両入力が三次元の内部を持ち、閉じた凸多面体の境界を外向きで整合した三角形で表したものだけを受け付ける。
- 直接版では、相手の各 facet の法線がただ 1 つの頂点を支持すること、異なる入力の幾何的な辺の方向のすべての組についてなす角の正弦の二乗が `general_position_margin` より大きいこと、交差する法線錐が境界から同じマージン以上離れていることが必要である。これは高速版の一般位置の契約であり、自己との和や平行な箱同士などは通常この契約を満たさない。
- `general_position_margin` は開区間 $(0, 0.01)$ 内の有限値でなければならない。位相・向き・数値マージンの検査に失敗すると `std::domain_error`、非有限な入力や次元の違いには `std::invalid_argument`、座標の和のオーバーフローには `std::overflow_error` を送出する。直接版は、暗黙に候補点による版へフォールバックしない。
- 一般位置を保証できない入力では `minkowski_sum_3d` または `minkowski_sum_3d_with_seed` を使う。
