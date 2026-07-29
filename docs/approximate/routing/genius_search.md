---
title: GENI, US, GENIUS and Record-to-Record Travel (GENI・US・GENIUS・Record-to-Record探索)
documentation_of: ../../../src/approximate/routing/genius_search.hpp
---

対称TSP巡回路の一般化挿入と局所探索を提供する。巡回路は始点を末尾に重複させない `[0,N)` の置換で表す。

## `geni_tour`

~~~cpp
vector<int> geni_tour(
    const DistanceMatrix& distance,
    size_t neighborhood_size = 5,
    size_t start = 0
)
~~~

各未挿入頂点について、通常の1辺分割挿入と、近傍頂点対の間を反転して3辺を張り替える一般化挿入を比較する。全候補中で巡回路増分が最小の頂点・位置を反復して選ぶ。

- 時間計算量: $O(N^3\log N+N^2P^2)$（`P = neighborhood_size`）
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

各頂点を一度巡回路から外し、GENIの一般化挿入で最良位置へ戻す。巡回路コストを厳密に減少させる変更だけを採用し、改善がないpassで終了する。

- 時間計算量: $O(IN^2(P^2+N))$（`I = maximum_passes`）
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

GENIで初期巡回路を構築し、その結果にUSを適用する。

- 時間計算量: $O(N^3\log N+N^2P^2+IN^2(P^2+N))$
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

2-opt近傍を決定的に走査し、現在までのrecordから `maximum_deviation` 以内の候補を移動先として許す。探索中の最良巡回路を返すため、返り値は入力巡回路より悪化しない。非対称距離でも反転後の全コストを再計算して判定する。

- 時間計算量: $O(IN^3)$
- 追加空間計算量: $O(N)$

## 注意点

GENI・US・GENIUSは対称な正方距離行列を要求し、非対称なら `std::invalid_argument`。`neighborhood_size=0`、非有限または負のdeviation、置換でない巡回路も `std::invalid_argument`。巡回路または始点の範囲外頂点は `std::out_of_range`。
`N > INT_MAX`: `std::length_error`.
距離の大小比較と `long double` への変換が可能でなければならない。
