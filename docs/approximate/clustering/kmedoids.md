---
title: PAM / CLARA / CLARANS k-Medoids (kメドイド法)
documentation_of: ../../../src/approximate/clustering/kmedoids.hpp
---

`approximate::clustering` 名前空間で、添字間の距離 `distance(i, j)` を受け取る k-medoids 法を提供する。距離は有限かつ非負でなければならない。通常は対称で `distance(i, i) == 0` を満たす距離を渡す。同距離の場合は小さい medoid 添字を優先する。

## `KMedoidsResult`

- `medoids`: 代表点（medoid）の添字の昇順列。
- `labels[i]`: 点 `i` に最も近い medoid の、`medoids` 内での位置。
- `cost`: 各点から割り当てた medoid までの距離の和。
- `iterations`: PAM では採用した交換の回数、CLARANS では選ばれた試行で採用した交換の回数。

## `pam_k_medoids`

```cpp
auto result = pam_k_medoids(n, k, distance, maximum_iterations = 100);
```

BUILD 段階で貪欲に medoid を追加し、その後、全ての medoid と非 medoid の交換を調べる SWAP 段階を行う。本実装は距離表を前計算する。採用した交換の回数を $I$ とすると、時間計算量は $O(n^2+(k+Ik)n^2)$、追加空間計算量は $O(n^2)$ である。

## `clara_k_medoids`

```cpp
auto result = clara_k_medoids(n, k, distance, sample_size, sample_count,
                              rng, maximum_pam_iterations = 100);
```

各標本に PAM を適用し、全 $n$ 点で評価した最良解を返す。標本は `rng` で非復元抽出する。標本サイズを $s$、標本数を $r$ とすると、時間計算量は $O(n^2+r((k+Ik)s^2+nk))$、追加空間計算量は $O(n^2+s^2)$ である。

## `clarans_k_medoids`

```cpp
auto result = clarans_k_medoids(n, k, distance,
                                maximum_neighbor_checks, restarts, rng);
```

ランダムな 1 点交換の近傍を調べ、改善すれば採用して失敗回数を 0 に戻す。各再始動で得た局所解のうち最良のものを返す。調べた近傍数を $Q$、採用数を $A$、再始動回数を $R$ とすると、時間計算量は $O(n^2+R(A+1)Qnk)$、追加空間計算量は $O(n^2)$ である。

## 注意点

- 全ての API で $1\le k\le n$ でなければならない。CLARA は $k\le s\le n$ かつ正の標本数を、CLARANS は正の探索上限と再始動回数を要求し、違反時は `std::invalid_argument` を送出する。
- 距離の和が `long double` の範囲を超える場合は `std::overflow_error` を送出する。
- 局所探索であり、大域最適性は保証しない。
