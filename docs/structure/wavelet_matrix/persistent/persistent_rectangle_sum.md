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

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE`, `MAX_VERSION` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = PersistentRectangleSum<X, Y, W>;
using Fixed = PersistentRectangleSum<X, Y, W, N, Versions>;
```

可変長版は必要なノード・版・履歴を明示的に追加し、保存要素数を $K$ として追加領域は $O(K)$。
再確保の費用は追加操作全体で償却され、上記各操作の計算量を維持する。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
