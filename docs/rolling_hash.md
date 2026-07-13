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

## API別計算量

$N$ を構築文字列長、$L$ を比較可能なsuffix長の小さい方とする。

| API | 時間計算量 |
| --- | --- |
| `RollingHash()` | $O(1)$ |
| `RollingHash(s,base)` | $O(N)$（前処理） |
| `build(s,base)` | $O(N)$（再構築） |
| `size()` / `base_value()` | $O(1)$（metadata query） |
| `get(l,r)` / `all()` | $O(1)$ |
| `lcp(other,l1,l2)` | $O(\log(L+1))$ |
| static metadata `mod` / `default_base` の参照 | $O(1)$ |

構築後の保持領域は $O(N)$。hash比較には衝突可能性があるが、計算量は入力内容に依存しない。
