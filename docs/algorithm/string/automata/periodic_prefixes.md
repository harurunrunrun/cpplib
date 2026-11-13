---
title: Periodic Prefixes (周期的接頭辞) [PERIOD]
documentation_of: ../../../../src/algorithm/string/automata/periodic_prefixes.hpp
---

列の各接頭辞のうち、同じブロックを 2 回以上連結して表せるものを列挙する。
接頭辞関数から各接頭辞の最小周期を求めて判定する。

## API

```cpp
template<class Sequence>
vector<pair<int, int>> periodic_prefixes(const Sequence& sequence)
```

## 引数・戻り値

- `sequence`: 周期的な接頭辞を列挙する入力列。
- 戻り値: `(prefix_length, repetition_count)` の列。
  `prefix_length` は接頭辞の長さ、`repetition_count` はその接頭辞を同じ空でないブロックの連結として表すときの最大の反復回数である。
  結果は `prefix_length` の昇順で、反復回数 1 のものは含まない。

## 時間計算量・空間計算量

$N$ を入力列の長さ、$K$ を返す組の個数とし、要素の等値比較 1 回を $O(1)$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `periodic_prefixes(sequence)` | $O(N)$ | 追加領域 $O(N)$、出力 $O(K)$ |

## 注意点

- `Sequence` は `size()`、0-indexed のランダムアクセス、要素の等値比較に対応する必要がある。
- 長さ 0 または 1 の列では空の結果を返す。
- 組の第 1 要素は 0-indexed の位置ではなく接頭辞の長さである。
