---
title: Quaternion between Two Vectors (二ベクトル間の最短回転)
documentation_of: ../../../../../../src/algorithm/geometry/3d/quaternion/point_point/quaternion3_from_two_vectors.hpp
---

## API

- `quaternion3_from_two_vectors(from, to)`: `from` の向きを `to` の向きへ移す最短回転を表す単位 `Quaternion3` を返す。

## API別の時間計算量・空間計算量

時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 両ベクトルは有限な非零ベクトルでなければならず、違反時は `std::invalid_argument` を送出する。ベクトルの長さは無視する。
- ほぼ同じ向き（なす角の余弦が $1 - 8\varepsilon$ 以上、$\varepsilon$ は `long double` の機械イプシロン）なら恒等回転を返す。
- ほぼ反対向きなら回転軸が一意に定まらないため、`from` の成分のうち絶対値が最小の座標軸方向の単位ベクトルと `from` の外積を軸として、決定的に $\pi$ 回転を返す。
