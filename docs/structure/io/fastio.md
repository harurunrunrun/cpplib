---
title: Fast IO (高速入出力) [aplusb] [many_aplusb] [many_aplusb_128bit]
documentation_of: ../../../src/structure/io/fastio.hpp
---

`stdin` と `stdout` をそれぞれ固定長bufferで扱う整数・文字列入出力。
destructorで未出力bufferをflushするため、通常はmain関数の先頭で構築する。

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
| `fastio()` | 空の入出力bufferを構築する | $O(1)$ |
| `~fastio()` | 未出力bufferを`stdout`へ書き出す | buffer内の文字数に対して$O(B)$ |
| `readint(int& x)` | 次の空白区切り10進整数を`x`へ格納する | 読み込む文字数に対して$O(D)$ |
| `readll(long long& x)` | 次の空白区切り10進整数を`x`へ格納する | $O(D)$ |
| `readi128(__int128& x)` | 次の符号付き128 bit整数を読み込む | $O(D)$ |
| `readstr(char* s)` | 次の空白区切り文字列と終端`'\0'`を`s`へ格納する | $O(L)$ |
| `write(char c)` | 1文字を出力bufferへ追加する | amortized $O(1)$ |
| `write(const char* s)` | 終端`'\0'`まで追加する | $O(L)$ |
| `writeint(int x)` | `x`を10進表記で追加する | $O(D)$ |
| `writell(long long x)` | `x`を10進表記で追加する | $O(D)$ |
| `writei128(__int128 x)` | 符号付き128 bit整数を10進表記で追加する | $O(D)$ |

## 空間計算量（API別の追加領域）

constructor、destructor、`readint`、`readll`、`readstr`、2種類の`write`、`writeint`、`writell`はいずれも $O(1)$ の追加空間を使う。`readstr`の出力文字列は呼び出し側の領域であり、入出力bufferはobject本体の固定保存領域 $O(\mathtt{read\_size}+\mathtt{write\_size})$ である。

## 注意点

- `readstr`の出力先には文字列長+1以上の領域を用意する。
- 数値入力は対象型で表現できる10進整数とする。入力失敗を通知する戻り値や例外はない。
- 整数の読み書きは各型の最小値・最大値を含む。128 bit整数にはGCC 13の `__int128` を使う。入出力後の算術演算では、呼び出し側で結果が対象型の範囲に収まることを保証する。
- 読み書きbufferはそれぞれ`read_size`, `write_size` byte。bufferの補充・flushを含めても全体は入出力文字数に対して線形時間。
