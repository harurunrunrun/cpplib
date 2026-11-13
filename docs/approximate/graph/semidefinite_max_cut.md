---
title: Semidefinite Max-Cut Rounding (最大カットの半正定値丸め)
documentation_of: ../../../src/approximate/graph/semidefinite_max_cut.hpp
---

呼び出し側で求めた Max-Cut SDP のベクトル解を、超平面でカットに丸める。SDP 自体は解かない。

以下、頂点数・辺数・ベクトルの次元・法線の本数をそれぞれ $N, E, D, T$ とする。

## `semidefinite_rounding`

~~~cpp
template<class Real, class DirectionRange>
MaxCutResult semidefinite_rounding(
    const vector<vector<Real>>& unit_vectors,
    const vector<pair<size_t, size_t>>& edges,
    const DirectionRange& directions
)
~~~

`directions` の各超平面の法線について、法線との内積の符号（0 以上なら側 1）で頂点を二分し、その中で最大のカットを返す。カット辺数が等しい場合は先に現れた法線の結果を採る。法線を固定すれば決定的である。

- 時間計算量: $O(T(ND+E))$
- 追加空間計算量: $O(N)$（入力を除く）

## `goemans_williamson_max_cut`

~~~cpp
template<class Real, class URBG>
MaxCutResult goemans_williamson_max_cut(
    const vector<vector<Real>>& unit_vectors,
    const vector<pair<size_t, size_t>>& edges,
    size_t rounding_trials,
    URBG& random
)
~~~

各成分が独立な標準正規分布に従う法線を `rounding_trials` 本生成し、最良の丸め結果を返す。

ベクトルが SDP の最適な実行可能解なら、1 回の試行は通常の Goemans–Williamson の期待近似保証を持つ。複数回の試行の最大値を取るので結果が悪化することはない。ただし本実装は、SDP の実行可能性・最適性を検証しない。

- 時間計算量: $O(T(ND+E))$
- 追加空間計算量: $O(TD+N)$

## 注意点

- `Real` は浮動小数点型でなければならない（`static_assert`）
- `unit_vectors` が空なら、空の割当てを返す（このとき `rounding_trials` は検査しない）
- 次元 0、次元の不一致、非有限値、零ベクトル、空の `directions`、非有限の法線、`rounding_trials == 0` には `std::invalid_argument` を送出する
- 範囲外の辺端点には例外を送出する（`cut_edge_count` に従う）
