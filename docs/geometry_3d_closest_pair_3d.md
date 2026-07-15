---
title: 3D Closest Pair (三次元最近点対)
documentation_of: ../src/algorithm/geometry/3d/closest_pair_3d.hpp
---

三次元点集合からEuclidean距離が最小の異なるindex対を求める。全点対列挙ではなく、
最大spread軸で再帰的に分割した平衡kd-treeとbounding box枝刈りを使う。

## API

- `closest_pair_3d(points)`: `ClosestPair3DResult{first, second, distance}` を返す。
  `first < second` であり、同距離ならindex対が辞書順最小のものを返す。点が2個未満
  なら `nullopt`。

完全に同一の座標は事前sortで検出し、距離0を直ちに返す。

## API別の時間計算量・空間計算量

kd-tree構築は $O(N\log N)$ 時間・$O(N)$ 領域。最近傍queryを全点に行う平均的な
実行時間は $O(N\log N)$、枝刈りが効かない退化配置での最悪時間は $O(N^2)$。

## 注意点

- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 距離は3引数 `std::hypot` で計算し、平方距離のoverflowを避ける。
- bounding box枝刈りには丸め誤差分の保守的な余白を設ける。
