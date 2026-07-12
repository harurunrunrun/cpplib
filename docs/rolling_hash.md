---
title: Rolling Hash
documentation_of: ../src/algorithm/string/rolling_hash.hpp
---

mod $2^{61}-1$ の Rolling Hash。

# コンストラクタ

```cpp
RollingHash()
RollingHash(const string& s, uint64_t base = ...)
```

# hash

```cpp
uint64_t get(int l, int r)
uint64_t all()
```

半開区間 `[l,r)` の hash を返す。

# lcp

```cpp
int lcp(const RollingHash& other, int l1, int l2)
```

`this[l1:]` と `other[l2:]` の LCP 長を返す。

## 時間計算量

- build: $O(N)$
- get: $O(1)$
- lcp: $O(\log N)$
