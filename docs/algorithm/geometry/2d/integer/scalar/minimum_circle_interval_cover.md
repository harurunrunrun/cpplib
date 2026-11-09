---
title: Minimum Circle Interval Cover (円周区間最小被覆)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/scalar/minimum_circle_interval_cover.hpp
---

円周上の候補区間から、全周を覆うのに必要な最小本数を求める。

## API

~~~cpp
int minimum_circle_interval_cover(
    const std::vector<std::pair<long double, long double>>& intervals
)
~~~

各 `pair` は、`start` から `end` へ反時計回りに進む半開区間を表す。
全周を覆えない場合は `-1` を、覆える場合は必要な候補区間の最小本数を返す。同じ候補を複数回選ぶことはできない。

## 時間計算量

候補数を $N$ として $O(N \log N)$。区間を 2 周分に複製してソートし、貪欲法の遷移をダブリングで高速化する。

## 空間計算量

$O(N \log N)$。

## 注意点

- 端点はラジアンで指定し、$2\pi$ の周期でどの表現を用いてもよい。非有限の端点があれば `std::invalid_argument` を送出する。ただし全周区間を見つけた時点で 1 を返すので、それ以降の候補は検査しない。
- $\lvert \mathrm{end} - \mathrm{start} \rvert \ge 2\pi - \texttt{GEOMETRY_EPS}$ の候補は全周区間とみなし、答えは 1 になる。
- 正規化した長さが `GEOMETRY_EPS` 以下の候補は空区間として無視する。
- 0 をまたぐ区間や端点を共有する区間も扱う。被覆の判定には `GEOMETRY_EPS` の許容誤差を用いる。
