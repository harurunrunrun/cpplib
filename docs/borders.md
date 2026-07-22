---
title: Borders (境界) [SUFEQPRE]
documentation_of: ../src/algorithm/string/automata/borders.hpp
---

列の非空な真のprefixであり、同時にsuffixでもある部分列（border）の長さを列挙する。

## API

```cpp
template<class Sequence>
std::vector<int> border_lengths(const Sequence& sequence)
```

## 引数・戻り値

- `sequence`: borderを列挙する入力列。
- 戻り値: すべてのborder長を狭義単調増加順に格納したvector。
  空列と長さ1の列では空vectorを返す。

## API別の時間計算量・空間計算量

$N$ を列長、$K$ をborder数とする。

| API | 時間計算量 | 空間計算量（出力・追加領域） |
| --- | --- | --- |
| `border_lengths(sequence)` | $O(N+K)$ | 追加領域 $O(N)$、出力 $O(K)$ |

要素比較は $O(1)$ とする。

## 注意点

- 空prefix、空suffix、列全体はborderとして返さない。
- prefixとsuffixの位置が重なるborderも返す。
- `Sequence` は `size()`、0-indexed random access、要素の `operator==` に
  対応し、列長は `int` に収まることを前提とする。
