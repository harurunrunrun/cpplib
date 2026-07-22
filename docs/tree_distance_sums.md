---
title: Tree Distance Sums (木の全頂点距離和) [ABC220_F]
documentation_of: ../src/algorithm/tree/dynamic_programming/tree_distance_sums.hpp
---

無向木の各頂点について、全頂点への距離の総和を返す。

## 関数

```cpp
vector<long long> tree_distance_sums(
    int n,
    const vector<pair<int, int>>& edges
)
```

戻り値の`v`番目は、頂点`v`から全頂点への辺数距離の総和。
`n == 0`では空vectorを返す。

## API別の時間計算量・空間計算量

$N=n$とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `tree_distance_sums(n, edges)` | $O(N)$ | $O(N)$ |

## 注意点

- $n$は非負でなければならない。
- 頂点番号は$[0,n)$。
- `n > 0`では`edges`が`n-1`辺からなる連結な無向木でなければならない。
- `n == 0`では`edges`も空でなければならない。
- 頂点範囲、辺数、連結性、非巡回性の違反では`runtime_error`を送出する。
- 距離和は`long long`に格納する。
