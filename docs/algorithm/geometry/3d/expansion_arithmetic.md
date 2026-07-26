---
title: Floating-Point Expansion Arithmetic (浮動小数点展開算術)
documentation_of: ../../../../src/algorithm/geometry/3d/expansion_arithmetic.hpp
---

浮動小数点展開型と全演算leafをまとめて読み込む互換aggregatorです。

## API

- `Geometry3DExpansion`: 展開の成分列です。
- `geometry3d_two_sum`, `geometry3d_two_diff`, `geometry3d_two_product`: 2スカラーを展開へ変換します。
- `geometry3d_expansion_sum`, `geometry3d_expansion_product`: 2展開の二項演算です。
- `geometry3d_expansion_negate`, `geometry3d_expansion_scale`: 符号反転とスカラー倍です。
- `geometry3d_expansion_estimate`, `geometry3d_expansion_sign`: 近似値と符号を返します。

## API別の時間計算量・空間計算量

成分数を $N,M$ とすると、2スカラー演算は $O(1)$、`negate`、`estimate`、`sign` は $O(N)$、`sum` は $O(M(N+M))$、`scale` は $O(N^2)$、`product` は最悪 $O(N^2M^2)$ です。返却展開を除く空間は生成される中間展開の大きさに比例します。

## 注意点

従来の一括include用headerです。必要なAPIだけを使う場合は対応するleafを直接includeできます。入力は有限値かつ絶対値の小さい成分から並ぶ展開を前提とします。