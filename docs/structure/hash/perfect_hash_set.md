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

## 時間計算量

With a separating base hash, construction is expected $O(N)$ and `contains` is worst-case $O(1)$.
If distinct keys have the same base hash, construction terminates using an equality-checked fallback bucket; lookup in that bucket is $O(C)$ for collision-group size $C$.
`empty` and `size` are $O(1)$, and expected space is $O(N)$.

## 注意点

同値keyは1個へまとめる。異なるkeyに対して基礎 `Hash` が十分なentropyを
持つこと。
Construction retries are bounded, so even a constant custom `Hash` cannot cause an infinite loop.
