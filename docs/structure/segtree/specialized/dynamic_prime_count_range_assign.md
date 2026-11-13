---
title: Dynamic Prime Count Range Assign (区間代入・素数個数) [CNTPRIME]
documentation_of: ../../../../src/structure/segtree/specialized/dynamic_prime_count_range_assign.hpp
---

整数列への区間一括代入と、区間内の素数の個数の問い合わせを処理する。

## DynamicPrimeCountRangeAssign

```cpp
template<int MAX_SIZE = -1, int MAX_VALUE = -1>
class DynamicPrimeCountRangeAssign {
public:
    explicit DynamicPrimeCountRangeAssign(const vector<int>& values);
    int size() const;
    void assign(int left, int right, int value);
    int count_primes(int left, int right) const;
};
```

各要素を「素数なら 1、そうでなければ 0」に置き換えた列を、区間代入・区間和の遅延セグメント木（`DynamicLazySegtree`）で管理する。

## コンストラクタ

初期列を構築し、`MAX_VALUE` 以下の素数判定表（篩）を用意する。

## size

列の長さを返す。

## assign

半開区間 $[left, right)$ の全要素を `value` に置き換える。

## count_primes

半開区間 $[left, right)$ にある素数の個数を返す。

## API 別の時間計算量・空間計算量

列の長さを $N$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(\texttt{MAX_VALUE}\log\log \texttt{MAX_VALUE}+N\log N)$ | $O(\texttt{MAX_VALUE}+N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `assign(l, r, x)` | $O(\log N)$ | $O(1)$ |
| `count_primes(l, r)` | $O(\log N)$ | $O(1)$ |

## 容量を省略する場合

テンプレート引数を両方省略した `DynamicPrimeCountRangeAssign<>` は、列の長さと値域の上限が不要になる。代入値の素数判定には Miller–Rabin 法を使い、値域全体の篩用メモリは確保しない。1 回の代入では木の更新に加えて $O(\log V)$ 回の 64 ビット剰余演算を行う（$V$ は代入値）。2 未満の値は素数でないとみなす。

## 注意点

- 固定容量版では、すべての値が素数判定表の扱える範囲になければならない。
- 列の長さは `MAX_SIZE` 以下、区間は有効な半開区間でなければならない。
- 前提に違反した場合は下位の API から `std::runtime_error` が送出される。
