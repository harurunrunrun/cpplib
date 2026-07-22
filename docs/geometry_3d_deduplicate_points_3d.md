---
title: 3D Point Deduplication (三次元点の重複除去)
documentation_of: ../src/algorithm/geometry/3d/deduplicate_points_3d.hpp
---

## API

```cpp
vector<Point3> deduplicate_points_3d(
    const vector<Point3>& points,
    long double relative_tolerance = 0,
    long double absolute_tolerance = GEOMETRY3D_EPS
)
```

各座標を `relative_equal` で比較し、すでに残した代表点と三座標すべてが等しい点を
取り除く。各グループでは入力中で最初の点を残し、返り値の順序も入力順を保つ。

```cpp
vector<Point3> deduplicate_points_3d_relative_range_tree(
    const vector<Point3>& points,
    long double relative_tolerance,
    long double absolute_tolerance = GEOMETRY3D_EPS
)
```

相対許容誤差が正の場合に、同じ重複除去をオフライン三次元範囲木で高速に行う。
各軸で `relative_equal` が真になる座標順位区間を単調二分探索し、その直積に
すでに残した代表点があるかを判定する。入力順、残す代表点、浮動小数点の
オーバーフロー時を含む比較結果は上の API と同じである。

## API別の時間計算量・空間計算量

入力点数を $N$、残る点数を $U$ とする。`long double` の表現幅を定数とみなす。

- 両方の許容誤差が $0$ の場合、座標を正確にソートして時間 $O(N\log N)$、
  戻り値を含む追加領域 $O(N)$。
- 相対許容誤差が $0$、絶対許容誤差が正の場合、幅が絶対許容誤差の三次元セルへ点を
  格納し、隣接する $27$ セルだけを調べる。ordered map を用いるため期待・最悪とも
  時間 $O(N\log U)$、戻り値を含む追加領域 $O(U)$。
- 相対許容誤差が正の場合は、先行する代表点を順に調べるため、期待・最悪とも
  時間 $O(NU)$、戻り値を含む追加領域 $O(U)$。

- `deduplicate_points_3d_relative_range_tree` は、座標圧縮と三重 Fenwick 木の
  オフライン構築に時間 $O(N\log^3 N)$、追加領域 $O(N\log^2 N)$ を使う。
  各点について許容座標区間を $O(\log N)$ で求め、定数個の直方体の空判定と
  代表点の追加を $O(\log^3 N)$ で行うため、全体の最悪時間は
  $O(N\log^3 N)$ である。入力が大きく相対許容誤差が正のときはこちらを使う。

セル番号は `long double` を exact dyadic として多倍長整数へ変換して求める。
したがって、座標と絶対許容誤差の比が組み込み整数や浮動小数点数の範囲を超えても
セルが衝突しない。多倍長整数のビット演算量を明示する場合は、上記にセル番号の
ビット長に応じた演算コストを掛ける。

## 注意点

全座標は有限、許容誤差は有限かつ非負でなければならず、違反時は
`std::invalid_argument` を送出する。誤差付きの一致関係は推移的とは限らないため、
結果は入力順に依存する。相対許容誤差が正の場合は許容幅が座標ごとに変わるので、
固定幅セルによる高速化は適用しない。
`deduplicate_points_3d_relative_range_tree` の `relative_tolerance` は正で
なければならない。許容区間は丸めた境界式ではなく実際の `relative_equal`
を単調二分探索するため、境界値や相対上限の乗算がオーバーフローする値も同じ意味で
処理する。組合せ構造を正確な入力座標だけで構築したい場合は、両方の許容誤差を
$0$ にする。
