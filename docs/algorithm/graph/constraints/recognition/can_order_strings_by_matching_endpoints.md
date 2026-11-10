---
title: String Endpoint Chain Feasibility (文字列端点連鎖判定) [WORDS1]
documentation_of: ../../../../../src/algorithm/graph/constraints/recognition/can_order_strings_by_matching_endpoints.hpp
---

各文字列をちょうど 1 回ずつ使い、直前の文字列の末尾文字と次の文字列の先頭文字が一致するように並べられるかを判定する。

## `can_order_strings_by_matching_endpoints`

```cpp
bool can_order_strings_by_matching_endpoints(
    const std::vector<std::string>& strings
);
```

各文字列を先頭文字から末尾文字への有向辺とみなし、すべての辺を 1 回ずつ通るオイラー路またはオイラー閉路が存在するかを判定する。

## 計算量

文字列の個数を $N$、バイト値の種類数を $\Sigma = 256$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `can_order_strings_by_matching_endpoints` | $O(N + \Sigma)$ | $O(N + \Sigma)$ |

## 注意点

- 各文字列は空であってはならない。空文字列を含む場合は `runtime_error` を送出する。
- 先頭・末尾の文字は `unsigned char` に変換したバイト値として扱うため、英小文字以外も使える。
- 文字列の列が空なら `false` を返す。
- 同じ文字列が複数回現れる場合も、それぞれを別の辺として扱う。
