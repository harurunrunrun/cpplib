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
- `select`: worst-case $O(1)$
- `lower_bound`, `rank`: worst-case $O(\log(N+1))$
- `empty`, `size`, `universe`: $O(1)$

The low bits and high bitvector use $O(N\log(U/N+1)+N)$ bits. The select
directory samples every 256th one. A block spanning at most 65536 bits is
scanned for a fixed number of machine words; a longer sparse block stores all
of its one positions directly. Consequently a locally huge zero run cannot
increase the select time. Because disjoint long blocks consume more than
$256^2$ high-bit positions each, all select directories together use
$O(N/256)$ machine words.

## 注意点

入力が単調非減少でなければ `invalid_argument`。範囲外selectは
`out_of_range`。
