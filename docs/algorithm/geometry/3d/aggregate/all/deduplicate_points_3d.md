---
title: 3D Point Deduplication (三次元点の重複除去)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/deduplicate_points_3d.hpp
---

このヘッダはデフォルト法と相対範囲木法の個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。新規コードでは必要な計算法の個別ヘッダだけを include すればよい。

## API

```cpp
std::vector<Point3> deduplicate_points_3d(
    const std::vector<Point3>& points,
    long double relative_tolerance = 0,
    long double absolute_tolerance = GEOMETRY3D_EPS
);
```

各座標を `relative_equal` で比較し、すでに残した代表点と 3 座標すべてが等しい点を取り除く。各グループでは入力中で最初の点を残し、戻り値の順序も入力順を保つ。

```cpp
std::vector<Point3> deduplicate_points_3d_relative_range_tree(
    const std::vector<Point3>& points,
    long double relative_tolerance,
    long double absolute_tolerance = GEOMETRY3D_EPS
);
```

相対許容誤差が正の場合に、同じ重複除去をオフラインの三次元範囲木で高速に行う。各軸で `relative_equal` が真になる座標順位の区間を単調性を利用した二分探索で求め、その直積の中にすでに残した代表点があるかを判定する。入力順、残す代表点、浮動小数点のオーバーフロー時を含む比較結果は、上の API と同じである。

## API 別の時間計算量・空間計算量

入力点数を $N$、残る点数を $U$ とする。`long double` の表現幅は定数とみなす。

`deduplicate_points_3d` は許容誤差に応じて次の 3 通りに分かれる。

- 両方の許容誤差が $0$ の場合: 座標を正確にソートし、時間 $O(N \log N)$、戻り値を含む追加空間 $O(N)$。
- 相対許容誤差が $0$ で絶対許容誤差が正の場合: 一辺が絶対許容誤差の三次元セルに点を格納し、隣接する $27$ セルだけを調べる。順序付き連想配列を用いるため、期待・最悪ともに時間 $O(N \log U)$、戻り値を含む追加空間 $O(U)$。
- 相対許容誤差が正の場合: 先に残した代表点を順に調べるため、期待・最悪ともに時間 $O(NU)$、戻り値を含む追加空間 $O(U)$。

`deduplicate_points_3d_relative_range_tree` は、座標圧縮と三重の Fenwick tree のオフライン構築に時間 $O(N \log^3 N)$、追加空間 $O(N \log^2 N)$ を使う。各点について許容される座標区間を $O(\log N)$ で求め、定数個の直方体の空判定と代表点の追加を $O(\log^3 N)$ で行うため、全体の最悪時間は $O(N \log^3 N)$ である。入力が大きく相対許容誤差が正のときはこちらを使う。

セル番号は、`long double` を正確な 2 進有理数として多倍長整数に変換して求める。したがって、座標と絶対許容誤差の比が組み込みの整数型や浮動小数点数の範囲を超えても、セルが衝突しない。多倍長整数のビット演算量まで考える場合は、上記にセル番号のビット長に応じた演算コストが掛かる。

## 注意点

- すべての座標は有限、許容誤差は有限かつ非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 誤差付きの一致関係は推移的とは限らないため、結果は入力順に依存する。
- 相対許容誤差が正の場合は許容幅が座標ごとに変わるため、固定幅のセルによる高速化は適用しない。
- `deduplicate_points_3d_relative_range_tree` の `relative_tolerance` は正でなければならない。許容区間は、丸めた境界式ではなく実際の `relative_equal` を単調性を利用して二分探索して求めるため、境界値や相対上限の乗算がオーバーフローする値も同じ意味で処理する。
- 組合せ構造を正確な入力座標だけで構築したい場合は、両方の許容誤差を $0$ にする。
