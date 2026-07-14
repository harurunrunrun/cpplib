---
title: Minimum Optimal Tree TTL (木ネットワークの最小最適TTL) [GCPC11J]
documentation_of: ../src/algorithm/tree/minimum_optimal_tree_ttl.hpp
---

木ネットワークで送信元を最適に選んだとき、全頂点へ届くために必要な最小 TTL を返す。

# minimum_optimal_tree_ttl

```cpp
int minimum_optimal_tree_ttl(
    int vertex_count,
    const vector<pair<int, int>>& edges
);
```

無向木の任意の頂点を送信元に選べるとき、全頂点までの距離の最大値を最小化した値を返す。頂点数 `0` では `0` を返す。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_optimal_tree_ttl` | $O(N)$ | $O(N)$ |

## 注意点

- 頂点番号は `0` 以上 `vertex_count` 未満でなければならない。
- 辺集合は自己ループや多重辺を含まない連結な木でなければならない。
- 頂点番号が範囲外、または辺集合が木でない場合は `runtime_error` を送出する。
