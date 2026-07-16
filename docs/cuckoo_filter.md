---
title: Cuckoo Filter (カッコウフィルタ)
documentation_of: ../src/approximate/streaming/cuckoo_filter.hpp
---

## Cuckoo Filter (カッコウフィルタ)

各 key の短い fingerprint を 2 候補 bucket のどちらかに置く固定容量 filter。
`BucketCount` は 2 の冪、fingerprint は 1--16 bit。

## API

- `CuckooFilter(seed)`: 空で構築する。時間計算量は $O(BucketCount\,SlotsPerBucket)$。 追加空間計算量は $O(1)$。
- `add(key)`: 空 slot を使い、満杯なら最大 `MaxKicks` 回 relocation する。
  成功時 `true`、容量不足時は変更を rollback して `false`。期待時間計算量は $O(1)$、最悪
  $O(MaxKicks)$。同じ fingerprint が候補 bucket にあれば追加済みとみなし `true`。 追加空間計算量は $O(MaxKicks)$。
- `contains(key)`: 2 bucket を調べる。時間計算量は $O(SlotsPerBucket)$。 追加空間計算量は $O(1)$。
- `erase(key)`: 一致 fingerprint を 1 個消す。時間計算量は $O(SlotsPerBucket)$。 追加空間計算量は $O(1)$。
- `clear()`: 時間計算量は $O(BucketCount\,SlotsPerBucket)$。 追加空間計算量は $O(1)$。
- `size()`, `capacity()`, `load_factor()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

偽陰性はない。ただし fingerprint の衝突があるため `contains` と `erase` は偽陽性に
なり得る。低負荷時の偽陽性率は概ね
$2\,SlotsPerBucket/2^{FingerprintBits}$。偽陽性 key の `erase` は別 key の fingerprint を
削除して偽陰性を生むため、削除は実際に追加した key に限る。
保存に必要な空間計算量は $O(BucketCount\,SlotsPerBucket)$。
