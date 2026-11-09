---
title: Subset and Bounded Partition Counts (部分和と個数制限付き分割の個数) [sharp_p_subset_sum]
documentation_of: ../../../../src/algorithm/math/combinatorics/subset_sum_counts.hpp
---

## API

```cpp
template<int MOD = 998244353>
std::vector<Modint<MOD>> subset_sum_counts(const std::vector<int>& weights, int maximum);

template<int MOD = 998244353>
std::vector<Modint<MOD>> bounded_partition_counts(
    const std::vector<std::pair<int, std::uint64_t>>& types, int maximum);
```

- `subset_sum_counts<MOD>(weights, maximum)`: 各要素を区別し、それぞれ 0 回または 1 回選ぶときの和が $s$ になる選び方の個数を、$s = 0, \ldots, \mathrm{maximum}$ について返す。
- `bounded_partition_counts<MOD>(types, maximum)`: 各 `(weight, count)` の種類から 0 個以上 `count` 個以下を選ぶときの和が $s$ になる選び方の個数を返す。同じ種類の個体は区別しない。

返り値の長さはいずれも `maximum + 1`（第 0 要素を含む）である。生成関数の対数を調和級数型のループで求め、形式的冪級数の指数で復元する。

## 時間計算量

入力の項数を $N$、最大和を $T$ として $O(N+T\log(T+1))$ 時間、$O(T)$ 空間。

## 注意点

- `MOD` は $T$ より大きい素数で、NTT の変換長の制約を満たす必要がある。`maximum < 0` または `maximum >= MOD` なら `invalid_argument` を送出する。
- 部分和版では負の重みに対して `invalid_argument` を送出する。重み $0$ の要素は、選ぶ・選ばないの 2 通りとして全体を 2 倍する。
- 個数制限版では重みは正でなければならず、$0$ 以下なら `invalid_argument` を送出する。個数は `uint64_t` で指定する。
- どちらも `maximum` を超える重みの要素は無視する。
