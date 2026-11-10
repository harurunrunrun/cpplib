---
title: Degeneracy Ordering (退化順序)
documentation_of: ../../../../src/algorithm/graph/traversal/degeneracy_ordering.hpp
---

無向グラフの頂点を、取り除く時点で残っているグラフでの次数が常に退化数以下になる順序に並べる。
次数ごとのバケットを直接更新する、他のヘッダに依存しない実装である。

## API

```cpp
DegeneracyOrderingResult degeneracy_ordering(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

```cpp
struct DegeneracyOrderingResult {
    vector<int> order;
    int degeneracy;
};
```

- `order`: 先頭から頂点を取り除く退化順序
- `degeneracy`: 退化数。すなわち、各頂点を取り除く時点の次数の最大値（この順序で達成される最小の上界）

多重辺は別々に数え、自己ループは無視する。

## 時間計算量

頂点数を $N$、自己ループを除く辺数を $M$ とする。

- 隣接リストと次数バケットの構築: $O(N+M)$
- 各辺に対する次数更新: $O(N+M)$
- 戻り値の構築: $O(N)$
- `degeneracy_ordering` 全体: $O(N+M)$

## 空間計算量

$O(N+M)$。

## 注意点

- 頂点番号は $[0,N)$ でなければならない。範囲外の頂点または負の頂点数を受け取ると `runtime_error` を送出する。
- 同じグラフに正しい退化順序が複数存在し得る。
