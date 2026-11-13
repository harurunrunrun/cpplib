---
title: Rolling Hash (ローリングハッシュ)
documentation_of: ../../../../src/algorithm/string/automata/rolling_hash.hpp
---

法 $2^{61}-1$ の多項式ハッシュ（ローリングハッシュ）である。前処理後、任意の部分文字列のハッシュ値を $O(1)$ で求める。

## コンストラクタ・構築

```cpp
RollingHash()
explicit RollingHash(const string& s, uint64_t base = default_base)
void build(const string& s, uint64_t base = default_base)
```

- デフォルトコンストラクタは空文字列を表し、`size() == 0`、`all() == 0` である。
- `build(s, base)` は同じオブジェクトを文字列 `s` で再構築する。
- `base` は `1 <= base < mod` を満たす必要があり、違反時は `std::runtime_error` を送出する。`default_base` は `1000003` である。

## ハッシュ

```cpp
uint64_t get(int l, int r) const
uint64_t all() const
int size() const
uint64_t base_value() const
```

- `get(l, r)` は半開区間 `[l, r)` のハッシュ値を返す。`0 <= l <= r <= size()` が必要である。
- `all()` は `get(0, size())` と同じである。
- `size()` は構築した文字列の長さ、`base_value()` は使用中の基数を返す。

## LCP

```cpp
int lcp(const RollingHash& other, int l1, int l2) const
```

自身の位置 `l1` から始まる接尾辞と、`other` の位置 `l2` から始まる接尾辞の最長共通接頭辞の長さを、二分探索で返す。
両オブジェクトの基数が一致し、開始位置がそれぞれ `[0, size()]` に入る必要がある。

## 時間計算量

$N$ を構築した文字列の長さ、$L$ を比較する 2 つの接尾辞の長さの小さい方とする。

| API | 時間計算量 |
| --- | --- |
| `RollingHash()` | $O(1)$ |
| `RollingHash(s, base)` / `build(s, base)` | $O(N)$ |
| `size()` / `base_value()` | $O(1)$ |
| `get(l, r)` / `all()` | $O(1)$ |
| `lcp(other, l1, l2)` | $O(\log(L+1))$ |
| 静的メンバ `mod` / `default_base` の参照 | $O(1)$ |

構築後の保持領域は $O(N)$ である。

## 注意点

- 範囲外の区間・開始位置や、基数が異なるオブジェクトとの `lcp` では `std::runtime_error` を送出する。
- `u64`、`u128` は公開の型別名、`mod`、`default_base` は公開の定数である。
- ハッシュ値の一致は文字列の一致を確率的に示すだけであり、衝突の可能性がある。基数のデフォルトは固定値なので、意図的に衝突させる入力が問題になる場合は乱数の基数を渡す。
