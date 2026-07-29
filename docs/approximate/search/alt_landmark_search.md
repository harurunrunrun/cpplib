---
title: ALT (A*・ランドマーク・三角不等式)
documentation_of: ../../../src/approximate/search/alt_landmark_search.hpp
---

ALT は landmark との最短距離を前計算し、三角不等式から A* の許容的ヒューリスティックを作る。有向グラフでは各 landmark $L$ について $d(L,v)$ と $d(v,L)$ の両方を保存し、

$$
h(s,t)=\max_L\{0,\ d(L,t)-d(L,s),\ d(s,L)-d(t,L)\}
$$

を用いる。

## `AltLandmarkHeuristic`

```cpp
template<class State, class Cost,
         class Hash = std::hash<State>,
         class Equal = std::equal_to<State>>
class AltLandmarkHeuristic;
```

公開メンバー:

- `landmark_count()`, `landmarks()`
- `distances_from(i)`: $d(L_i,v)$ のハッシュ表
- `distances_to(i)`: $d(v,L_i)$ のハッシュ表
- `operator()(from, target)`: ALT 下界
- `hash_function()`, `key_eq()`

距離表を直接渡すコンストラクタは、landmark 数と2種類の表の数が一致しなければ `std::invalid_argument`。

## 前計算

```cpp
make_alt_landmark_heuristic(
    landmarks,
    forward_neighbors, backward_neighbors,
    hash = {}, equal = {}
)
```

各 landmark から通常向きと逆向きの Dijkstra を1回ずつ実行する。近傍 callable は `pair<State, Cost>` の range を返す。到達できない状態は対応する表に存在せず、その landmark の該当差分を使わない。

landmark 数を $k$、前計算で到達する頂点・辺を $V,E$ とすると、期待時間計算量は $O(k(E+V)\log V)$、空間計算量は $O(kV)$。ハッシュ表操作は期待 $O(1)$。

## ALT A*

```cpp
alt_a_star_search(start, target, neighbors, landmark_heuristic)
```

前計算済みヒューリスティックを使って A* を実行し、`HeuristicSearchResult<State, Cost>` を返す。1回のヒューリスティック評価は $O(k)$。生成辺数 $A$、展開数 $X$、最大キュー長 $Q$ に対し時間計算量は $O(Ak+(A+X)\log Q)$、探索追加空間は $O(V_d+Q)$。

## 注意点

- Dijkstra 前計算のため全辺コストは非負でなければならない。
- `backward_neighbors` は元の入辺を逆向きに同コストで返す。
- 距離の加減算は `Cost` の表現可能範囲に収まる必要がある。
- landmark 0個も有効で、その場合ヒューリスティックは常に0。
- landmark の選択は正しさを変えないが、遠く分散した landmark は通常より強い下界を与える。

## English

ALT precomputes distances to and from landmarks and derives an admissible A* heuristic from the triangle inequality. For directed graphs it uses both `d(L,v)` and `d(v,L)`.

`make_alt_landmark_heuristic` runs forward and reverse Dijkstra from every landmark and returns `AltLandmarkHeuristic`. Preprocessing takes $O(k(E+V)\log V)$ expected time and $O(kV)$ space. `alt_a_star_search` evaluates the resulting $O(k)$ heuristic during A* and returns `HeuristicSearchResult<State, Cost>`. All arcs must be non-negative; reverse neighbors must represent reversed original arcs with identical costs.
