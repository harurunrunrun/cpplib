---
title: Maximum Antichain (最大反鎖)
documentation_of: ../../../../src/algorithm/matching/bipartite/maximum_antichain.hpp
---

DAG の到達可能性が定める半順序について、互いに比較不能な頂点の最大集合（最大反鎖）を求める。Dilworth の定理により、推移閉包上の最大マッチングから構成する。

## API

```cpp
MaximumAntichainResult maximum_antichain(
    const vector<vector<int>>& graph
)
```

`graph` は隣接リストで与えた有向グラフである。

```cpp
struct MaximumAntichainResult {
    int size;
    vector<int> vertices;
};
```

- `size`: 最大反鎖の要素数
- `vertices`: 最大反鎖をなす頂点番号（昇順）

## 時間計算量

頂点数を $N$、入力の辺数を $M$、推移閉包の辺数を $R$、$W = 64$ とする。

- DAG 判定: $O(N + M)$
- ビット集合による推移閉包: $O((N + M) \lceil N/W \rceil)$
- 推移閉包からの二部グラフ構築: $O(N \lceil N/W \rceil + R)$（各ワードの立っているビットだけを列挙する）
- Hopcroft–Karp 法と反鎖の復元: $O(R \sqrt{N} + N)$
- `maximum_antichain` 全体: $O((N + M) \lceil N/W \rceil + R \sqrt{N})$

## 空間計算量

$O(N \lceil N/W \rceil + R)$。

## 注意点

入力は DAG でなければならない。閉路を含む場合は `runtime_error` を送出する。隣接頂点が範囲外の場合も `runtime_error` を送出する。最大反鎖が複数ある場合は、そのうちの 1 つを返す。
