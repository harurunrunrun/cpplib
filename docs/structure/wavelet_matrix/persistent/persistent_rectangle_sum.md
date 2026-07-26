---
title: Persistent Rectangle Sum (完全永続長方形和)
documentation_of: ../../../../src/structure/wavelet_matrix/persistent/persistent_rectangle_sum.hpp
---

`PersistentWaveletMatrix2DWeighted` の矩形和向け派生名。
`size/versions/latest_version/x/y/weight/set/set_y/set_weight/fork/`
`rectangle_count/range_freq/rectangle_sum/range_sum` をそのまま利用できる。

## 時間計算量

$D=\mathtt{Y\_BIT\_WIDTH}$、$H=O(\log(N+1))$。

- 構築: $O(\mathtt{MAX\_SIZE}+N\log N+DN)$
- 定数情報と `x`: $O(1)$、`weight`: $O(H)$、`y`: $O(DH)$
- 更新: $O(DH)$、`fork`: $O(D)$
- 半開矩形のcount/sum: $O(\log N+DH)$

## 注意点

任意versionから分岐し、既存versionは不変。型・範囲・容量・例外時rollbackの契約は基底型と同じ。
固定ブロック幅のtemplate引数は存在しない。
