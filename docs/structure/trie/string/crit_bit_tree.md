---
title: Crit-bit Tree (Crit-bit木)
documentation_of: ../../../../src/structure/trie/string/crit_bit_tree.hpp
---

2 つのキーが最初に異なるバイト位置とビットだけを内部ノードに保持する、圧縮された二分トライである。文字列の集合を管理する。

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

最大のキー長を $L$ とする。`empty`, `size` は $O(1)$、`contains`・`insert`・`erase` は $O(L)$、`keys` は出力文字数に
線形である。空間計算量は格納する文字列の総長とキー数に線形である。

## 注意点

- `insert` は追加できれば `true` を、既に存在すれば `false` を返す。`erase` は削除できれば `true` を返す。
- キーは内部に `'\0'` を含まないこと（終端以降を `'\0'` とみなして比較するため）。空文字列は格納できる。
- `keys` は全キーのコピーを返す。順序は内部の木構造に従う。
