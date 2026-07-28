---
title: Crit-bit Tree (Crit-bit木)
documentation_of: ../../../../src/structure/trie/string/crit_bit_tree.hpp
---

最初に異なるbyte/bitだけをinternal nodeに保持するcompressed binary trie。

## API

~~~cpp
CritBitTree()
bool empty() const
size_t size() const
bool contains(string_view key) const
bool insert(string key)
bool erase(string_view key)
vector<string> keys() const
~~~

## 時間計算量

最大key長を $L$ とする。検索・追加・削除は $O(L)$、`keys` は出力文字数に
線形。空間計算量は格納文字列の総長とkey数に線形。

## 注意点

keyは内部に `'\0'` を含まないこと。空stringは格納できる。
