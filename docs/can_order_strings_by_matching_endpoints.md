---
title: String Endpoint Chain Feasibility (文字列端点連鎖判定) [WORDS1]
documentation_of: ../src/algorithm/graph/can_order_strings_by_matching_endpoints.hpp
---

各文字列をちょうど1回ずつ使用し、直前の文字列の末尾文字と次の文字列の先頭文字が一致する順序を作れるかを判定する。

# can_order_strings_by_matching_endpoints

```cpp
bool can_order_strings_by_matching_endpoints(
    const vector<string>& strings
);
```

各文字列を先頭文字から末尾文字への有向辺とみなし、すべての辺を1回ずつ通るオイラー路またはオイラー閉路の存在を判定する。

## API別の時間計算量・空間計算量

文字列数を $N$、byte値の種類数を $\Sigma=256$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `can_order_strings_by_matching_endpoints` | $O(N+\Sigma)$ | $O(N+\Sigma)$ |

## 注意点

- 各文字列は空でなければならない。空文字列を含む場合は `runtime_error` を送出する。
- 先頭・末尾は `unsigned char` に変換したbyte値として扱うため、英小文字以外も使用できる。
- 空の文字列列には `false` を返す。
- 同じ文字列が複数回現れる場合も、それぞれを別の辺として扱う。
