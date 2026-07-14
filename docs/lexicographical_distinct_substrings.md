---
title: Lexicographical Distinct Substrings (辞書順相異部分文字列) [SUBLEX]
documentation_of: ../src/structure/string/lexicographical_distinct_substrings.hpp
---

# LexicographicalDistinctSubstrings

```cpp
class LexicographicalDistinctSubstrings {
public:
    explicit LexicographicalDistinctSubstrings(string text);
    long long count() const;
    optional<string> kth(long long one_based_rank) const;
};
```

## Constructor

`text` の相異なる非空部分文字列を問い合わせられる状態を構築する。

## count

相異なる非空部分文字列の個数を返す。

## kth

辞書順で `one_based_rank` 番目の部分文字列を返す。順位が範囲外なら `nullopt` を返す。

## API別の時間計算量・空間計算量

$N=text.size()$、返す文字列長を $L$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N\log N)$ | $O(N)$ |
| `count()` | $O(1)$ | $O(1)$ |
| `kth(rank)` | $O(\log(N+1)+L)$ | $O(L)$ |

## 注意点

- 順位は1-indexedである。
- 部分文字列数が `long long` を越える場合は `overflow_error` を送出する。
