---
title: Quaternion SLERP (クォータニオン球面線形補間)
documentation_of: ../../../../../../src/algorithm/geometry/3d/quaternion/quaternion_quaternion/quaternion3_slerp.hpp
---

## API

- `quaternion3_slerp(first, second, parameter)`: 2 つの回転の間の最短弧を `parameter` で球面線形補間した単位クォータニオンを返す。`parameter` が $0$ なら `first`、$1$ なら `second` の表す回転になる。

## API別の時間計算量・空間計算量

時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 両入力は有限な非零クォータニオン、`parameter` は $[0, 1]$ の有限値でなければならず、違反時は `std::invalid_argument` を送出する。入力は内部で正規化する。
- 内積が負なら終点の符号を反転し、回転空間上で短い方の経路を選ぶ。
- ほぼ同じ回転（内積が $1 - 32\varepsilon$ を超える）では正規化線形補間に切り替える。
