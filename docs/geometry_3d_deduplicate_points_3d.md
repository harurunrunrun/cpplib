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

## API別の時間計算量・空間計算量

入力点数を $N$、残る点数を $U$ とする。`long double` の表現幅を定数とみなす。

- 両方の許容誤差が $0$ の場合、座標を正確にソートして時間 $O(N\log N)$、
  戻り値を含む追加領域 $O(N)$。
- 相対許容誤差が $0$、絶対許容誤差が正の場合、幅が絶対許容誤差の三次元セルへ点を
  格納し、隣接する $27$ セルだけを調べる。ordered map を用いるため期待・最悪とも
  時間 $O(N\log U)$、戻り値を含む追加領域 $O(U)$。
- 相対許容誤差が正の場合は、先行する代表点を順に調べるため、期待・最悪とも
  時間 $O(NU)$、戻り値を含む追加領域 $O(U)$。

セル番号は `long double` を exact dyadic として多倍長整数へ変換して求める。
したがって、座標と絶対許容誤差の比が組み込み整数や浮動小数点数の範囲を超えても
セルが衝突しない。多倍長整数のビット演算量を明示する場合は、上記にセル番号の
ビット長に応じた演算コストを掛ける。

## 注意点

全座標は有限、許容誤差は有限かつ非負でなければならず、違反時は
`std::invalid_argument` を送出する。誤差付きの一致関係は推移的とは限らないため、
結果は入力順に依存する。相対許容誤差が正の場合は許容幅が座標ごとに変わるので、
固定幅セルによる高速化は適用しない。組合せ構造を正確な入力座標だけで構築したい場合は、
両方の許容誤差を $0$ にする。
