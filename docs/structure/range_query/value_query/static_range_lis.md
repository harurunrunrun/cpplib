---
title: Static Range Increasing Subsequence (区間最長増加部分列) [static_range_lis_query]
documentation_of: ../../../../src/structure/range_query/value_query/static_range_lis.hpp
---

静的な列の各半開区間について、最長増加部分列の長さを求める。

## API

```cpp
template<class T, int MAX_SIZE = -1>
class StaticRangeLIS;

explicit StaticRangeLIS(const vector<T>& values, bool nondecreasing = false)
int size() const
int query(int l, int r) const
```

- `query(l, r)` は `values[l..r)` の最長（狭義）増加部分列の長さを返す。
- `nondecreasing = true` を指定すると、広義単調増加部分列の長さを求める。
- 値の型は `<` による比較とコピーができればよく、負の数や同じ値の重複も許す。
- `MAX_SIZE` は列の長さの上限であり、省略時（`-1`）は上限を設けない。

seaweed doubling と unit Monge 積で索引を構築し、wavelet matrix でクエリに答える。

## 時間計算量

列の長さを $N$ とする。

- 構築: $O(N\log^2(N+1))$
- `query`: $O(\log(\texttt{MAX_SIZE}+1))$（容量省略時は $O(\log(N+1))$）

保存領域は $O(\texttt{MAX_SIZE}\log(\texttt{MAX_SIZE}+1))$（容量省略時は $O(N\log(N+1))$）、構築中の追加領域は $O(N\log(N+1))$ である。

## 注意点

- `N <= MAX_SIZE`、`0 <= l <= r <= N` を要求し、違反時は `std::out_of_range` を送出する。空区間の答えは 0 である。
- 内部の unit Monge 積は noshi91 氏の実装を改変したものである。帰属表記と Apache-2.0 の全文はヘッダ内を参照。
