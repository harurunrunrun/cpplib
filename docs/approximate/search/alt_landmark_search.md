---
title: ALT (A*・ランドマーク・三角不等式)
documentation_of: ../../../src/approximate/search/alt_landmark_search.hpp
---

ALT は、ランドマーク（landmark）との最短距離を前計算し、三角不等式から A* の許容的なヒューリスティックを作る。有向グラフでは各ランドマーク $L$ について $d(L,v)$ と $d(v,L)$ の両方を保存し、

$$
h(s,t)=\max_L\max\{0,\ d(L,t)-d(L,s),\ d(s,L)-d(t,L)\}
$$

を用いる。

## `AltLandmarkHeuristic`

```cpp
template<class State, class Cost,
         class Hash = std::hash<State>,
         class Equal = std::equal_to<State>>
class AltLandmarkHeuristic;
```

公開メンバ:

- `landmark_count()`, `landmarks()`
- `distances_from(i)`: $d(L_i,v)$ のハッシュ表
- `distances_to(i)`: $d(v,L_i)$ のハッシュ表
- `operator()(from, target)`: ALT 下界
- `hash_function()`, `key_eq()`

距離表を直接渡すコンストラクタは、ランドマーク数と 2 種類の表の数が一致しなければ `std::invalid_argument` を送出する。

## 前計算

```cpp
make_alt_landmark_heuristic(
    landmarks,
    forward_neighbors, backward_neighbors,
    hash = {}, equal = {}
)
```

各ランドマークから通常の向きと逆向きの Dijkstra 法を 1 回ずつ実行する。近傍関数は `pair<State, Cost>` の範囲を返す。到達できない状態は対応する表に存在せず、そのランドマークの該当する差分は使わない。

ランドマーク数を $k$、前計算で到達する頂点数・辺数を $V$, $E$ とすると、期待時間計算量は $O(k(E+V)\log V)$、空間計算量は $O(kV)$。ハッシュ表の操作は期待 $O(1)$ とする。

## ALT A*

```cpp
alt_a_star_search(start, target, neighbors, landmark_heuristic)
```

前計算済みのヒューリスティックを使って A* を実行し、`HeuristicSearchResult<State, Cost>` を返す。1 回のヒューリスティック評価は $O(k)$。生成辺数を $A$、展開数を $X$、キューの最大長を $Q$、発見状態数を $V_d$ とすると、時間計算量は $O(Ak+(A+X)\log Q)$、探索の追加空間計算量は $O(V_d+Q)$。

## 注意点

- Dijkstra 法で前計算するため、全辺のコストは非負でなければならない。負の辺には `std::invalid_argument` を送出する。
- `backward_neighbors` は、元の入辺を逆向きに同じコストで返さなければならない。
- 距離の加減算は `Cost` の表現範囲に収まらなければならない。
- ランドマーク 0 個も有効で、その場合ヒューリスティックは常に 0 である。
- ランドマークの選び方は正しさに影響しないが、遠くに分散したランドマークほど通常は強い下界を与える。
