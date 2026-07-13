---
title: Exact and Approximate Subset Sum (部分和の厳密解法と近似解法)
documentation_of: ../src/approximate/dp/subset_sum.hpp
---

非負整数列から、合計が容量以下で最大となる部分集合を求める。`approximate::dp` 名前空間にある。`SubsetSumResult::selected_indices` は選んだ元の添字の昇順、`sum` はその合計である。

# sparse_subset_sum

~~~cpp
template<class Integer, class Capacity>
SubsetSumResult sparse_subset_sum(
    const vector<Integer>& values,
    Capacity capacity
)
~~~

到達可能な合計だけを昇順リストとして保持し、各要素について元のリストと平行移動したリストをマージする。重複する合計では、その要素を使わない既存状態を優先する。最適解と選択添字を厳密に返す。

途中で保持する状態数の最大を $L$ とする。

- 時間計算量: $O(NL)$
- 追加空間計算量: $O(NL)$（復元用の永続的な履歴を含む）

最悪の場合は $L=\min(2^N,C+1)$ である。容量 $C$ に比例する密 DP と異なり、到達可能和が疎な入力に向く。

# subset_sum_trimming_fptas

~~~cpp
template<class Integer, class Capacity>
SubsetSumResult subset_sum_trimming_fptas(
    const vector<Integer>& values,
    Capacity capacity,
    long double epsilon
)
~~~

各要素を追加した後の到達可能和リストをマージし、直前に残した値を $z$ とすると `candidate <= z * (1 + delta)` である近接状態を除く。`delta = epsilon / (2N)` を使い、容量超過状態も除く。残した状態には復元履歴を持たせる。

$0<\varepsilon<1$ に対し、返す合計は最適値の $(1-\varepsilon)$ 以上である。途中で保持する状態数の最大を $L$ とする。

- 時間計算量: $O(NL)$
- 追加空間計算量: $O(NL)$（復元履歴を含む）

正の最小到達和を $a$、容量を $C$ とすると、典型的には $L=O((N/\varepsilon)\log(C/a))$ に抑えられる。0 の要素しかない場合も扱える。

# SubsetSumResult

`sum` の取得は時間・追加空間ともに $O(1)$。`selected_indices` の走査は $O(選択数)$。

## 注意点

`Integer` と `Capacity` は `bool` 以外の 64-bit 以下の整数型とし、値と容量は非負とする。負値には `std::invalid_argument` を送出する。`subset_sum_trimming_fptas` の `epsilon` は有限かつ $(0,1)$ でなければならず、違反時は `std::invalid_argument` を送出する。必要なメモリを確保できない場合は `std::bad_alloc` が送出され得る。
