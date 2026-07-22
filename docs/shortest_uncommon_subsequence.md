---
title: Shortest Uncommon Subsequence (最短非共通部分列) [MSTRING]
documentation_of: ../src/algorithm/string/sequence/shortest_uncommon_subsequence.hpp
---

列 `source` の部分列であり、列 `forbidden` の部分列ではない非空列の
最短長を返す。

## API

```cpp
template<class SourceSequence, class ForbiddenSequence>
int shortest_uncommon_subsequence_length(
    const SourceSequence& source,
    const ForbiddenSequence& forbidden
)
```

## 引数・戻り値

- `source`: 候補となる部分列の元の列。
- `forbidden`: 部分列になってはならない列。
- 戻り値: 条件を満たす非空部分列の最短長。存在しなければ `-1`。
  `source` が空なら `-1`、`source` が非空で `forbidden` が空なら `1`。

## API別の時間計算量・空間計算量

$N=|source|$, $M=|forbidden|$ とし、要素比較を $O(1)$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `shortest_uncommon_subsequence_length(source,forbidden)` | $O((N+1)(M+1))$ | $O(M+1)$ |

## 注意点

- 空列は答えの候補に含めない。
- 両方の列長は `int` に収まる必要がある。
- 両Sequenceは `size()`、0-indexed random access、要素間の `operator==` に
  対応する必要がある。
- 具体的な部分列は返さない。
