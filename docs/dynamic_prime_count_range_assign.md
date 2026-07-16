---
title: Dynamic Prime Count Range Assign (区間代入・素数個数) [CNTPRIME]
documentation_of: ../src/structure/segtree/dynamic_prime_count_range_assign.hpp
---

整数列への区間一括代入と区間内の素数個数問い合わせを処理する。

## DynamicPrimeCountRangeAssign

```cpp
template<int MAX_SIZE, int MAX_VALUE>
class DynamicPrimeCountRangeAssign {
public:
    explicit DynamicPrimeCountRangeAssign(const vector<int>& values);
    int size() const;
    void assign(int left, int right, int value);
    int count_primes(int left, int right) const;
};
```

## Constructor

初期列を構築し、`MAX_VALUE` 以下の素数判定表を用意する。

## size

列長を返す。

## assign

半開区間 `[left,right)` の全要素を `value` に置き換える。

## count_primes

半開区間 `[left,right)` にある素数の個数を返す。

## API別の時間計算量・空間計算量

列長を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(MAX\_VALUE\log\log MAX\_VALUE+N\log N)$ | $O(MAX\_VALUE+N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `assign(l,r,x)` | $O(\log N)$ | $O(1)$ |
| `count_primes(l,r)` | $O(\log N)$ | $O(1)$ |

## 注意点

- 全値は素数判定表が扱える範囲でなければならない。
- 列長は `MAX_SIZE` 以下、区間は有効な半開区間でなければならない。
- 前提違反時は下位APIから `runtime_error` が送出される。
