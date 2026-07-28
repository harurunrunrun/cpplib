---
title: General b-Matching (一般グラフb-マッチング)
documentation_of: ../../../../src/algorithm/matching/general/general_b_matching.hpp
---

一般無向多重グラフで、各頂点の次数上限を守りながら選ぶ辺数を最大化する。自己ループは頂点容量を2消費する。

## API

```cpp
GeneralBMatching matching(capacity);
```

各頂点の非負次数上限を指定する。

```cpp
int edge_id = matching.add_edge(first, second);
```

単位容量の無向辺を追加する。多重辺と自己ループを許す。

```cpp
GeneralBMatchingResult result = matching.solve();
```

最大辺数 `size`、選択した入力辺番号 `selected_edge_ids`、各頂点の使用次数 `degree` を返す。

## 時間計算量

$B=\sum_v b(v)$、元の辺数を $E$ とする。Tutte型補助グラフの頂点数は $V'=B+2E$、辺数は

$$
E'=E+\sum_{uv\in E}(b(u)+b(v))
$$

であり、Micali--Vazirani法により $O(E'\sqrt{V'})$ 時間、$O(V'+E')$ 空間。

## 注意点

容量は非負整数である。補助グラフの頂点数が `int` を超える場合は `length_error`、頂点番号の違反では `runtime_error` を送出する。
