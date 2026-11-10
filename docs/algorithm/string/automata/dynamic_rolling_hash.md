---
title: Dynamic Rolling Hash (動的ローリングハッシュ)
documentation_of: ../../../../src/algorithm/string/automata/dynamic_rolling_hash.hpp
---

1 文字の更新ができるバイト文字列について、部分文字列のハッシュと最長共通接頭辞（LCP）を求める。
法 $2^{61}-1$ の多項式ハッシュをセグメント木で管理する。

## API

```cpp
explicit DynamicRollingHash(const std::string& text, uint64_t base = default_base)
int size() const noexcept
void set(int index, unsigned char symbol)
Node get(int left, int right) const
bool equal(int first, int second, int length) const
int lcp(int first, int second) const
```

- コンストラクタ: `base` は $[1,2^{61}-1)$ に入る必要があり、違反時は `std::runtime_error` を送出する。`default_base` は `1'000'003` である。
- `set(index, symbol)`: 位置 `index` の文字を `symbol` に変更する。
- `get(left, right)`: 半開区間 `[left, right)` のハッシュ値 `hash` と長さ `length` を持つ `Node` を返す。
- `equal(first, second, length)`: 位置 `first` と `second` から始まる長さ `length` の部分文字列のハッシュ値を比較する。
- `lcp(first, second)`: 位置 `first` と `second` から始まる接尾辞の最長共通接頭辞の長さを、二分探索で返す。

## 時間計算量・空間計算量

$N$ を `text` の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `set` | $O(\log N)$ | $O(1)$ |
| `get`, `equal` | $O(\log N)$ | $O(1)$ |
| `lcp` | $O(\log^2 N)$ | $O(1)$ |

## 注意点

- ハッシュの衝突により、異なる部分文字列を等しいと判定する可能性がある。基数のデフォルトは固定値なので、意図的に衝突させる入力が問題になる場合は乱数の `base` を渡す。
- 添字または区間が範囲外なら `std::runtime_error` を送出する（`equal` では負の長さも同様）。
