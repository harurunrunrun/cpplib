---
title: Advanced Heuristic Search (高度ヒューリスティック探索)
documentation_of: ../../../src/approximate/search/advanced_heuristic_search.hpp
---

非負辺グラフ上で、解の品質と探索量を調整する汎用探索群。状態、ゴール判定、近傍生成、ヒューリスティックを callable として受け取る。

`neighbors(state)` は `pair<State, Cost>` の range、`goal(state)` は真偽値、ヒューリスティックは非負の推定値を返す。`Hash` と `Equal` を省略した場合は `std::hash<State>` と `std::equal_to<State>` を使う。

## `AnytimeHeuristicSearchResult`

```cpp
template<class State, class Cost>
struct AnytimeHeuristicSearchResult {
    vector<HeuristicSearchResult<State, Cost>> solutions;
    size_t expanded;
    explicit operator bool() const noexcept;
    const HeuristicSearchResult<State, Cost>& best() const;
};
```

- `solutions`: 発見順の改善解。末尾が最良解。
- `expanded`: 探索全体の展開回数。
- `best()`: 最良解への参照。解がなければ `std::logic_error`。

`operator bool` と `best()` は $O(1)$、解の記録は経路長を $L$ として $O(L)$。

## `anytime_a_star_search`

```cpp
anytime_a_star_search(start, goal, neighbors, heuristic, weight,
                      hash = {}, equal = {})
```

最初は $g+w h$ で早い解を探し、解の発見後も $g+h$ が現暫定解を改善できる状態を探索する。許容的ヒューリスティックなら最後の解は最適となる。`weight >= 1` が必要。

生成辺数を $A$、展開回数を $X$、キュー最大要素数を $Q$ とすると、時間計算量は $O((A+X)\log Q+\sum L_i)$、空間計算量は $O(V_d+Q+\sum L_i)$。$L_i$ は各改善経路長、$V_d$ は発見状態数。

## `ara_star_search`

```cpp
ara_star_search(start, goal, neighbors, heuristic, weights,
                hash = {}, equal = {})
```

非増加な重み列に従う Anytime Repairing A*。発見済みの $g$ 値を保持し、暫定解改善時に OPEN を次の重みで再構築する。探索を最初からやり直さない。重み列は空でなく、全要素が `>= 1` でなければならない。

重み段階数を $I$ とすると、時間計算量は $O((A+X+I V_d)\log V_d+\sum L_i)$、空間計算量は $O(V_d+\sum L_i)$。

## `ana_star_search`

```cpp
ana_star_search(start, goal, neighbors, heuristic, hash = {}, equal = {})
```

解発見後に改善可能性 $(G-g)/h$ が大きい状態を優先する Anytime Nonparametric A*。$G$ は暫定解コスト。許容的ヒューリスティックなら最後の解は最適となる。

改善回数を $I$ とすると、OPEN 再構築を含む時間計算量は $O((A+X+I V_d)\log V_d+\sum L_i)$、空間計算量は $O(V_d+\sum L_i)$。

## `focal_search`

```cpp
focal_search(start, goal, neighbors, heuristic, secondary, factor,
             hash = {}, equal = {})
```

OPEN の最小 $f=g+h$ に対し、$f\leq\text{factor}\cdot f_{\min}$ の FOCAL から `secondary(state)` が最小の状態を展開する。許容的ヒューリスティックなら返すコストは最適値の `factor` 倍以下。`factor >= 1` が必要。

時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(V_d+A_q)$。$A_q$ は遅延削除待ちのキュー要素数。

## `a_epsilon_search`

```cpp
a_epsilon_search(start, goal, neighbors, heuristic, epsilon,
                 hash = {}, equal = {})
```

$f\leq(1+\varepsilon)f_{\min}$ の FOCAL 内で $h$ が小さい状態を優先する Aε。許容的ヒューリスティックなら $(1+\varepsilon)$ 近似解を返す。`epsilon >= 0` が必要。

時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(V_d+A_q)$。

## `explicit_estimation_search`

```cpp
explicit_estimation_search(start, goal, neighbors, heuristic,
    estimated_heuristic, distance_estimate, factor,
    hash = {}, equal = {})
```

許容的下界 `heuristic` で全体の品質境界を管理し、推定コスト `g + estimated_heuristic(state)` が境界内の状態から `distance_estimate(state)` が小さいものを選ぶ。推定誤差をコストと残り手数で分離して与えられる。

時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(V_d+A_q)$。品質保証には `factor >= 1` と許容的な `heuristic` が必要。

## `optimistic_search`

```cpp
optimistic_search(start, goal, neighbors, heuristic, factor,
                  hash = {}, equal = {})
```

許容的な $g+h$ を品質境界に保ちながら、境界内では $g+\text{factor}\,h$ を優先する。`factor >= 1` が必要で、許容的ヒューリスティックなら `factor` 倍品質を保証する。

時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(V_d+A_q)$。

## `potential_search`

```cpp
potential_search(start, goal, neighbors, heuristic, exclusive_bound,
                 hash = {}, equal = {})
```

厳密な上限 `exclusive_bound` 内で、残予算当たりの推定量 $h/(\text{bound}-g)$ が小さい状態を優先する。上限未満の解がなければ未発見を返す。

時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(V_d)$。

## `multi_heuristic_a_star_search`

```cpp
multi_heuristic_a_star_search(start, goal, neighbors, heuristics,
                              weight, hash = {}, equal = {})
```

`heuristics(state)` が返す非空 range の最小推定値を用いて $g+w\min_i h_i$ を優先する。複数の下界のうち状態ごとに有効なものを選べる。`weight >= 0` が必要。

ヒューリスティック数を $K$ とすると、時間計算量は $O((A+X)\log V_d+K V_d)$、空間計算量は $O(V_d)$。

## `mha_star_search`

```cpp
mha_star_search(start, goal, neighbors, anchor_heuristic,
    additional_heuristics, weight1, weight2, hash = {}, equal = {})
```

許容的な anchor の $g+h_0$ で品質境界を管理し、その `weight2` 倍以内から $g+\text{weight1}\,h_i$ が最小の状態を展開する Multi-Heuristic A*。追加ヒューリスティックは非許容でもよい。両重みは `>= 1` が必要。

追加ヒューリスティック数を $K$ とすると、時間計算量は $O((A+X)\log V_d+K V_d)$、空間計算量は $O(V_d+A_q)$。許容的 anchor の下で返すコストは最適値の `weight2` 倍以下。

## `dynamic_weighted_a_star_search`

```cpp
dynamic_weighted_a_star_search(start, goal, neighbors, heuristic,
    epsilon, expected_solution_depth, hash = {}, equal = {})
```

深さ $d$ に対し、推定深さまで重みを
$1+\varepsilon(1-\min(1,d/N))$ と線形に減らす Dynamic Weighted A*。探索前半の貪欲性を高め、終盤は A* に近づける。`epsilon >= 0` かつ `expected_solution_depth > 0` が必要。

時間計算量は $O((A+X)\log V_d)$、空間計算量は $O(V_d)$。

## 注意点

- 負辺、負のヒューリスティック、各 API の範囲外パラメータは `std::invalid_argument`。
- `Cost` の加算と `long double` への変換が必要であり、値は表現可能範囲に収まらなければならない。
- 最適性・近似率の保証には、明記した許容性が必要。FOCAL の単調な更新効率には整合的ヒューリスティックが望ましい。
- `neighbors` とヒューリスティックは探索中に同一状態へ複数回呼ばれ得る。
