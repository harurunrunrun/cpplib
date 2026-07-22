---
title: Subcubic Irreducible Pairs from Distance Matrix (距離行列の部分三乗既約対)
documentation_of: ../src/algorithm/graph/specialized/irreducible_pairs_from_distance_matrix_subcubic.hpp
---

非負整数の有限距離行列から、第三の頂点を経由する等長な分解を持たない頂点対を厳密に列挙する。値域上限を引数に取らず、`long long` の非負範囲全体を扱う理論的高速版である。

## 関数

```cpp
std::vector<std::pair<int, int>>
irreducible_pairs_from_distance_matrix_subcubic(
    const std::vector<std::vector<long long>>& distances
);
```

`left < right` かつ、すべての `middle != left, right` に対して

$$
D[left][right]
\ne D[left][middle] + D[middle][right]
$$

となる組だけを辞書順で返す。`middle == left` と `middle == right` による自明な等式はwitnessに数えない。

- 時間計算量: worst-case $O(N^3/\log N)$
- 追加空間計算量: $O(N^2)$
- 戻り値の組数を $K$ とすると、戻り値自体は $O(K)$

$N<2$ では空vectorを返す。小さい $N$ ではblock幅を1にするため、通常の三重走査に近い動作になる。このAPIは漸近的なworst-case改善を目的とし、通常の競技入力では `irreducible_pairs_from_distance_matrix` の方が高速なことが多い。

## 正当性

### block内最小中継点

中継点を連続したblock $K$ に分ける。固定した候補 $k\in K$ と競合候補 $h\in K\setminus\{k\}$ に対し、行 $i$ のred座標と列 $j$ のblue座標を

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
D[i][h]+D[h][j],
$$

または両辺が等しく $k<h$ であることと同値である。したがってred点 $i$ がblue点 $j$ を全座標でdominateする候補 $k$ は、block内で和を最小化する最小番号の中継点ただ1つである。差と和は `__int128_t` で作るため、`long long` の減算・加算overflowに依存しない。

各候補のdominating pairは、最後の座標の中央値でred/blue点を分割し、

1. left red と left blue
2. right red と right blue
3. 最後の座標を落とした left red と right blue

を再帰的に処理して報告する。等しい座標ではredをblueより前に置くため、非狭義不等号を正しく扱う。中央値はmedian-of-mediansでworst-case線形時間に選ぶ。hash、乱択、入力値域に依存する符号化は使わない。

### 自明なendpointの除外

頂点対 $(i,j)$ の両endpointがblock $K$ の外なら、得られたwinner $k\in K$ は必ず $k\ne i,j$ である。このwinnerの和が $D[i][j]$ と等しい場合、かつその場合に限り、$K$ 内に第三頂点witnessがある。winnerの和が小さければ三角不等式違反である。

$i$ または $j$ を含むblockでは、endpointが常に自明な最小候補となり、同値な第三頂点を隠し得る。この高々2 blockだけは、$i,j$ を除外してblock内を直接走査する。よって自明な $k=i,j$ をwitnessに数えず、全第三頂点について等式と狭い不等式の両方を検査できる。

## worst-case時間計算量の証明

block幅を

$$
B=\max(1,\lfloor\log_2 N/4\rfloor)
$$

とする。固定候補のdominance問題は高々 $d=B-1$ 次元、red/blueを合わせて $O(N)$ 点である。

出力時間を除くdominance再帰の時間を $T_d(s)$ とする。中央値分割後の同一次元の2子は点数の合計が $s$、射影した1子の点数は高々 $s$ なので、

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

従って格子路の数で評価して

$$
T_d(s)
=O\!\left(
s\binom{a+d}{d}
\right).
$$

$d\le a/4+O(1)$ では二項係数のentropy boundから

$$
\binom{a+d}{d}
\le
2^{(5a/4)H(1/5)+O(1)}
<
2^{0.903a+O(1)}.
$$

よって固定候補の非出力時間は $O(N^{1.903})$、全blockの全候補は合計 $N$ 個なので $O(N^{2.903})=o(N^3/\log N)$ である。

tie-breakにより、1 blockで各順序付き頂点対が報告される回数はちょうど1回である。従って報告総数は1 blockあたり $N^2$、$O(N/B)$ block全体で

$$
O(N^3/B)=O(N^3/\log N).
$$

endpointを含むblockの直接走査は各頂点対につき高々 $2B$ 候補なので $O(N^2B)$。入力検査は $O(N^2)$、出力構築も $O(N^2)$ である。以上を合わせたworst-case時間計算量は $O(N^3/\log N)$。

この解析は Chan の任意実数min-plus積に対する高次元dominance reductionを、第三頂点witnessの検出へ適用したものである。参考: [T. M. Chan, All-Pairs Shortest Paths with Real Weights in O(n^3/log n) Time](https://doi.org/10.1007/s00453-007-9062-1)。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
|---|---:|---:|
| `irreducible_pairs_from_distance_matrix_subcubic` | worst-case $O(N^3/\log N)$ | $O(N^2)$ |

入力行列の保持領域は追加空間に含めない。dominance再帰の一時vectorは $O(N\log^2 N)$ 以下で、$O(N^2)$ のwinner表と判定表に支配される。


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

送出条件は次のとおり。

- 行列が正方でない: `std::invalid_argument`
- 負の成分がある: `std::invalid_argument`
- 対角成分が0でない: `std::invalid_argument`
- 対称でない: `std::invalid_argument`
- 第三頂点を経由する和が直接距離より小さい: `std::invalid_argument`
- 頂点数が `int` に収まらない、または $N^2$ が `std::size_t` に収まらない: `std::length_error`
- 内部の一意winner不変条件が破れた場合: `std::logic_error`
- メモリ確保に失敗した場合: 標準ライブラリの確保例外

- 入力は正方・非負・対称・対角成分0で、全第三頂点について三角不等式を満たす必要がある。
このほか、次の点に注意する。

- 頂点番号は0-indexed。
- 距離値の上限引数はない。各成分は `0` 以上 `LLONG_MAX` 以下を使用できる。
- 和・差・tie-break符号化は符号付き128 bitで厳密に比較する。
- 単一includeで使用でき、外部依存や乱択を必要としない。
