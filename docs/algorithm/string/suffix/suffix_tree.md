---
title: Suffix Tree (接尾辞木)
documentation_of: ../../../../src/algorithm/string/suffix/suffix_tree.hpp
---

接尾辞配列とLCP配列から、辺ラベルを元文字列上の半開区間として持つ圧縮接尾辞木を
構築する。ノード数は高々 $2N-1$ である。

## API

```cpp
SuffixTree(std::string text)
int node_count() const
const Node& operator[](int node) const
const std::string& text() const
int find_locus(std::string_view pattern) const
bool contains(std::string_view pattern) const
int count(std::string_view pattern) const
std::vector<int> locate(std::string_view pattern) const
```

`Node` は `parent`, `edge_begin`, `edge_end`, `string_depth`,
`suffix_begin`, `leaf_count`, `children` を持つ。`find_locus` はパターン末尾を
含む辺の下側ノードを返し、不在なら `-1` を返す。空パターンは全境界に一致する。

## API別の時間計算量・空間計算量

$N=|text|,\ M=|pattern|,\ K$ を報告件数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `node_count`, `operator[]`, `text` | $O(1)$ | $O(1)$ |
| `find_locus`, `contains`, `count` | $O(M\log 256)$ | $O(1)$ |
| `locate` | $O(M\log 256+K)$ | $O(K)$ |

不正なノード番号では例外を送出する。

## 注意点

辺ラベルは文字列を複製せず、`text()[edge_begin:edge_end]` を参照する。
