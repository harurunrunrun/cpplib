---
title: Partially Persistent Rectangle Sum (部分永続長方形和)
documentation_of: ../../../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_rectangle_sum.hpp
---

`PartiallyPersistentWaveletMatrix2DWeighted` の矩形和向け派生名。
`size/versions/latest_version/x/y/weight/set/set_y/set_weight/`
`rectangle_count/range_freq/rectangle_sum/range_sum` をそのまま利用できる。

## 時間計算量

$D=\mathtt{Y\_BIT\_WIDTH}$、$H=O(\log(N+1))$。

- 構築: $O(\mathtt{MAX\_SIZE}+N\log N+DN)$
- 定数情報と `x`: $O(1)$、`weight`: $O(H)$、`y`: $O(DH)$
- 最新版への更新: $O(DH)$
- 任意versionの半開矩形count/sum: $O(\log N+DH)$

## 注意点

更新元は最新版だけで、過去versionは不変。型・範囲・容量・rollbackの契約は基底型と同じ。
固定ブロック幅のtemplate引数は存在しない。
