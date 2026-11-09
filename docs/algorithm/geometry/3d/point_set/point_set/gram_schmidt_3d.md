---
title: 3D Gram-Schmidt Orthonormalization (三次元Gram–Schmidt正規直交化)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/point_set/gram_schmidt_3d.hpp
---

入力順を保つ修正 Gram–Schmidt 法（modified Gram–Schmidt）で、三次元ベクトル列が張る部分空間の正規直交基底を構築する。各入力に対して再直交化を 1 回行う（射影の除去を 2 回繰り返す）。

## API

- `gram_schmidt_3d(vectors, relative_epsilon = GEOMETRY3D_EPS)`: 独立な方向だけを残した単位ベクトル列を返す。要素数は $0$ 以上 $3$ 以下である。

## API 別の時間計算量・空間計算量

入力数を $N$ とすると時間 $O(N)$、戻り値を除く追加領域 $O(1)$。

## 注意点

- ゼロベクトル、および既存の基底に数値的に従属する入力（最大絶対成分で正規化して射影を除いた残りの長さが `relative_epsilon` 以下のもの）は除く。
- 基底が 3 本そろった時点で走査を打ち切る。
- 非有限なベクトル、または負・非有限の `relative_epsilon` には `std::invalid_argument` を送出する。
