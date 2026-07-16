---
title: 3D Closest Pair (三次元最近点対)
documentation_of: ../src/algorithm/geometry/3d/closest_pair_3d.hpp
---

三次元点集合からEuclidean距離が最小の異なるindex対を求める。全点対列挙ではなく、
$x$ 座標で分ける分割統治と、中央 slab に対する $y$ 帯・$z$ 順の線形走査を使う。
各再帰段の結合処理は固定次元の packing bound により線形時間となる。

## API

- `closest_pair_3d(points)`: `ClosestPair3DResult{first, second, distance}` を返す。
  `first < second` であり、同距離ならindex対が辞書順最小のものを返す。点が2個未満
  なら `nullopt`。

完全に同一の座標は事前sortで検出し、距離0を直ちに返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `closest_pair_3d(points)` | 最悪 $O(N\log N)$ | $O(N)$ |

再帰の左右で得た距離を $d$ とすると、中央面から距離 $d$ 以下の点だけを調べる。
slab を高さ $d$ 以下の $y$ 帯へ分け、各帯と隣接帯を $z$ 順に走査する。左右それぞれの
点集合では任意の2点間距離が $d$ 以上なので、各 $O(d)\times O(d)\times O(d)$
領域に入る点数は定数であり、各再帰段の比較回数は $O(N)$ である。

## 注意点

- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 内部の距離比較は仮数と2進指数を分離して行う。有限座標同士の差やEuclidean距離が
  `long double` の最大値を越えても、比較と枝刈りはoverflowしない。
- 返す `distance` 自体を `long double` で表せない場合、その値は正の無限大となる。
- Euclidean norm の仮数計算は `std::hypot` を使い、平方和のoverflowを避ける。
