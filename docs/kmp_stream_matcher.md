---
title: KMP Stream Matcher (KMPストリーム照合器) [NHAY]
documentation_of: ../src/algorithm/string/kmp_stream_matcher.hpp
---

入力列を保存せず、1要素ずつ受け取って固定patternの全出現位置を列挙する。

## API

```cpp
KmpStreamMatcher(Pattern pattern)
long long feed(const Value& value)
void reset()
int pattern_size() const
int matched_length() const
long long processed_size() const
```

`feed(value)` は `value` を末尾へ1要素追加し、その要素で終わるpatternの出現が
あれば0-indexed開始位置を、なければ `-1` を返す。重なる出現も返す。
`reset()` はpatternと前処理を保持したまま照合状態と処理済み長を0へ戻す。
残りのaccessorはそれぞれpattern長、現在一致中の接頭辞長、処理済み要素数を返す。

## API別の時間計算量・空間計算量

pattern長を $M$、投入した要素数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| constructor | $O(M)$ | $O(M)$ |
| `feed(value)` | 1回の最悪 $O(M)$、全投入で合計 $O(N+M)$ | $O(1)$ |
| `reset()` | $O(1)$ | $O(1)$ |
| 各accessor | $O(1)$ | $O(1)$ |

## 注意点

patternは非空でなければならず、空なら例外を送出する。patternは値として保持するが、
`string_view` など参照型の要素を持つ型を渡す場合は参照先の寿命を利用中維持する。
列長と返却位置は `long long` に収まること、pattern長は `int` に収まることを前提とする。
