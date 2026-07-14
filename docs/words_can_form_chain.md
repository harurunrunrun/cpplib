---
title: Words Can Form Chain (単語連鎖の可否) [WORDS1]
documentation_of: ../src/algorithm/graph/words_can_form_chain.hpp
---

各単語をちょうど1回使い、末尾文字と次の先頭文字が一致する列を作れるか判定する。

# words_can_form_chain

```cpp
bool words_can_form_chain(const vector<string>& words);
```

すべての単語を条件どおりに並べられる場合に `true`、そうでなければ `false` を返す。

## API別の時間計算量・空間計算量

単語数を $N$、全文字数を $L$、英小文字の種類数を $\Sigma=26$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `words_can_form_chain` | $O(L+\Sigma^2)$ | $O(\Sigma^2)$ |

## 注意点

- 各単語は空でなく、`a` から `z` のみで構成されなければならない。
- 不正な単語では `runtime_error` を送出する。
- 空の単語列に対しては `false` を返す。
