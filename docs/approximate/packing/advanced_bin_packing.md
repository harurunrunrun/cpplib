---
title: Advanced One-Dimensional Bin Packing (高度な一次元ビンパッキング)
documentation_of: ../../../src/approximate/packing/advanced_bin_packing.hpp
---

`approximate::packing` 名前空間で、発展的な 1 次元ビンパッキングの API を提供する。すべての関数は元のアイテムの添字に対応する `BinPackingResult` を返す。以下、アイテム数を $N$、生成したビンの数を $B$ とする。

## 降順版の簡易 API

```cpp
next_fit_decreasing_bin_packing(items, capacity);
first_fit_decreasing_bin_packing(items, capacity);
best_fit_decreasing_bin_packing(items, capacity);
worst_fit_decreasing_bin_packing(items, capacity);
```

既存の 4 手法（`bin_packing.hpp`）を、アイテムを降順に並べて実行する API である。時間計算量はいずれも $O(N\log N)$、空間計算量は $O(N)$ である。

## Almost Worst Fit

```cpp
almost_worst_fit_bin_packing(items, capacity, decreasing = false);
```

アイテムが入るビンのうち残容量が 2 番目に大きいビンを選び、入るビンが 1 個しかなければそれを選ぶ。時間計算量は $O(N\log N)$、空間計算量は $O(N)$ である。

## Harmonic 系

```cpp
harmonic_bin_packing(items, capacity, class_count = 10);
harmonic_fit_bin_packing(items, capacity, class_count = 10);
harmonic_first_fit_bin_packing(items, capacity, class_count = 10);
```

容量に対するサイズ比が $(1/(j+1),1/j]$ のアイテムをクラス $j$ とし、クラス $j$ は 1 つのビンに $j$ 個ずつ詰める。最小のクラスには、Harmonic / Harmonic Fit では Next Fit、Harmonic First Fit では First Fit を使う。`harmonic_fit_bin_packing` は `harmonic_bin_packing` の別名である。$K = \texttt{class_count}$ として、前の 2 つの時間計算量は $O(NK)$、Harmonic First Fit は最悪 $O(N^2+NK)$ である。空間計算量は $O(N+K)$ である。

## Refined First Fit / MFFD

```cpp
refined_first_fit_bin_packing(items, capacity);
modified_first_fit_decreasing_bin_packing(items, capacity);
mffd_bin_packing(items, capacity);
```

Refined First Fit は $1/2$, $2/5$ を境界とするサイズクラスと、6 個ごとに確保する中サイズ用のビンを使い、大きいアイテムとの組合せを制御する。MFFD はアイテムを降順に並べて、大（$>1/2$）、中・小（$>1/6$）、極小に分け、大のアイテムを入れたビンの残りを最もよく埋めるように先に詰めてから First Fit を行う。`mffd_bin_packing` は同じ処理の別名である。

時間計算量は RFF が $O(NB)$、MFFD が $O(N\log N+NB)$、空間計算量は $O(N+B)$ である。

## 注意点

- 容量は正の有限値、アイテムの大きさは非負の有限値で、容量以下でなければならない。違反は `std::invalid_argument` を送出する
- Harmonic 系は `class_count >= 2` でなければならず、違反は `std::invalid_argument` を送出する
- 各手法は明記した詰め方の規則を決定的に実行するヒューリスティックであり、最適なビン数は保証しない
