---
title: Advanced Convex Geometry (高度な凸幾何)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/advanced_convex_geometry.hpp
---

`geometry.hpp` の `Point` と `Line` を用いる高度な凸幾何の API をまとめて読み込む集約ヘッダである。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/aggregate/all/advanced_convex_geometry.hpp"
```

## 構成

| leaf header | 提供する API |
| --- | --- |
| `normalize_convex_polygon.hpp` | `normalize_convex_polygon` |
| `half_plane_contains.hpp` | `half_plane_contains` |
| `half_plane_intersection.hpp` | `half_plane_intersection` |
| `convex_polygon_minkowski_sum.hpp` | `convex_polygon_minkowski_sum` |
| `convex_polygon_minkowski_difference.hpp` | `convex_polygon_minkowski_difference` |
| `convex_polygon_intersection.hpp` | `convex_polygon_intersection` |

## 凸多角形の正規化

```cpp
std::vector<Point> normalize_convex_polygon(std::vector<Point> polygon);
```

頂点を周回順に与える。時計回り・反時計回りのどちらも受け付け、返り値は反時計回りで、辞書順最小の頂点から始まる。次の正規化を行う。

- 連続する重複頂点と、末尾に重ねて書かれた先頭頂点を除く。
- 辺上にある同一直線上の中間頂点を除く。
- 全点が同一直線上にあれば両端の 2 点だけ、全点が同一点なら 1 点にする。
- 空列は空列のまま返す。

入力は弱凸な周回列でなければならない。左右の折れ曲がりが混在する場合、または境界が 2 周以上巻いている場合は `std::invalid_argument` を送出する。任意順の点集合から凸包を作るには `geometry.hpp` の `convex_hull` を使う。

時間計算量・追加の空間計算量はともに $O(N)$ である。

## 半平面

有向直線 `Line{a, b}` は、$\operatorname{cross}(b - a, p - a) \ge 0$ を満たす左側の閉半平面を表す。

```cpp
bool half_plane_contains(const Line& half_plane, const Point& point);

enum HalfPlaneIntersectionKind {
    HALF_PLANE_EMPTY,
    HALF_PLANE_BOUNDED,
    HALF_PLANE_UNBOUNDED,
};

struct HalfPlaneIntersectionResult {
    HalfPlaneIntersectionKind kind;
    std::vector<Point> polygon;
};

HalfPlaneIntersectionResult half_plane_intersection(std::vector<Line> half_planes);
```

境界線の 2 点が同じなら `std::invalid_argument` を送出する。空の半平面列は全平面を表すので `HALF_PLANE_UNBOUNDED` になる。

- `HALF_PLANE_EMPTY`: 共通部分がない。
- `HALF_PLANE_BOUNDED`: `polygon` が共通部分を、反時計回り・辞書順最小の点から始まる頂点列で表す。
- `HALF_PLANE_UNBOUNDED`: 共通部分は存在するが有界でない。`polygon` は空である。

有界な共通部分が 1 点または線分に退化した場合も `HALF_PLANE_BOUNDED` であり、`polygon.size()` はそれぞれ $1$ または $2$ になる。

同じ向きに平行な境界は最も狭い半平面だけを残す。逆向きに平行な境界は、両立すれば帯（ほかの制約で閉じなければ非有界）、両立しなければ空として扱う。境界を含むので、接するだけの共通部分も消えない。

実装は偏角ソートと両端キューによる半平面交差である。有界性の判定には、入力座標から決めた十分大きい一時的なバウンディングボックスを使い、最終的な境界に一時的な辺が残るかを調べる。座標が非有限なら `std::invalid_argument` を、中間値を有限に保てないほど極端な座標なら `std::overflow_error` を送出する。

$K$ 本の半平面に対して時間 $O(K \log K)$、追加の空間計算量 $O(K)$ である。`half_plane_contains` は時間・空間ともに $O(1)$ である。

## Minkowski 和・差

```cpp
std::vector<Point> convex_polygon_minkowski_sum(std::vector<Point> first, std::vector<Point> second);
std::vector<Point> convex_polygon_minkowski_difference(std::vector<Point> first, std::vector<Point> second);
```

`convex_polygon_minkowski_sum` は凸集合

$$
\{ a + b \mid a \in \mathrm{first},\ b \in \mathrm{second} \}
$$

の頂点列を返す。入力には `normalize_convex_polygon` と同じ弱凸列を指定できる。空集合との和は空、点との和は平行移動、線分どうしの和は点・線分・平行四辺形のいずれかになる。返り値は反時計回りで辞書順最小の頂点から始まり、重複頂点や同一直線上の中間頂点を持たない。

`convex_polygon_minkowski_difference` は `second` の各点の符号を反転してから和を取り、$\lbrace a - b \rbrace$ を返す。

正規化を含めて時間 $O(N + M)$、追加の空間計算量 $O(N + M)$ である。

## 凸多角形の共通部分

```cpp
std::vector<Point> convex_polygon_intersection(std::vector<Point> first, std::vector<Point> second);
```

2 つの閉凸集合の共通部分を返す。入力と出力の規約は `normalize_convex_polygon` と同じである。

- 共通部分がない: 空列
- 1 点だけで接する: 1 点
- 辺が重なる、または線分に退化する: 両端の 2 点
- 面積を持つ: 反時計回りの凸多角形

点・線分の入力は専用の包含判定とパラメータ表示によるクリッピングで処理する。面積を持つ 2 つの多角形では、正規化後に巡回順になっている 2 本の辺方向列を線形にマージし、偏角順に並んだ半平面を両端キューで 1 回ずつ処理する。

点・線分への退化を含めて、時間 $O(N + M)$、追加の空間計算量 $O(N + M)$ である。

## EPS と大きな座標

基準値は既存の API と同じ `GEOMETRY_EPS = 1e-10L` だが、積を絶対的な EPS と直接比較はしない。

- 辺方向の平行・同一直線判定は、$\varepsilon \cdot \max(1, \lvert u \rvert \lvert v \rvert)$ を許容幅とする。これはおおむね角度についての相対判定である。
- 半平面の内外判定には、境界方向の長さを掛けた距離の絶対 EPS と、`long double` の機械イプシロンから見積もった積の丸め誤差を使う。遠くの点だからという理由だけで、境界から大きく外れた点を内側とみなすことはない。
- 重複点の判定は、座標差の絶対 EPS に、座標の大きさに応じた丸め誤差を加えたもので行う。

平行とみなされたほぼ平行な境界は、同じ向きまたは逆向きとして処理される。この判定幅より小さい角度差を区別したい場合は、用途に合わせた厳密な述語の実装を使うこと。

入力座標は有限で、加減算・外積・交点や Minkowski 和の中間値も `long double` の有限範囲に収まる必要がある。$10^{12}$ 程度の平行移動を含むケースをテストしている。整数座標で組合せ的な判定を完全に厳密にしたい場合は、`long long` の差と `__int128` の外積を使う整数専用の実装を選ぶこと。

## テスト

`advanced_convex_geometry.test.cpp` は、独立した generator と checker による実際の入出力を使い、次を検査する。

- 手で書いた平行・逆平行、空・有界・非有界、点・線分への退化のケース
- 厳密な有理数演算による愚直解と比較する、ランダムな Minkowski 和・共通部分
- 冗長な同じ向きの半平面を含む、ランダムな有界半平面交差
- ランダムな逆向きの平行境界による、両立する帯と矛盾するケース
- 2 万頂点程度の大きな凸多角形と $10^{12}$ 規模の座標
