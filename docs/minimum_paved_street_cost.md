---
title: Minimum Paved Street Cost (最小道路舗装費) [CSTREET]
documentation_of: ../src/algorithm/graph/minimum_paved_street_cost.hpp
---

# minimum_paved_street_cost

```cpp
long long minimum_paved_street_cost(
    long long unit_price,
    int vertex_count,
    const vector<MinimumNetworkConnectionEdge>& streets
)
```

全頂点を接続する道路を選ぶときの、`unit_price` を反映した最小費用を返す。

## 引数

- `unit_price`: 道路長1単位当たりの費用。
- `vertex_count`: 頂点数。
- `streets`: 道路候補。`cost` に道路長を格納する。

## 戻り値

最小舗装費用。

## API別の時間計算量・空間計算量

$N=vertex_count$, $M=streets.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_paved_street_cost` | $O(N+M\log(M+1))$ | $O(N+M)$ |

## 注意点

- `unit_price` と各道路長は非負でなければならない。
- 非連結、端点範囲違反、または答えが `long long` を越える場合は例外を送出する。
