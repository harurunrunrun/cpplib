---
title: Grid Hamiltonian Path with Direction Counts (方向別移動回数を指定したグリッドのハミルトン路)
documentation_of: ../../../../../src/algorithm/graph/specialized/construction/grid_hamiltonian_path_with_direction_counts.hpp
---

障害物のない $N$ 行 $M$ 列のグリッドで、左上 $(1,1)$ から右下 $(N,M)$ まで、
全マスをちょうど1回ずつ訪問する経路を判定・構築する。
左・右・上・下への移動回数をそれぞれ $L,R,U,D$ に指定する。
移動回数の合計は **$NM-1$**。

## API

~~~cpp
bool can_construct_grid_hamiltonian_path_with_direction_counts(
    long long n, long long m,
    long long l, long long r, long long u, long long d
);

std::optional<std::string> grid_hamiltonian_path_with_direction_counts(
    long long n, long long m,
    long long l, long long r, long long u, long long d
);
~~~

- 判定関数は、条件を満たす経路が存在するときだけ `true` を返す。
- 構築関数は、可能なら長さ $NM-1$ の移動列を返し、不可能なら `std::nullopt` を返す。
- `L` / `R` は列番号を $-1$ / $+1$、`U` / `D` は行番号を $-1$ / $+1$ する。
- `n == m == 1` かつ全移動回数が0なら、構築結果は値を持つ空文字列。

## 使用例

~~~cpp
#include "src/algorithm/graph/specialized/construction/grid_hamiltonian_path_with_direction_counts.hpp"

auto path = grid_hamiltonian_path_with_direction_counts(3, 3, 2, 4, 0, 2);
// *path == "RRDLLDRR"

bool possible =
    can_construct_grid_hamiltonian_path_with_direction_counts(3, 3, 1, 3, 1, 3);
// false: 合計と終点の条件を満たしても、左移動1回では構築できない。
~~~

## 必要十分条件

まず次の条件をすべて満たす必要がある。

- $N,M \ge 1$、$L,R,U,D \ge 0$。
- $R-L=M-1$、$D-U=N-1$。
- $L+R+U+D=NM-1$。
- $N,M$ がともに偶数ではない。

以降、$M$ が偶数ならグリッドを転置し、
$(N,M,L,R,U,D)$ を $(M,N,U,D,L,R)$ に読み替える。
これで $M$ は奇数になる。追加条件は次の2つだけ。

$$
L \equiv 0 \pmod 2,
\qquad
L \le \left\lfloor\frac{N-1}{2}\right\rfloor(M-1).
$$

### 条件が必要な理由

終点と移動回数の式は変位と訪問マス数から従う。
両辺が偶数の場合は、グリッドの市松模様の色と経路長の偶奇が一致しない。

隣り合う列の境界を考える。経路は左側から右側へ終わるため、その境界を右に渡る回数は
左に渡る回数より1多い。境界上の辺は $N$ 本しかなく、各辺は高々1回使うので、
左に渡れるのは各境界で高々 $\lfloor(N-1)/2\rfloor$ 回となる。

$L$ の偶奇は次のように示せる。証明内だけ座標を0始まりとし、経路の終点から
$(N,M-1),(N,-1),(0,-1),(0,0)$ の順にグリッド外を通って閉じる。
これは内部に格子点を持たない単純格子多角形で、境界長は $(N+1)(M+1)$。
Pick の定理から面積は $A=(N+1)(M+1)/2-1$ となる。

元の経路のうち奇数行の水平辺数を $H_1$ とする。
各頂点に行の偶奇による符号を付け、経路辺の両端の符号を合計すると、
$N$ が奇数なら $H_1=L$、偶数なら $H_1=L+(M-1)/2$ が得られる。
また、面積を水平辺に沿って計算すると $A \equiv H_1+NM \pmod 2$。
$M$ が奇数であることを代入すれば、どちらの場合も $L$ が偶数と分かる。

### 構築方法

$M$ を奇数にそろえた後、現在残っている高さと左移動回数を更新しながら進める。

1. $L \ge M-1$ なら、上の2行を右・下・左の順に走査し、さらに1マス下へ進む。
   高さを2、$L$ を $M-1$ 減らして繰り返す。ただし $M=1$ ならこの処理を省く。
2. $L=0$ なら、残りを列ごとに下・上・下・上と蛇行する。
3. $0<L<M-1$ なら、左端の $L+1$ 列の上2行を右・下・左で走査し、
   その下を列方向に蛇行する。左端部分の右下から、残った偶数本の列を上・下と蛇行する。

各部分は重ならず、最終位置は右下になる。
上限条件によって2行を取り除く前には高さが3以上残っており、
$L+1$ の奇数性によって部分間の接続位置が一致する。
左移動回数は指定どおりであり、全長と終点の式から他の3方向の回数も指定どおりになる。
転置していた場合は、文字を `L ↔ U`、`R ↔ D` と対応させて出力する。

## 時間計算量

- 判定: $O(1)$。
- 構築: 不可能なら $O(1)$、可能なら $O(NM)$。出力長に対して最適。

## 空間計算量

- 判定: $O(1)$。
- 構築: 戻り値に $O(NM)$、戻り値を除く補助領域は $O(1)$。

## 注意点

- 寸法が0以下、または移動回数が負の場合は `false` / `std::nullopt` を返す。
- 入力はすべて `long long`。積と移動回数の合計は GCC 13 の `__int128` で計算し、
  `long long` の範囲内の入力で中間計算がオーバーフローしないようにしている。
- 判定関数はメモリを確保せず、経路を保存できない大きな寸法も判定できる。
- 構築可能でも長さが `std::string::max_size()` を超える場合は `std::length_error`、
  メモリ確保に失敗した場合は `std::bad_alloc` が送出される。
- GCC 13 / C++17 以降に対応し、外部ライブラリへの依存はない。

## 検証

面積25以下の全長方形について、DFSで列挙したハミルトン路の方向別移動回数と判定結果を比較する。
構築結果は、範囲内の移動、全マスの一度ずつの訪問、終点、4方向の回数を独立に検査する。
不正入力、転置、細長いグリッド、約100万マス、大きな整数の判定も検証する。
