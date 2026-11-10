---
title: General b-Matching (一般グラフb-マッチング)
documentation_of: ../../../../src/algorithm/matching/general/general_b_matching.hpp
---

一般の無向多重グラフで、各頂点の次数上限を守りながら選ぶ辺の本数を最大化する。自己ループは頂点の容量を $2$ 消費する。

## API

```cpp
GeneralBMatching matching(capacity);
```

各頂点の非負の次数上限 `capacity`（`vector<int>`）を指定して構築する。頂点数は `capacity` の長さで決まる。

```cpp
int edge_id = matching.add_edge(first, second);
```

単位容量の無向辺を追加し、その番号を返す。多重辺と自己ループを許す。

```cpp
GeneralBMatchingResult result = matching.solve();
```

選んだ辺の最大本数 `size`、選んだ入力辺の番号 `selected_edge_ids`、各頂点の使用次数 `degree` を返す。

## 時間計算量

$B = \sum_v b(v)$、元の辺数を $E$ とする。Tutte 型の補助グラフの頂点数は $V' = B + 2E$、辺数は

$$
E' = E + \sum_{uv \in E} (b(u) + b(v))
$$

であり、Micali–Vazirani 法により時間 $O(E' \sqrt{V'})$、空間 $O(V' + E')$。

## 注意点

容量は非負の整数でなければならず、負の容量や範囲外の頂点番号に対しては `runtime_error` を送出する。補助グラフの頂点数が `int` の範囲を超える場合は `length_error` を送出する。
