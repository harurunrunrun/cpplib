---
title: Maximum Bipartite Pair Count (二部グラフ最大ペア数) [MATCHING]
documentation_of: ../src/algorithm/matching/maximum_bipartite_pair_count.hpp
---

許可された組だけを使う一対一対応の最大数を返す。

# maximum_bipartite_pair_count

```cpp
int maximum_bipartite_pair_count(
    int left_size,
    int right_size,
    const vector<pair<int, int>>& allowed_pairs
);
```

左側 `left_size` 頂点、右側 `right_size` 頂点からなる二部グラフの最大matchingサイズを返す。`allowed_pairs[i]` は接続可能な左頂点と右頂点の組である。

## API別の時間計算量・空間計算量

$L=left_size$、$R=right_size$、$E=allowed_pairs.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_bipartite_pair_count` | $O(E\sqrt{L+R})$ | $O(L+R+E)$ |

## 注意点

- 頂点数は非負、各組の添字は対応する頂点範囲内でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 同じ組を複数回入力しても最大ペア数は変わらない。
