---
title: Fast IO (高速入出力) [aplusb] [many_aplusb] [many_aplusb_128bit]
documentation_of: ../../../src/structure/io/fastio.hpp
---

`stdin` と `stdout` をそれぞれ固定長のバッファで扱う、整数・文字列の入出力。
デストラクタで未出力のバッファをフラッシュするため、通常は `main` 関数の先頭で構築する。

```cpp
fastio io;
int n;
long long x;
char word[100];
io.readint(n);
io.readll(x);
io.readstr(word);
io.writeint(n);
io.write(' ');
io.writell(x);
io.write('\n');
```

## API

| 操作 | 説明 | 時間計算量 |
| --- | --- | --- |
| `fastio()` | 空の入出力バッファを構築する | $O(1)$ |
| `~fastio()` | 未出力のバッファを `stdout` に書き出す | バッファ内の文字数 $B$ に対して $O(B)$ |
| `readint(int& x)` | 次の空白区切りの 10 進整数を `x` に格納する | 読み込む文字数 $D$ に対して $O(D)$ |
| `readll(long long& x)` | 次の空白区切りの 10 進整数を `x` に格納する | $O(D)$ |
| `readi128(__int128& x)` | 次の符号付き 128 bit 整数を読み込む | $O(D)$ |
| `readstr(char* s)` | 次の空白区切りの文字列と終端の `'\0'` を `s` に格納する | 文字列長 $L$ に対して $O(L)$ |
| `write(char c)` | 1 文字を出力バッファに追加する | 償却 $O(1)$ |
| `write(const char* s)` | 終端の `'\0'` の手前までを追加する | $O(L)$ |
| `writeint(int x)` | `x` を 10 進表記で追加する | $O(D)$ |
| `writell(long long x)` | `x` を 10 進表記で追加する | $O(D)$ |
| `writei128(__int128 x)` | 符号付き 128 bit 整数を 10 進表記で追加する | $O(D)$ |

バッファの補充・フラッシュを含めても、全体の時間は入出力の文字数に対して線形である。

## 空間計算量

- 入出力バッファはオブジェクト本体の固定領域で、$O(\texttt{read_size}+\texttt{write_size})$（それぞれ $10^6$ バイト）である
- 各関数の追加領域は $O(1)$。`readstr` の出力文字列は呼び出し側の領域である

## 注意点

- `readstr` の出力先には、文字列長 $+1$ 以上の領域を用意する。
- 数値の入力は、対象の型で表現できる 10 進整数とする。入力の失敗を通知する戻り値や例外は無い。
- 整数の読み書きは、各型の最小値・最大値も扱える。128 bit 整数には GCC の `__int128` を使う。入出力後の算術演算では、結果が対象の型の範囲に収まることを呼び出し側で保証する。
- バッファの大きさはマクロ `read_size`、`write_size` で定義されている。同名の識別子と衝突しないように注意する。
- オブジェクトは 2 MB 程度の配列を持つので、ローカル変数として置く場合はスタックの大きさに注意する（グローバル変数や `static` にしてもよい）。
