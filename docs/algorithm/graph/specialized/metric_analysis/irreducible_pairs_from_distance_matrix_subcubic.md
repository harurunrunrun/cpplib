---
title: Subcubic Irreducible Pairs from Distance Matrix (距離行列の既約な頂点対の三乗未満時間列挙)
documentation_of: ../../../../../src/algorithm/graph/specialized/metric_analysis/irreducible_pairs_from_distance_matrix_subcubic.hpp
---

非負整数の有限距離行列から、第三の頂点を経由する等長な分解を持たない頂点対を厳密に列挙する。値域の上限を引数に取らず、`long long` の非負範囲全体を扱える理論的な高速版である。

## 関数

```cpp
std::vector<std::pair<int, int>>
irreducible_pairs_from_distance_matrix_subcubic(
    const std::vector<std::vector<long long>>& distances
);
```

`left < right` かつ、すべての `middle != left, right` に対して

$$
D[\mathit{left}][\mathit{right}]
\ne D[\mathit{left}][\mathit{middle}] + D[\mathit{middle}][\mathit{right}]
$$

となる組（$D$ は `distances`）だけを辞書順で返す。`middle == left` と `middle == right` による自明な等式は中継点（witness）として数えない。

- 時間計算量: 最悪 $O(N^3/\log N)$
- 追加空間計算量: $O(N^2)$
- 戻り値の組数を $K$ とすると、戻り値自体は $O(K)$

$N<2$ では空の `std::vector` を返す。$N$ が小さいときはブロック幅が $1$ になるため、通常の三重ループに近い動作になる。この API は最悪計算量の漸近的な改善を目的としたもので、通常の競技プログラミングの入力では `irreducible_pairs_from_distance_matrix` の方が高速なことが多い。

## 正当性

### ブロック内で和を最小にする中継点

中継点を連続したブロック $K$ に分ける。固定した候補 $k\in K$ と競合候補 $h\in K\setminus\lbrace k\rbrace$ に対し、行 $i$ の赤座標と列 $j$ の青座標を

$$
R_{i,h}^{(k)}
=2(D[i][k]-D[i][h])+[k>h],
$$

$$
B_{j,h}^{(k)}
=2(D[h][j]-D[k][j])
$$

とする。すべての $h$ について $R_{i,h}^{(k)}\le B_{j,h}^{(k)}$ であることは、

$$
D[i][k]+D[k][j]
<
D[i][h]+D[h][j]
$$

であるか、または両辺が等しく $k<h$ であることと同値である。したがって赤点 $i$ が青点 $j$ を全座標で支配（dominate）する候補 $k$ は、ブロック内で和を最小にする中継点のうち番号最小のものただ 1 つである。差と和は `__int128_t` で計算するため、`long long` の減算・加算のオーバーフローは起きない。

各候補について支配対は、最後の座標の中央値で赤点・青点を分割し、

1. 左側の赤点と左側の青点
2. 右側の赤点と右側の青点
3. 最後の座標を落とした、左側の赤点と右側の青点

を再帰的に処理して報告する。座標が等しい場合は赤点を青点より前に置くため、非狭義の不等号を正しく扱える。中央値は median-of-medians により最悪線形時間で選ぶ。ハッシュ、乱択、入力の値域に依存する符号化は使わない。

### 自明な端点の除外

頂点対 $(i,j)$ の両端点がブロック $K$ の外にあれば、得られた最小候補 $k\in K$ は必ず $k\ne i,j$ を満たす。この $k$ を経由する和が $D[i][j]$ と等しいとき、かつそのときに限り、$K$ 内に第三頂点の中継点がある。和が $D[i][j]$ より小さければ三角不等式違反である。

$i$ または $j$ を含むブロックでは、端点自身が常に自明な最小候補となり、同じ和を持つ第三頂点を隠してしまう可能性がある。このような高々 2 個のブロックだけは、$i,j$ を除いてブロック内を直接走査する。よって自明な $k=i,j$ を中継点に数えず、すべての第三頂点について等式と狭義の不等式の両方を検査できる。

## 最悪時間計算量の証明

ブロック幅を

