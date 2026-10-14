// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/convolution_mod_large
#include <cstdio>
#include <vector>
#include "../../src/algorithm/fft/transform/large_convolution_ntt.hpp"
namespace{
char input_buffer[1<<16],output_buffer[1<<16];
std::size_t input_position=0,input_size=0,output_size=0;
int get(){
    if(input_position==input_size){input_size=std::fread(input_buffer,1,sizeof(input_buffer),stdin);input_position=0;}
    return input_position<input_size?input_buffer[input_position++]:EOF;
}
unsigned read(){int c=get();while(c<=32 && c!=EOF)c=get();unsigned x=0;while(c>32){x=x*10+c-'0';c=get();}return x;}
void put(char c){if(output_size==sizeof(output_buffer)){std::fwrite(output_buffer,1,output_size,stdout);output_size=0;}output_buffer[output_size++]=c;}
void write(unsigned x){char digits[16];int n=0;do{digits[n++]=char('0'+x%10);x/=10;}while(x);while(n)put(digits[--n]);}
}
int main(){
    const unsigned n=read(),m=read();
    std::vector<Modint998244353> a(n),b(m);
    for(auto& x:a)x=Modint998244353::raw(read());
    for(auto& x:b)x=Modint998244353::raw(read());
    const auto c=large_convolution_ntt<998244353>(a,b);
    for(std::size_t i=0;i<c.size();++i){write(c[i].val());put(i+1==c.size()?'\n':' ');}
    std::fwrite(output_buffer,1,output_size,stdout);
}
