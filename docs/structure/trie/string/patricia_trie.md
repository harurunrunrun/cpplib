---
title: Patricia Trie (Patriciaトライ)
documentation_of: ../../../../src/structure/trie/string/patricia_trie.hpp
---

branchしないpathをedge labelへ圧縮したstring set。

## API

~~~cpp
PatriciaTrie()
bool empty() const
size_t size() const
bool contains(string_view key) const
bool insert(string_view key)
bool erase(string_view key)
vector<string> keys() const
~~~

`keys` はunsigned-byte lexicographic順で全keyを返す。

## 時間計算量

key長を $L$、alphabet上のbranch数を $\sigma$ とする。

- `empty`, `size`: $O(1)$
- `contains`, `insert`, `erase`: $O(L\log\sigma)$
- `keys`: 出力文字数に線形

空間計算量は格納する圧縮edge labelの総長に線形。

## 注意点

空stringも格納できる。返したstringは独立したcopyである。
