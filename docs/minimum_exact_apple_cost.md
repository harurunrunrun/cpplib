---
title: Minimum Exact Apple Cost (指定重量りんごの最小購入費) [ABA12C]
documentation_of: ../src/algorithm/other/minimum_exact_apple_cost.hpp
---

利用可能な重量別パケットを個数制限内で買う最小費用を返す。

# minimum_exact_apple_cost

```cpp
int minimum_exact_apple_cost(
    int maximum_packet_count, const vector<int>& packet_prices
);
```

`packet_prices[i-1]` は重量 `i` の価格で、`-1` は販売なしを表す。合計重量 `packet_prices.size()` を作れなければ `-1` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_exact_apple_cost` | $O(PK^2)$ | $O(K)$ |

## 注意点

- パケットは何個でも重複して選べるが、合計個数は `maximum_packet_count` 以下である。
- 不正な制限・価格では `runtime_error` を送出する。