$$
B=\max(1,\lfloor\log_2 N/4\rfloor)
$$

とする（実装では $\lfloor \log_2 N \rfloor$ を $4$ で割って切り捨てる）。固定した候補の支配問題は高々 $d=B-1$ 次元で、赤点・青点を合わせて $O(N)$ 点である。

出力時間を除く支配対の再帰の時間を $T_d(s)$ とする。中央値で分割した後の同じ次元の 2 つの子は点数の合計が $s$、射影した 1 つの子の点数は高々 $s$ なので、

$$
T_d(s)
\le
T_d(\lceil s/2\rceil)
+T_d(\lfloor s/2\rfloor)
+T_{d-1}(s)
+O(s).
$$

$a=\lceil\log_2 s\rceil$ とし、$S(a,d)=T_d(2^a)/2^a$ と正規化すると、

$$
S(a,d)\le S(a-1,d)+S(a,d-1)+O(1).
$$

したがって格子路の数で評価して

$$
T_d(s)
=O\left(
s\binom{a+d}{d}
\right).
$$

$d\le a/4+O(1)$ では二項係数のエントロピー上界から

$$
\binom{a+d}{d}
\le
2^{(5a/4)H(1/5)+O(1)}
<
2^{0.903a+O(1)}.
$$

よって固定した候補 1 つあたりの出力以外の時間は $O(N^{1.903})$ であり、全ブロックの候補は合計 $N$ 個なので $O(N^{2.903})=o(N^3/\log N)$ である。

タイブレークにより、1 つのブロックで各順序付き頂点対が報告される回数はちょうど 1 回である。したがって報告の総数は 1 ブロックあたり $N^2$、$O(N/B)$ 個のブロック全体で

$$
O(N^3/B)=O(N^3/\log N).
$$

端点を含むブロックの直接走査は各頂点対につき高々 $2B$ 候補なので $O(N^2B)$ である。入力の検査は $O(N^2)$、出力の構築も $O(N^2)$ である。以上を合わせて、最悪時間計算量は $O(N^3/\log N)$ となる。

この解析は、Chan による任意実数重みの min-plus 積に対する高次元支配対への帰着を、第三頂点の中継点の検出に適用したものである。参考: [T. M. Chan, All-Pairs Shortest Paths with Real Weights in O(n^3/log n) Time](https://doi.org/10.1007/s00453-007-9062-1)。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `irreducible_pairs_from_distance_matrix_subcubic` | 最悪 $O(N^3/\log N)$ | $O(N^2)$ |

入力行列の保持領域は追加空間に含めない。支配対の再帰で使う一時的な `std::vector` は $O(N\log^2 N)$ 以下で、$O(N^2)$ の最小候補表と判定表に支配される。

## 使用例

```cpp
std::vector<std::vector<long long>> distances{
    {0, 2, 5, 9},
    {2, 0, 3, 7},
    {5, 3, 0, 4},
    {9, 7, 4, 0},
};

const auto pairs =
    irreducible_pairs_from_distance_matrix_subcubic(distances);
// (0,1), (1,2), (2,3) が返る。
```

## 注意点

- 入力は正方・非負・対称・対角成分 $0$ で、すべての第三頂点について三角不等式を満たす必要がある。
- 頂点番号は 0-indexed である。
- 距離値の上限を表す引数はない。各成分には $0$ 以上 `LLONG_MAX` 以下の値を使用できる。
- 和・差・タイブレーク用の符号化は符号付き 128 bit 整数で厳密に比較する。
- 単一の include で使用でき、外部依存や乱択を必要としない。

例外を送出する条件は次のとおりである。

- 行列が正方でない: `std::invalid_argument`
- 負の成分がある: `std::invalid_argument`
- 対角成分が $0$ でない: `std::invalid_argument`
- 対称でない: `std::invalid_argument`
- 第三頂点を経由する和が直接の距離より小さい: `std::invalid_argument`
- 頂点数が `int` に収まらない、または $N^2$ が `std::size_t` に収まらない: `std::length_error`
- 内部の「最小候補が一意である」という不変条件が破れた場合: `std::logic_error`
- メモリ確保に失敗した場合: 標準ライブラリの確保例外
