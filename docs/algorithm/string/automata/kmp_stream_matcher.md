---
title: KMP Stream Matcher (KMPストリーム照合器) [NHAY]
documentation_of: ../../../../src/algorithm/string/automata/kmp_stream_matcher.hpp
---

入力列を保存せず、要素を 1 個ずつ受け取りながら、固定パターンの全出現位置を KMP 法で列挙する。

## API

```cpp
template<class Pattern>
explicit KmpStreamMatcher(Pattern pattern)
template<class Value>
long long feed(const Value& value)
void reset()
int pattern_size() const
int matched_length() const
long long processed_size() const
```

- `feed(value)`: `value` を末尾に 1 要素追加し、その要素で終わるパターンの出現があれば 0-indexed の開始位置を、なければ `-1` を返す。重なる出現も返す。
- `reset()`: パターンと前処理を保持したまま、照合状態と処理済みの長さを 0 に戻す。
- `pattern_size()`、`matched_length()`、`processed_size()`: それぞれパターン長、現在一致している接頭辞の長さ、処理済みの要素数を返す。

推論補助があるので、`KmpStreamMatcher matcher(std::string("abc"));` のように書ける。

## 時間計算量・空間計算量

パターン長を $M$、投入した要素数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(M)$ | $O(M)$ |
| `feed(value)` | 1 回あたり最悪 $O(M)$、全投入の合計で $O(N+M)$ | $O(1)$ |
| `reset()` | $O(1)$ | $O(1)$ |
| 各アクセサ | $O(1)$ | $O(1)$ |

## 注意点

- パターンは空であってはならず、空なら `std::runtime_error` を送出する。
- パターンは値として保持するが、`string_view` など参照型の要素を持つ型を渡す場合は、使用中ずっと参照先を生存させること。
- `Value` と `Pattern` の要素は `operator==` で比較できる必要がある。
- 列長と返す位置は `long long` に、パターン長は `int` に収まることを前提とする。
