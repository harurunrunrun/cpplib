---
title: Cuckoo Filter
documentation_of: ../src/approximate/streaming/cuckoo_filter.hpp
---

各 key の短い fingerprint を 2 候補 bucket のどちらかに置く固定容量 filter。
`BucketCount` は 2 の冪、fingerprint は 1--16 bit。

- `CuckooFilter(seed)`: 空で構築する。$O(BucketCount\,SlotsPerBucket)$。
- `add(key)`: 空 slot を使い、満杯なら最大 `MaxKicks` 回 relocation する。
  成功時 `true`、容量不足時は変更を rollback して `false`。期待 $O(1)$、最悪
  $O(MaxKicks)$。同じ fingerprint が候補 bucket にあれば追加済みとみなし `true`。
- `contains(key)`: 2 bucket を調べる。$O(SlotsPerBucket)$。
- `erase(key)`: 一致 fingerprint を 1 個消す。$O(SlotsPerBucket)$。
- `clear()`: $O(BucketCount\,SlotsPerBucket)$。
- `size()`, `capacity()`, `load_factor()`: $O(1)$。

偽陰性はない。ただし fingerprint の衝突があるため `contains` と `erase` は偽陽性に
なり得る。低負荷時の偽陽性率は概ね
$2\,SlotsPerBucket/2^{FingerprintBits}$。偽陽性 key の `erase` は別 key の fingerprint を
削除して偽陰性を生むため、削除は実際に追加した key に限る。
保存領域は $O(BucketCount\,SlotsPerBucket)$。
