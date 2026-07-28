---
title: Dynamic Rolling Hash (動的ローリングハッシュ)
documentation_of: ../../../../src/algorithm/string/automata/dynamic_rolling_hash.hpp
---

1文字更新を行う byte 文字列について、部分文字列ハッシュと最長共通接頭辞を求める。
法 $2^{61}-1$ の多項式ハッシュをセグメント木で管理する。

## API

```cpp
DynamicRollingHash(const std::string& text, uint64_t base = default_base)
int size() const
void set(int index, unsigned char symbol)
Node get(int left, int right) const
bool equal(int first, int second, int length) const
int lcp(int first, int second) const
```

- `get(left, right)` は半開区間の `hash` と `length` を返す。
- `equal` は同じ長さの2部分文字列のハッシュを比較する。
- `lcp` は2接尾辞の最長共通接頭辞長を返す。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `set` | $O(\log N)$ | $O(1)$ |
| `get`, `equal` | $O(\log N)$ | $O(1)$ |
| `lcp` | $O(\log^2 N)$ | $O(1)$ |

## 注意点

ハッシュ衝突の可能性はある。添字または区間が範囲外なら例外を送出する。
