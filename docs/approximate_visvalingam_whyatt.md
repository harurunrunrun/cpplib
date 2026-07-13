---
title: Visvalingam–Whyatt Algorithm (Visvalingam–Whyatt法)
documentation_of: ../src/approximate/geometry/visvalingam_whyatt.hpp
---

# Visvalingam–Whyatt法

隣接する3点が作る三角形の面積が最小の内点を順に除き、2次元折れ線を指定頂点数まで簡略化する。両端点は削除しない。

## `visvalingam_whyatt_indices(polyline, target_size)`

残す頂点の添字を入力順で返す。2点以上の折れ線では `2 <= target_size <= N` が必要である。同面積の場合は添字が小さい点を先に除く。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$
- 誤差: 頂点数だけを制約し、元の折れ線からの最大距離は保証しない。自己交差や位相も保存しない。

## `visvalingam_whyatt(polyline, target_size)`

残した頂点そのものを返す。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$（返り値を含む）

非有限座標、および面積計算が有限範囲を超える入力は受け付けない。重複点と共線点の有効面積は0となる。
