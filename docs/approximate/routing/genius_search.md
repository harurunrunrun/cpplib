---
title: GENI, US, GENIUS and Record-to-Record Travel (GENI・US・GENIUS・Record-to-Record 探索)
documentation_of: ../../../src/approximate/routing/genius_search.hpp
---

対称 TSP の巡回路に対する一般化挿入と局所探索を提供する。巡回路は、始点を末尾に重複させない $[0, N)$ の順列で表す。以下、$P$ を `neighborhood_size`、$I$ をパス数の上限とする。

## `geni_tour`

~~~cpp
vector<int> geni_tour(
    const DistanceMatrix& distance,
    size_t neighborhood_size = 5,
    size_t start = 0
)
~~~

各未挿入頂点について、通常の 1 辺を分割する挿入と、近傍頂点の対の間を反転して 3 辺を張り替える一般化挿入とを比較する。全候補の中で巡回路コストの増分が最小となる頂点と位置を選ぶことを繰り返す。

- 時間計算量: $O(N^3 \log N + N^2 P^2)$
- 追加空間計算量: $O(N)$

## `unstringing_stringing_tour`

~~~cpp
vector<int> unstringing_stringing_tour(
    const DistanceMatrix& distance,
    vector<int> tour,
    size_t maximum_passes = 8,
    size_t neighborhood_size = 5
)
~~~

各頂点をいったん巡回路から外し、GENI の一般化挿入で最良の位置へ戻す（US）。巡回路コストが真に減少する変更だけを採用し、改善のないパスが生じたら終了する。

- 時間計算量: $O(IN^2(P^2 + N))$
- 追加空間計算量: $O(N)$

## `genius_tour`

~~~cpp
vector<int> genius_tour(
    const DistanceMatrix& distance,
    size_t neighborhood_size = 5,
    size_t maximum_us_passes = 8,
    size_t start = 0
)
~~~

GENI で初期巡回路を構築し、その結果に US を適用する。

- 時間計算量: $O(N^3 \log N + N^2 P^2 + IN^2(P^2 + N))$
- 追加空間計算量: $O(N)$

## `record_to_record_travel_tour`

~~~cpp
vector<int> record_to_record_travel_tour(
    const DistanceMatrix& distance,
    vector<int> tour,
    long double maximum_deviation,
    size_t maximum_passes = 20
)
~~~

2-opt 近傍を決定的に走査し、これまでの最良値（record）から `maximum_deviation` 以内の候補への移動を許す。探索中の最良の巡回路を返すため、返値は入力の巡回路より悪くならない。非対称な距離でも、反転後のコスト全体を再計算して判定する。

- 時間計算量: $O(IN^3)$
- 追加空間計算量: $O(N)$

## 注意点

- GENI・US・GENIUS は対称な正方距離行列を要求し、非対称なら `std::invalid_argument` を送出する。
- `neighborhood_size == 0`、有限でないか負の `maximum_deviation`、順列でない巡回路にも `std::invalid_argument` を送出する。巡回路や始点に範囲外の頂点があれば `std::out_of_range` を、$N$ が `INT_MAX` を超える場合は `std::length_error` を送出する。
- 距離は大小比較と `long double` への変換ができなければならない。
