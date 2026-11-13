---
title: Lexicographical Distinct Substrings (辞書順相異部分文字列) [SUBLEX]
documentation_of: ../../../src/structure/string/lexicographical_distinct_substrings.hpp
---

文字列の相異なる非空部分文字列の個数と、辞書順で $k$ 番目の部分文字列を求める。
接尾辞配列と LCP 配列から、各接尾辞が新たに寄与する部分文字列数の累積和を前計算する。

## API

```cpp
class LexicographicalDistinctSubstrings {
public:
    explicit LexicographicalDistinctSubstrings(string text);
    long long count() const;
    optional<string> kth(long long one_based_rank) const;
};
```

## コンストラクタ

`text` の相異なる非空部分文字列を問い合わせられる状態を構築する。

## `count`

相異なる非空部分文字列の個数を返す。

## `kth`

辞書順で `one_based_rank` 番目の部分文字列を返す。順位が範囲外（`one_based_rank <= 0` または `count()` 超過）なら `nullopt` を返す。

## API 別の時間計算量・空間計算量

$N$ を `text.size()`、返す文字列の長さを $L$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N)$ |
| `count()` | $O(1)$ | $O(1)$ |
| `kth(rank)` | $O(\log(N+1)+L)$ | $O(L)$ |

## 注意点

- 順位は 1-indexed である。
- 部分文字列の個数が `long long` の範囲を超える場合は `overflow_error` を送出する。
- `text` の長さが `int` の最大値を超える場合は `length_error` を送出する。
