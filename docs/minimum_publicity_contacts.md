---
title: Minimum Publicity Contacts (宣伝に必要な最小接触人数) [CAM5]
documentation_of: ../src/algorithm/graph/minimum_publicity_contacts.hpp
---

友達関係が与えられた集団へ情報を広めるため、最初に直接伝える必要がある人数の最小値を返す。

# minimum_publicity_contacts

```cpp
int minimum_publicity_contacts(
    int peer_count,
    const vector<pair<int, int>>& friendships
);
```

`peer_count` 頂点の無向グラフについて、連結成分数を返す。

## API別の時間計算量・空間計算量

頂点数を $N$、友達関係の数を $M$、逆 Ackermann 関数を $\alpha$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_publicity_contacts` | $O((N+M)\alpha(N))$ | $O(N)$ |

## 注意点

- 頂点番号は `0` 以上 `peer_count` 未満でなければならない。
- `peer_count` が負、または頂点番号が範囲外の場合は `runtime_error` を送出する。
- 自己ループと多重辺は答えに影響しない。
