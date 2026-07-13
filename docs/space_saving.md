---
title: Space-Saving
documentation_of: ../src/approximate/streaming/space_saving.hpp
---

頻出候補を推定値と誤差上界付きで保持する。

- `add(key, delta)`: 候補を加算する。未登録かつ満杯なら最小 counter を置換する。
  $O(Capacity)$。総重みが `Count` の上限を超える更新は `std::overflow_error`。
- `estimate(key)`: `Candidate{key, estimate, error}` を返す。$O(Capacity)$。
- `candidates()`: 推定値の降順で候補を返す。$O(Capacity\log Capacity)$。
- `clear()`: $O(Capacity)$。
- `total()`, `size()`, `capacity()`: $O(1)$。

候補について `estimate - error <= true_frequency <= estimate`。単位更新では
最小 counter は高々 $N/Capacity$ なので、頻度が $N/Capacity$ を超える要素は
必ず候補となる。保存領域は $O(Capacity)$。
