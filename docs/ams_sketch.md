---
title: AMS Sketch (AMSスケッチ)
documentation_of: ../src/approximate/streaming/ams_sketch.hpp
---

turnstile stream の第 2 周波数モーメント $F_2=\sum_x f_x^2$ を推定する。
各標本は 4-wise independent な符号和の二乗、group 内は平均、group 間は median を使う。

- `AMSSketch(seed)`: 符号 hash の係数を生成する。$O(Groups\,SamplesPerGroup)$。
- `add(key, delta)`: 頻度更新を全標本へ反映する。$O(Groups\,SamplesPerGroup)$。
- `estimate_second_moment()`: group 平均の median。$O(Groups\,SamplesPerGroup+Groups\log Groups)$。
- `clear()`: 符号和を 0 にする。$O(Groups\,SamplesPerGroup)$。
- `groups()`, `samples_per_group()`: $O(1)$。

4-wise independent 符号の理論では単一標本は不偏で分散が高々 $2F_2^2$。
group 平均に Chebyshev、group median に Chernoff bound を適用でき、
`SamplesPerGroup = O(1/epsilon^2)`, `Groups = O(log(1/delta))` で
相対誤差 `epsilon`、失敗確率 `delta`。保存領域は
$O(Groups\,SamplesPerGroup)$。
