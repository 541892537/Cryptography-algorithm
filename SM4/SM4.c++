#include "SM4.h"
using namespace std;


void HexToBit(bitset<4> &x, char c){
    int val;
    if('0'<=c&&c<='9') val=c-'0';
    else val=c-'a'+10;
    rpe(i,3,0) x[3-i]=val&(1<<i);
}

void BitToHex(bitset<4> x){
    int val=0;
    rep(i,0,3) val=val*2+x[i];
    if(val<=9) putchar(val+'0');
    else putchar(val-10+'a');
}

void DebugWord(Word x){
    bitset<4>y;
    rep(i,0,7){
        rep(j,0,3) y[j] = x[i*4+j];
        BitToHex(y);
    }
}


void Reverse(){
    unsigned long long x;
    rep(i,0,3){
        rep(j,0,15) x=FK[i][j],FK[i][j]=FK[i][31-j],FK[i][31-j]=x;
    }
    rep(i,0,31){
        rep(j,0,15) x=CK[i][j],CK[i][j]=CK[i][31-j],CK[i][31-j]=x;
    }
}

void Init(Word *x){
    char s[32];cin>>s;
    bitset<4>tmp;
    int num=0;
    rep(i,0,31){
        HexToBit(tmp,s[i]);
        rep(j,0,3) x[num][(i-num*8)*4+j]=tmp[j];
        if(i%8==7) num++;
    }
}

Word tao(Word x){
    Word y;int a,b;
    rep(i,0,3){
        a=b=0;
        rep(j,0,3) a=a*2+x[i*8+j];
        rep(j,4,7) b=b*2+x[i*8+j];
        a=SboxTable[a][b];
        b=a&(15);a>>=4;
        rep(j,0,3) y[i*8+j]=a&(1<<(3-j));
        rep(j,4,7) y[i*8+j]=b&(1<<(7-j));
    }
    return y;
}

Word LeftShift(Word x,int y){
    bitset<32>z;
    rep(i,0,y-1) z[i] = x[i];
    x>>=y;
    rep(i,0,y-1) x[32-y+i] = z[i];
    return x;  
}

Word L_Key(Word x){
    return x^LeftShift(x,13)^LeftShift(x,23);
}

Word TT(Word x){
    return L_Key(tao(x));
}

void KeyExpansion(){
    rep(i,0,3) K[i]=Key[i]^FK[i];
    rep(i,0,31){
        rk[i]=K[i+4]=K[i] ^ TT(K[i+1]^K[i+2]^K[i+3]^CK[i]);
    }
}

Word L_F(Word x){
    return x^LeftShift(x,2)^LeftShift(x,10)^LeftShift(x,18)^LeftShift(x,24);
}

Word T(Word x){return L_F(tao(x));}

void solve(){
    if(sign==1){//加密
        rep(i,0,3) X[i]=In[i];
        rep(i,0,31){
            X[i+4]=X[i] ^ T(X[i+1]^X[i+2]^X[i+3]^rk[i]);
        }
        rep(i,0,3) Out[i]=X[32+3-i];
        //rep(i,0,3) DebugWord(Out[i]);
    }
    else{
        rep(i,0,3) X[i+32]=In[3-i];
        rpe(i,31,0){
            X[i]=X[i+4]^T(X[i+3]^X[i+2]^X[i+1]^rk[i]);
        }
        rep(i,0,3) Out[i]=X[i];
        //rep(i,0,3) DebugWord(Out[i]);
    }
}

int main(){
    //freopen("123.txt","r",stdin);
    Reverse();//反转成功
    printf("--------------------------\n");
    printf("----欢迎使用SM4加密系统----\n");
    printf("--------------------------\n");
    while(1){
        printf("请选择你的操作 1 加密 2 解密 3 退出\n");
        cin>>sign;
        if(sign==3) {printf("退出成功");break;}
        printf("请输入16进制下的128位的密钥:\n");

        Init(Key);
      //  rep(i,0,3) DebugWord(Key[i]);// 密钥输入成功
        KeyExpansion();
        //rep(i,0,31) DebugWord(rk[i]);密钥扩展成功

        if(sign==1) printf("请输入16进制下的128位的明文:\n");
        else printf("请输入16进制下的128位的密文:\n");

        Init(In);
        //rep(i,0,3) DebugWord(In[i]); //明文或密文输入成功

        solve();

        rep(i,0,3) DebugWord(Out[i]);
        pts;
    }
    return 0;
}