---
title: Dynamic Prime Count Range Assign (区間代入・素数個数) [CNTPRIME]
documentation_of: ../../../../src/structure/segtree/specialized/dynamic_prime_count_range_assign.hpp
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

## 容量を省略する場合

容量引数の既定値は `-1`。`DynamicPrimeCountRangeAssign<> tree(values);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。

容量省略版は値域の上限も不要で、代入値の素数判定に Miller–Rabin を使用する。1回の代入は木の更新に加えて $O(\log V)$ 回の64bit剰余演算を行い、値域全体の篩用メモリを確保しない。
