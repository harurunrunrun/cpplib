---
title: Patricia Trie (Patriciaトライ)
documentation_of: ../../../../src/structure/trie/string/patricia_trie.hpp
---

分岐しないパスを辺ラベルに圧縮したトライによる、文字列の集合である。各ノードの子は `std::map` で管理する。

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

`insert` は追加できれば `true` を、既に存在すれば `false` を返す。`erase` は削除できれば `true` を返す。
`keys` は全キーを、バイトを符号なしとみなした辞書順で返す。

## 時間計算量

キー長を $L$、各ノードの分岐数（文字種）を $\sigma$ とする。

- `empty`, `size`: $O(1)$
- `contains`, `insert`, `erase`: $O(L\log\sigma)$
- `keys`: 出力文字数に線形

空間計算量は、格納する圧縮された辺ラベルの総長に線形である。

## 注意点

空文字列も格納できる。`keys` が返す文字列は独立したコピーである。
