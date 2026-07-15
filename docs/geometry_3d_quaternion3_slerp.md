---
title: Quaternion SLERP (クォータニオン球面線形補間)
documentation_of: ../src/algorithm/geometry/3d/quaternion3_slerp.hpp
---

## API

- `quaternion3_slerp(first, second, parameter)`: 2回転間の最短弧を`parameter`で球面線形補間した単位クォータニオンを返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

両入力は有限な非零クォータニオン、`parameter`は有限な$[0,1]$でなければならない。終点の符号を必要に応じて反転し、回転空間上の短い経路を選ぶ。ほぼ同一の回転では正規化線形補間へ切り替える。
