---
title: 3D Gram-Schmidt Orthonormalization (三次元Gram–Schmidt正規直交化)
documentation_of: ../src/algorithm/geometry/3d/gram_schmidt_3d.hpp
---

入力順を保つmodified Gram–Schmidt法で、三次元ベクトル列が張る部分空間の
正規直交基底を構築する。再直交化を1回行う。

## API

- `gram_schmidt_3d(vectors, relative_epsilon)`: 独立な方向だけを残した単位
  ベクトル列を返す。要素数は0〜3。

## 時間計算量

入力数を $N$ とすると $O(N)$ 時間、戻り値を除く追加領域 $O(1)$。

## 注意点

- ゼロまたは既存基底に数値的に従属する入力は除く。
- 非有限ベクトル、不正な許容誤差には `std::invalid_argument` を送出する。
