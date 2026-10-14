---
title: Static Range Increasing Subsequence (区間最長増加部分列) [static_range_lis_query]
documentation_of: ../../../../src/structure/range_query/value_query/static_range_lis.hpp
---

`StaticRangeLIS<T,MAX_SIZE>(values,nondecreasing=false)` は各半開区間の最長増加部分列長を照会する。
`query(l,r)` を使う。`nondecreasing=true` では広義単調増加部分列長を求める。
値型は `<` とコピーが可能であればよく、負数や同値の重複も許す。
seaweed doubling と unit Monge 積で索引を構築し、wavelet matrix で照会する。

## 時間計算量

配列長 $N$ に対して構築 $O(N\log^2(N+1))$、照会 $O(\log(\mathtt{MAX\_SIZE}+1))$。
保存空間 $O(\mathtt{MAX\_SIZE}\log(\mathtt{MAX\_SIZE}+1))$。構築中の追加空間 $O(N\log(N+1))$。

## 注意点

`N<=MAX_SIZE`、`0<=l<=r<=N`。違反時は `out_of_range`。空区間の答えは 0。
内部の unit Monge 積は noshi91 氏の実装を改変している。帰属表記と Apache-2.0 の全文はヘッダ内を参照。

## 可変長版

```cpp
using Dynamic = StaticRangeLIS<int>;
```

容量の template 引数を省略すると、実行時の要素数・更新数に合わせて `std::vector` を確保する。前処理 O(n log²(n + 1))、保存領域 O(n log(n + 1))、区間 LIS 長の取得 O(log(n + 1))。

容量を明示した固定長版も同じヘッダ・このページの code に含まれる。上記の固定上限に関する説明は容量を明示した場合に適用される。
