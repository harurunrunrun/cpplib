---
title: Elias-Fano Encoding (Elias-Fano符号)
documentation_of: ../../../src/structure/compressed/elias_fano.hpp
---

単調非減少unsigned integer列をhigh bitvectorとpacked low bitsへ圧縮する。

## API

~~~cpp
EliasFano()
EliasFano(const vector<uint64_t>& values)
void build(const vector<uint64_t>& values)
bool empty() const
size_t size() const
uint64_t select(size_t index) const
size_t lower_bound(uint64_t value) const
size_t rank(uint64_t value) const
uint64_t universe() const
~~~

`rank(x)` は `x` 以下の要素数。duplicateも保持する。

## 時間計算量

- 構築: $O(N)$
- `select`: $O(1)$ (`256` 個ごとのselect sample)
- `lower_bound`, `rank`: $O(\log(N+1))$
- `empty`, `size`, `universe`: $O(1)$

bitvector本体は $O(N\log(U/N+1))$ bit、select sampleは $O(N/256)$ machine
word。

## 注意点

入力が単調非減少でなければ `invalid_argument`。範囲外selectは
`out_of_range`。
