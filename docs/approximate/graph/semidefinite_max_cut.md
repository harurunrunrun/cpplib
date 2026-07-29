---
title: Semidefinite Max-Cut Rounding (最大カットの半正定値丸め)
documentation_of: ../../../src/approximate/graph/semidefinite_max_cut.hpp
---

This header rounds a caller-supplied vector solution of the Max-Cut SDP.
このヘッダは、呼び出し側で求めた Max-Cut SDP のベクトル解をカットへ丸める。SDP 自体は解かない。

## `semidefinite_rounding`

~~~cpp
template<class Real, class DirectionRange>
MaxCutResult semidefinite_rounding(
    const vector<vector<Real>>& unit_vectors,
    const vector<pair<size_t, size_t>>& edges,
    const DirectionRange& directions
)
~~~

For every supplied hyperplane normal, vertices are separated by the sign of
their dot product; the best resulting cut is returned.
各超平面の法線との内積の符号で頂点を二分し、入力した法線の中で最大のカットを返す。法線を固定すれば決定的である。

Let \(N,E,D,T\) be the numbers of vertices, edges, coordinates, and directions.

- Time / 時間計算量: \(O(T(ND+E))\)
- Extra space / 追加空間計算量: \(O(N)\), excluding input / 入力を除く

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

Draws independent Gaussian normals and returns the best rounded cut.
独立な標準 Gaussian 法線を `rounding_trials` 本生成し、最良の丸め結果を返す。

When the vectors are an optimal feasible SDP solution, one random trial has the
usual Goemans--Williamson expected guarantee; extra trials cannot reduce the
returned cut value.  The implementation does not certify SDP feasibility or
optimality.
ベクトルが最適な SDP 実行可能解なら、1 trial は通常の Goemans--Williamson の期待近似保証を持つ。複数 trial の最大値は結果を悪化させないが、本実装は SDP の実行可能性・最適性を検証しない。

- Time / 時間計算量: \(O(T(ND+E))\)
- Extra space / 追加空間計算量: \(O(TD+N)\)

## 注意点

Dimensions, finite vector entries, nonzero vectors, positive trial count, and
edge endpoints are validated.
次元不一致、非有限値、零ベクトル、0 trial、不正な辺端点には例外を送出する。
