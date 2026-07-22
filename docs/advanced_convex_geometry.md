---
title: Advanced Convex Geometry (高度な凸幾何)
documentation_of: ../src/algorithm/geometry/2d/advanced_convex_geometry.hpp
---

geometry.hpp の Point と Line を用いる高度な凸幾何である。

    #include "src/algorithm/geometry/2d/advanced_convex_geometry.hpp"

## 凸多角形の正規化

    std::vector<Point> normalize_convex_polygon(std::vector<Point> polygon);

頂点を周回順に与える。時計回り・反時計回りの両方を受理し、返り値は反時計回り、
辞書順最小頂点始まりになる。次を正規化する。

- 連続する重複頂点と、末尾に重ねて書かれた先頭頂点を除く。
- 辺上の中間 collinear 頂点を除く。
- 全点が同一直線なら両端だけ、全点が同一点なら1点にする。
- 空列は空列のまま返す。

入力は弱凸な周回列でなければならない。左右の折れ曲がりが混在する場合は
std::invalid_argument を送出する。任意順の点集合から凸包を作る用途には
geometry.hpp の convex_hull を使う。

時間計算量・空間計算量（追加領域）はともに $O(N)$。

## 半平面

有向直線 Line{a,b} は

    cross(b - a, p - a) >= 0

を満たす左側の閉半平面を表す。

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

    HalfPlaneIntersectionResult
    half_plane_intersection(std::vector<Line> half_planes);

境界線の両端が同じなら std::invalid_argument を送出する。空の半平面列は全平面を
表すため HALF_PLANE_UNBOUNDED になる。

- HALF_PLANE_EMPTY: 共通部分がない。
- HALF_PLANE_BOUNDED: polygon が共通部分を反時計回り・辞書順最小点始まりで表す。
- HALF_PLANE_UNBOUNDED: 共通部分は存在するが有界でない。polygon は空である。

有界な共通部分が1点または線分へ退化した場合も HALF_PLANE_BOUNDED であり、
polygon.size() はそれぞれ1または2となる。

同方向に平行な境界は最も狭い半平面だけを残す。反対方向に平行な境界は、
両立すれば strip（ほかの制約で閉じなければ unbounded）、両立しなければ empty
として扱う。境界を含むため、接するだけの共通部分も消えない。

実装は角度 sort と deque による半平面交差である。有界性の判定には入力座標から
決めた十分大きい一時 bounding box を使い、最終境界に一時辺が残るかを調べる。
中間値を有限に保てない極端な座標には std::overflow_error を送出する。

$K$ 本に対して時間 $O(Klog K)$、空間計算量（追加領域） $O(K)$。
half_plane_contains は時間・領域ともに $O(1)$。

## Minkowski 和

    std::vector<Point> convex_polygon_minkowski_sum(
        std::vector<Point> first,
        std::vector<Point> second
    );

凸集合

    { a + b | a in first, b in second }

の頂点列を返す。入力には normalize_convex_polygon と同じ弱凸列を指定できる。
空集合との和は空、点との和は平行移動、線分同士の和は点・線分・平行四辺形の
いずれかになる。返り値は反時計回りで辞書順最小頂点から始まり、重複頂点と
中間 collinear 頂点を持たない。

正規化を含めて時間 $O(N+M)$、空間計算量（追加領域） $O(N+M)$。

## 凸多角形の共通部分

    std::vector<Point> convex_polygon_intersection(
        std::vector<Point> first,
        std::vector<Point> second
    );

2つの閉凸集合の共通部分を返す。入力と出力の規約は
normalize_convex_polygon と同じである。

- 共通部分なし: 空列
- 1点だけで接触: 1点
- 辺が重なる、または線分へ退化: 両端2点
- 面積を持つ: 反時計回りの凸多角形

点・線分入力は専用の包含判定・parametric clipping で処理する。面積を持つ
2多角形では、正規化後に巡回順となっている2本の辺方向列を線形マージし、
角度順の半平面をdequeで一度ずつ処理する。

点・線分への退化を含め、時間 $O(N+M)$、空間計算量（追加領域）
$O(N+M)$。

## EPS と大座標

基準値は既存 API と同じ GEOMETRY_EPS = 1e-10L だが、積を直接絶対 EPS と
比較しない。

- 辺方向の平行・collinear 判定は
  EPS * max(1, |u| |v|) を許容幅とする。これはおおむね角度に対する相対判定である。
- 半平面の内外判定は、境界方向の長さを掛けた絶対距離 EPS と、long double
  の machine epsilon から見積もった積の丸め誤差を使う。遠くの点という理由だけで
  境界から大きく外れた点を内側にしない。
- 重複点判定は座標差の絶対 EPS に、座標の大きさに応じた machine-roundoff を加える。

平行とみなされたほぼ平行な境界は同一方向または反対方向として処理される。
この判定幅より小さい角度差を区別したい場合は、用途に合わせた exact predicate
版を使うこと。

入力座標は有限で、加減算・外積・交点および Minkowski 和の中間値も
long double の有限範囲に収まる必要がある。$10^{12}$ 程度の平行移動を含む
ケースをテストしている。整数座標で組合せ的な判定を完全に厳密にしたい場合は、
long long の差と __int128 の外積を使う整数専用実装を選ぶ。

## テスト

advanced_convex_geometry.test.cpp は独立 generator/checker の実 input/output を使い、
次を検査する。

- hand-written な平行・反平行、empty/bounded/unbounded、点・線分退化
- exact rational naive oracle による random Minkowski 和・共通部分
- 冗長な同方向半平面を含む random bounded 半平面交差
- random な反対向き平行境界による feasible strip と contradiction
- 2万頂点級の large convex polygon と $10^{12}$ 座標

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。
