---
title: Shortest Uncommon Subsequence (最短非共通部分列) [MSTRING]
documentation_of: ../src/algorithm/string/shortest_uncommon_subsequence.hpp
---

列 `source` の部分列であり、列 `forbidden` の部分列ではない非空列の最短長を
動的計画法で求める。

## API

```cpp
int shortest_uncommon_subsequence_length(source, forbidden)
```

## API詳細

`shortest_uncommon_subsequence_length(source, forbidden)` は条件を満たす非空部分列の
最短長を返す。存在しない場合は `-1` を返す。特に `source` が空なら `-1`、
`source` が非空で `forbidden` が空なら `1` となる。要素は `operator==` で比較する。

各状態では `source` の先頭要素を使わない遷移と、`forbidden` 内の最初の一致へ
対応させて使う遷移を比較する。最初の一致を選ぶと残りの `forbidden` が最長になり、
その要素を使った部分列が `forbidden` から外れるための最悪条件を表せる。

## API別の時間計算量・空間計算量

$N=|source|$, $M=|forbidden|$ とし、要素比較を $O(1)$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `shortest_uncommon_subsequence_length(source,forbidden)` | $O(NM)$ | $O(M)$ |

DPは `source` の隣接する2行だけを保持する。

## 注意点

空列は任意の列の部分列なので、答えの候補には含めない。長さは `int` に収まることを
前提とし、このAPIは具体的な部分列の復元を行わない。
