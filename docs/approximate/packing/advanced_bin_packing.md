---
title: Advanced One-Dimensional Bin Packing (高度な一次元ビンパッキング)
documentation_of: ../../../src/approximate/packing/advanced_bin_packing.hpp
---

`approximate::packing` 名前空間で roadmap の高度な 1D bin packing API を
提供する。全関数は元 item 添字に対応する `BinPackingResult` を返す。

## decreasing shortcut

```cpp
next_fit_decreasing_bin_packing(items, capacity);
first_fit_decreasing_bin_packing(items, capacity);
best_fit_decreasing_bin_packing(items, capacity);
worst_fit_decreasing_bin_packing(items, capacity);
```

既存4手法を item 降順で実行する明示 API。Next Fit は
$O(N\log N)$、他は $O(N\log N)$、空間計算量は $O(N)$。

## Almost Worst Fit

```cpp
almost_worst_fit_bin_packing(items, capacity, decreasing = false);
```

入る bin のうち残容量が2番目に大きい bin を選び、1個しかなければ最大を選ぶ。
時間計算量は $O(N\log N)$、空間計算量は $O(N)$。

## Harmonic family

```cpp
harmonic_bin_packing(items, capacity, class_count = 10);
harmonic_fit_bin_packing(items, capacity, class_count = 10);
harmonic_first_fit_bin_packing(items, capacity, class_count = 10);
```

サイズ比 $(1/(j+1),1/j]$ を class $j$ とし、class $j$ は1 binに $j$ 個ずつ
詰める。最小 class は Harmonic/Harmonic Fit では Next Fit、Harmonic First
Fit では First Fit を使う。前二者の時間計算量は $O(NK)$、Harmonic First Fit
は worst-case $O(N^2+NK)$。空間計算量は $O(N+K)$。

## Refined First Fit / MFFD

```cpp
refined_first_fit_bin_packing(items, capacity);
modified_first_fit_decreasing_bin_packing(items, capacity);
mffd_bin_packing(items, capacity);
```

Refined First Fit は $1/2,2/5$ の size class と6個ごとの reserved medium bin
を使って large item との組合せを制御する。MFFD は降順 item を
large ($>1/2$)、medium/small ($>1/6$)、tiny に分け、large bin の best
remainder を先に埋め、その後 First Fit する。`mffd_bin_packing` は同一処理の
alias。

時間計算量は RFF が $O(NB)$、MFFD が $O(N\log N+NB)$、空間計算量は
$O(N+B)$。$B$ は生成 bin 数。

## 注意点

capacity は正有限、item は非負有限で capacity 以下でなければならない。
`class_count >= 2` が必要。各手法は明記された packing 規則を決定的に実行する
heuristic であり、最適 bin 数を保証しない。
