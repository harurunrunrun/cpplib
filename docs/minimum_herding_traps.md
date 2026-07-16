---
title: Minimum Herding Traps (誘導格子最小罠数) [HERDING]
documentation_of: ../src/algorithm/graph/minimum_herding_traps.hpp
---

各マスから移動方向が一意に決まる格子で、どの開始マスからも最終的に捕獲するために必要な罠の最小数を返す。

## minimum_herding_traps

```cpp
int minimum_herding_traps(const vector<string>& directions);
```

各文字 `N`, `S`, `W`, `E` が次に移動する隣接マスを表す。任意のマスから指示に従って移動し続ける対象をすべて捕獲できる最小罠数を返す。

## API別の時間計算量・空間計算量

格子の行数を $R$、列数を $C$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_herding_traps` | $O(RC)$ | $O(RC)$ |

## 注意点

- 格子は空でなく、全行の長さが等しくなければならない。
- 各文字は `N`, `S`, `W`, `E` のいずれかで、指す隣接マスが格子内になければならない。
- 前提違反時は `runtime_error` を送出する。
