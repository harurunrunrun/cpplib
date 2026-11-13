---
title: Suffix Tree (接尾辞木)
documentation_of: ../../../../src/algorithm/string/suffix/suffix_tree.hpp
---

接尾辞配列と LCP 配列から、辺ラベルを元の文字列上の半開区間として持つ圧縮接尾辞木を構築する。
ノード数は高々 $2N-1$ である。

## API

```cpp
explicit SuffixTree(std::string text)
int node_count() const
const Node& operator[](int node) const
const std::string& text() const
int find_locus(std::string_view pattern) const
bool contains(std::string_view pattern) const
int count(std::string_view pattern) const
std::vector<int> locate(std::string_view pattern) const
```

- `Node` はメンバ `parent`, `edge_begin`, `edge_end`, `string_depth`, `suffix_begin`, `leaf_count`, `children` を持つ。根はノード `0` である。
- `operator[](node)` は不正なノード番号に対して `std::runtime_error` を送出する。
- `find_locus(pattern)` は、パターンの末尾を含む辺の下側のノードを返し、パターンが現れなければ `-1` を返す。空パターンでは根 `0` を返す。
- `contains`, `count`, `locate` はそれぞれ、出現の有無・出現回数・出現開始位置の列を返す。
- 空パターンは全境界（$N+1$ 箇所）に一致する。

## API 別の時間計算量・空間計算量

$N = \lvert \mathtt{text}\rvert$、$M = \lvert \mathtt{pattern}\rvert$、$K$ を報告件数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `node_count`, `operator[]`, `text` | $O(1)$ | $O(1)$ |
| `find_locus`, `contains`, `count` | $O(M\log 256)$ | $O(1)$ |
| `locate` | $O(M\log 256+K)$ | $O(K)$ |

## 注意点

辺ラベルは文字列を複製せず、`text()` の `[edge_begin, edge_end)` を参照する。
