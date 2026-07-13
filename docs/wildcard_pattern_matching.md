---
title: Wildcard Pattern Matching
documentation_of: ../src/algorithm/string/wildcard_pattern_matching.hpp
---

文字列`text`の各位置に対して、`pattern`がwildcardを考慮して一致するかを求める。
既定のwildcardは`'*'`であり、`text`側と`pattern`側のどちらに現れても任意の1文字と一致する。

各byteを正の整数$a,b$、wildcardを0に写すと、1組の文字の不一致量は
$ab(a-b)^2$で表せる。配置ごとの不一致量の総和を3回の畳み込みに分解し、
2つのNTT素数で同時に0となるかを調べる。

## `wildcard_pattern_matching`

```cpp
std::vector<bool> result = wildcard_pattern_matching(text, pattern);
std::vector<bool> result = wildcard_pattern_matching(text, pattern, wildcard);
```

`result[i]`は`text.substr(i, pattern.size())`と`pattern`が一致するときtrueになる。
`pattern`が`text`より長ければ空vector、空なら`text.size() + 1`個のtrueを返す。
`pattern`または`text`がすべてwildcardなら畳み込みを省略する。

必要なNTT長が$2^{23}$を超える場合、または下記の完全性の上限を超える場合は
`std::length_error`を送出する。Library Checkerの$|text|\leq524288$はこの範囲内である。

## 完全性

使用する法は998244353と469762049である。
受理する入力では非負整数である不一致量の総和が2法の積より小さいことを検査するため、
両方の剰余が0なら整数としても0である。乱択や衝突確率は用いない。

## 計算量

$N=|text|$、$M=|pattern|$とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `wildcard_pattern_matching` | $O((N+M)\log(N+M))$ | $O(N+M)$ |

全wildcardの早期終了時は$O(N+M)$時間である。
