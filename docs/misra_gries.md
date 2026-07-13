---
title: Misra-Gries
documentation_of: ../src/approximate/streaming/misra_gries.hpp
---

頻出候補を `Capacity` 個の counter で保持する。

- `add(key)`: 一致 counter の増加、空 slot への挿入、または全 counter の減算を行う。
  $O(Capacity)$。処理数が `Count` の上限なら `std::overflow_error`。
- `estimate_lower_bound(key)`: 保持中の counter、なければ 0。$O(Capacity)$。
- `candidates()`: 候補と下界を返す。$O(Capacity)$。
- `maximum_underestimate()`: `total() / (Capacity + 1)`。$O(1)$。
- `clear()`: $O(Capacity)$。
- `total()`, `capacity()`: $O(1)$。

真の頻度を $f_x$、返す下界を $\hat f_x$、stream 長を $N$ とすると、決定的に
$0\le f_x-\hat f_x\le\lfloor N/(Capacity+1)\rfloor$。従って
$f_x>N/(Capacity+1)$ の要素は必ず候補に残る。保存領域は $O(Capacity)$。
