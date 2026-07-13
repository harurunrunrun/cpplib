---
title: PAM / CLARA / CLARANS k-Medoids (kメドイド法)
documentation_of: ../src/approximate/clustering/kmedoids.hpp
---

`approximate::clustering`名前空間で、添字間距離`distance(i, j)`を受け取る
k-medoids法を提供する。距離は有限かつ非負でなければならない。通常は対称かつ
`distance(i,i)==0`の距離を渡す。同距離では小さいmedoid添字を優先する。

## `KMedoidsResult`

- `medoids`: 昇順の代表点添字。
- `labels[i]`: 点`i`に最も近い`medoids`内の位置。
- `cost`: 各点から割当medoidまでの距離の和。
- `iterations`: PAMでは採用swap数、CLARANSでは選ばれたrunの採用swap数。

## `pam_k_medoids`

```cpp
auto result = pam_k_medoids(n, k, distance, maximum_iterations);
```

BUILDで貪欲にmedoidを追加し、その後すべてのmedoid/non-medoid交換を調べる
SWAPを行う。本実装は距離表を前計算する。時間計算量は $O(n^2+(k+Ik)n^2)$、
追加空間計算量は $O(n^2)$（$I$は採用swap数）。

## `clara_k_medoids`

```cpp
auto result = clara_k_medoids(n, k, distance, sample_size, sample_count,
                              rng, maximum_pam_iterations);
```

各sampleにPAMを適用し、全$n$点で評価した最良解を返す。sampleは`rng`で
非復元抽出する。$s$をsample size、$r$をsample数とすると、時間計算量は $O(n^2+r((k+Ik)s^2+nk))$、追加空間計算量は $O(n^2+s^2)$。

## `clarans_k_medoids`

```cpp
auto result = clarans_k_medoids(n, k, distance,
                                maximum_neighbor_checks, restarts, rng);
```

ランダムな1-swap近傍を調べ、改善時に採用して失敗回数を0へ戻す。各restartの
局所解から最良を返す。調べた近傍数を$Q$、採用数を$A$、restart数を$R$とすると
時間計算量は $O(n^2+R(A+1)Qnk)$、追加空間計算量は $O(n^2)$。

全APIで$1\le k\le n$が必要である。CLARAは$k\le s\le n$かつ正のsample数、
CLARANSは正の探索上限とrestart数を要求し、違反時は`std::invalid_argument`を
送出する。
距離和が`long double`の範囲を超える場合は`std::overflow_error`を
送出する。

## 注意点

座標列または距離callbackは、各APIで示した次元・有限性・非負性の条件を満たす必要がある。初期値や入力順に依存する手法では、明記した場合を除いて一意な分割や大域最適性を保証しない。
