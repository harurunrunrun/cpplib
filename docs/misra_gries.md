---
title: Misra–Gries Algorithm (Misra–Griesアルゴリズム)
documentation_of: ../src/approximate/streaming/misra_gries.hpp
---

# Misra–Gries Algorithm (Misra–Griesアルゴリズム)

頻出候補を `Capacity` 個の counter で保持する。

## API

- `MisraGries()`: counterを0にして構築する。時間計算量と追加空間計算量はいずれも $O(Capacity)$。
- `add(key)`: 一致 counter の増加、空 slot への挿入、または全 counter の減算を行う。
  時間計算量は $O(Capacity)$。処理数が `Count` の上限なら `std::overflow_error`。 追加空間計算量は $O(1)$。
- `estimate_lower_bound(key)`: 保持中の counter、なければ 0。時間計算量は $O(Capacity)$。 追加空間計算量は $O(1)$。
- `candidates()`: 候補と下界を返す。時間計算量は $O(Capacity)$。 追加空間計算量は $O(Capacity)$。
- `maximum_underestimate()`: `total() / (Capacity + 1)`。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `clear()`: 時間計算量は $O(Capacity)$。 追加空間計算量は $O(1)$。
- `total()`, `capacity()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

真の頻度を $f_x$、返す下界を $\hat f_x$、stream 長を $N$ とすると、決定的に
$0\le f_x-\hat f_x\le\lfloor N/(Capacity+1)\rfloor$。従って
$f_x>N/(Capacity+1)$ の要素は必ず候補に残る。保存に必要な空間計算量は $O(Capacity)$。
