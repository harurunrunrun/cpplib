---
title: Space-Saving (頻出要素推定)
documentation_of: ../src/approximate/streaming/space_saving.hpp
---

# Space-Saving (頻出要素推定)

頻出候補を推定値と誤差上界付きで保持する。

## API

- `SpaceSaving::Candidate`: `key`, 頻度推定値 `estimate`, 過大評価幅 `error` を保持する。構築・各member参照の時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `SpaceSaving()`: 全slotを空にして構築する。時間計算量と追加空間計算量はいずれも $O(Capacity)$。
- `add(key, delta)`: 候補を加算する。未登録かつ満杯なら最小 counter を置換する。
  時間計算量は $O(Capacity)$。総重みが `Count` の上限を超える更新は `std::overflow_error`。 追加空間計算量は $O(1)$。
- `estimate(key)`: `Candidate{key, estimate, error}` を返す。時間計算量は $O(Capacity)$。 追加空間計算量は $O(1)$。
- `candidates()`: 推定値の降順で候補を返す。時間計算量は $O(Capacity\log Capacity)$。 追加空間計算量は $O(Capacity)$（返値を含む）。
- `clear()`: 時間計算量は $O(Capacity)$。 追加空間計算量は $O(1)$。
- `total()`, `size()`, `capacity()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

候補について `estimate - error <= true_frequency <= estimate`。単位更新では
最小 counter は高々 $N/Capacity$ なので、頻度が $N/Capacity$ を超える要素は
必ず候補となる。保存に必要な空間計算量は $O(Capacity)$。
