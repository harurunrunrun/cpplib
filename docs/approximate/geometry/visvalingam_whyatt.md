---
title: Visvalingam–Whyatt Algorithm (Visvalingam–Whyatt法)
documentation_of: ../../../src/approximate/geometry/visvalingam_whyatt.hpp
---

隣接する 3 点が作る三角形の面積が最小の内点を順に除き、2 次元の折れ線を指定した頂点数まで簡略化する。両端の点は削除しない。以下、頂点数を $N$ とする。

## `visvalingam_whyatt_indices(polyline, target_size)`

残す頂点の添字を入力順で返す。2 点以上の折れ線では `2 <= target_size <= N` でなければならない。面積が等しい場合は添字が小さい点を先に除く。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$

## `visvalingam_whyatt(polyline, target_size)`

残した頂点そのものを返す。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$（戻り値を含む）

## 注意点

- `target_size > N`、または 2 点以上の折れ線で `target_size < 2` の場合は `std::invalid_argument` を送出する。
- 非有限の座標や、面積の計算が有限の範囲を超える入力は受け付けない。重複点や共線な点の有効面積は 0 とする。
- 制約するのは頂点数だけであり、元の折れ線からの最大距離は保証しない。自己交差の有無や位相も保存しない。
