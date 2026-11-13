---
title: Partially Persistent Rectangle Sum (部分永続長方形和)
documentation_of: ../../../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_rectangle_sum.hpp
---

`PartiallyPersistentWaveletMatrix2DWeighted` を矩形和の用途の名前で公開する派生型。コンストラクタを含め、`size`, `versions`, `latest_version`, `x`, `y`, `weight`, `set`, `set_y`, `set_weight`, `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum` をそのまま利用できる。

## テンプレート引数

```cpp
PartiallyPersistentRectangleSum<X, Y, W, MAX_SIZE = -1, MAX_VERSION = -1, Y_BIT_WIDTH = (Y の bit 数)>
```

意味は `PartiallyPersistentWaveletMatrix2DWeighted` と同じである。固定ブロック幅のテンプレート引数は存在しない。

```cpp
using Variable = PartiallyPersistentRectangleSum<X, Y, W>;
using Fixed = PartiallyPersistentRectangleSum<X, Y, W, N, Versions>;
```

## 時間計算量

$N$ を点数、$D = \texttt{Y_BIT_WIDTH}$、$H = O(\log(N + 1))$ とする。

- 構築: $O(N \log N + DN)$（固定長版では配列の初期化に $O(\texttt{MAX_SIZE})$ が加わる）
- `size`, `versions`, `latest_version`, `x`: $O(1)$、`weight`: $O(H)$、`y`: $O(DH)$
- 最新版への更新: $O(DH)$
- 任意の版での半開矩形の個数・総和: $O(\log N + DH)$

## 注意点

- 更新元は最新版だけで、過去の版は変化しない。
- 型・範囲・容量・例外時のロールバックの規約は基底型と同じである。
