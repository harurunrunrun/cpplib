---
title: Static Perfect Hash Set (静的完全ハッシュ集合)
documentation_of: ../../../src/structure/hash/perfect_hash_set.hpp
---

FKS two-level hashingで構築後の衝突をなくすstatic set。

## API

~~~cpp
PerfectHashSet<Key, Hash, Equal>()
PerfectHashSet(const vector<Key>& keys)
void build(const vector<Key>& keys)
bool empty() const
size_t size() const
bool contains(const Key& key) const
~~~

The constructor and `build` deduplicate equal keys. They rebuild the first
level until the sum of squared bucket sizes is linear, then rebuild every
second-level table until it is collision-free. `contains` reads exactly one
first-level and one second-level slot and confirms the candidate with `Equal`.

There is no bounded-retry fallback. If two stored keys are not equal under
`Equal` but have the same base `Hash` value, `build` throws
`invalid_argument`. A failed rebuild preserves the previously constructed
set.

## 時間計算量

For input length $I$ and $N$ distinct keys, with worst-case $O(1)$ `Hash` and
`Equal` and a separating base hash:

- construction: expected $O(I+N)$ time and $O(I+N)$ temporary space
- `contains`: worst-case $O(1)$
- `empty`, `size`: $O(1)$
- space after construction: expected $O(N)$

Each unbounded rebuild loop succeeds with constant probability.

## 注意点

同値keyは1個へまとめる。`Hash` と `Equal` は通常のhash契約を満たし、さらに
格納する非同値keyのbase hash値はすべて異なる必要がある。constant hashへ
複数の非同値keyを渡すと、遅いfallbackを作らず `invalid_argument` を送出する。
multiply-shift familyの再選択は固定回数で打ち切らず、expected $O(1)$ 回かつ
確率1で成功する。
