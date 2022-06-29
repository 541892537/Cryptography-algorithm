#include "AES.h"

int flag;//系统状态
//得到迭代次数
void GetNr(){
    if(Nk==4) Nr=10;
    if(Nk==6) Nr=12;
    if(Nk==8) Nr=14;
}

void GetRcon(){
    rep(i,0,9){
        rpe(j,31,0) Rcon[i][31-j]=IntRcon[i]>>j&1;
    }
}

//将一个16进制字符转化为4位二进制的字符串
string HexToBin(char x){
    int y;
    if('0'<=x&&x<='9') y = x-'0';
    else{
        if('a'<=x&&x<='f') y = x-'a'+10;
        else if('A'<=x&&x<='F') y = x - 'A'+10;
    } 
    string tmp="";
    rpe(i,3,0) {
        if(y&(1<<i)) tmp+='1';
        else tmp+='0';
    }
    return tmp;
}

//将x变为16进制输出
void OutHex(int x){
    if(x>=0&&x<=9) putchar(x+'0');
    if(x>=10&&x<=15) putchar(x-10+'a');
}
//输入主密钥
void InitMainKey(){
    printf("请选择密钥输入格式 1:16进制 2:2进制\n");
    int Format;cin>>Format;
    char s[300];
    printf("请输入密钥(密钥长度须为128,192或256)\n");
    cin>>s;
    int len=strlen(s);
    if(Format==1){
        LenOfMainKey = len*4;Nk=len>>3;
        rep(i,0,len-1){
            string tmp = HexToBin(s[i]);
            rep(j,0,3) MainKey[i*4+j] = tmp[j]-'0';
        }
    }  
    else {
        LenOfMainKey = len;Nk=len>>5;
        rep(i,0,len-1) MainKey[i] = s[i] -'0';
    } 
}
//进行字节循环左移
bitset<32> Rotl(bitset<32> x){
    bitset<32>y;
    rep(i,0,23) y[i] = x[i+8];
    rep(i,24,31) y[i] = x[i-24];
    return y;
}
//进行S-盒变换
bitset<32> SubByte(bitset<32> x,bool flag){
    bitset<32>y;
    for(int i=0;i<32;i+=8){
        int row=x[i]*8 + x[i+1]*4 + x[i+2]*2 + x[i+3];
        int col=x[i+4]*8 + x[i+5]*4 + x[i+6]*2 + x[i+7];
        bitset<8> val;
        if(flag==0)  rep(j,0,7) val[j]=S_Box[row][col][7-j];
        if(flag==1)  rep(j,0,7) val[j]=Inv_S_Box[row][col][7-j];
        rep(j,0,7) y[i+j]=val[j];
    }
    return y;
}
//将128位bitset转为 16进制字符串输出
void DeBug128(bitset<128> x){
    int num=0,val=0;
    rep(i,0,127){
        num++;
        val=(val<<1)+x[i];
        if(num==4) OutHex(val),val=num=0;
    }
    pts;
}

//输出轮密钥进行调试输出
void DebugExpasionKey(){
    bitset<128>x;
    rep(i,0,Nr){
        rep(j,0,128) x[j]=W[i*128+j];
        DeBug128(x);
    }
}

//进行密钥拓展
void KeyExpansion(){
    if(Nk<=6){
        rep(i,0,Nk*32-1) W[i] = MainKey[i];
        bitset<32>tmp;
        rep(I,Nk,Nb*(Nr+1)-1){
            rep(i,0,31) tmp[i] = W[(I-1)*32 +i];
            if(I%Nk==0) tmp=SubByte(Rotl(tmp),0) ^ Rcon[I/Nk -1];
            rep(i,0,31) W[I*32 + i] = W[(I-Nk)*32+i]^tmp[i];
        }
    }
}

//输入明文或密文
void Init(){
    printf("请选择");
    if(flag==1) printf("明文");
    else printf("密文");
    printf("输入格式 1:16进制 2:2进制\n");
    int Format;cin>>Format;
    char s[300];
    printf("请输入");
    if(flag==1) printf("明文");
    else printf("密文");
    printf("(须为128位)\n");
    cin>>s;
    int len=strlen(s);
    if(Format==1){
        rep(i,0,len-1){
            string tmp = HexToBin(s[i]);
            rep(j,0,3) In[i*4+j] = tmp[j]-'0';
        }
    }  
    else {
        rep(i,0,len-1) In[i] = s[i] -'0';
    } 
    Out=In;
}
//轮密钥
void AddRoundKey(bitset<128> key){
    rep(i,0,127) Out[i]=Out[i]^key[i];
}


//////////////////////////////////////////////////////////

//S盒变换
void SubBytes(){
    bitset<32>x;
    rep(i,0,3){
        rep(j,0,31) x[j]=Out[i*32+j];
        x=SubByte(x,0);
        rep(j,0,31) Out[i*32+j]=x[j];
    }
} 
//行位移
void ShiftRows(){
    bitset<8>x,y,z;
//第二行循环左移一字节
    rep(i,0,7) x[i] = Out[i + 8];
    rep(i,0,7) {
        Out[i+8] = Out[i+8+32];
        Out[i+8+32] = Out[i + 8 + 64];
        Out[i+8+64] = Out[i + 8 + 96];
    }
    rep(i,0,7) Out[i+8+96]=x[i];
//第三行循环左移二字节
    rep(i,0,7) x[i] = Out[i + 16],y[i] = Out[i+16+32];
    rep(i,0,7) {
        Out[i+16] = Out[i+16+64];
        Out[i+16+32] = Out[i + 16 + 96];
    }
    rep(i,0,7) Out[i+16+64]=x[i],Out[i+16+96]=y[i];
//第三行循环左移三字节
    rep(i,0,7) x[i]=Out[i+24],y[i]=Out[i+24+32],z[i]=Out[i+24+64];
    rep(i,0,7) {
        Out[i+24] = Out[i+24+96];
    }
    rep(i,0,7) Out[i+24+32]=x[i],Out[i+24+64]=y[i],Out[i+24+96]=z[i];
}
//GF乘法
int GFMul(int a,int b){
    int p=0;
    int tmp;
    rep(i,0,7){
        if(b&1!=0) p^=a;
        tmp=(a&0x80);
        a<<=1;
        if(a>=(1<<8)) a-=(1<<8);
        if(tmp!=0) a^=0x1b;
        b>>=1;
    }
    return p;
}
//将int转为8位bitset
void IntToBitset8(int x, bitset<8> &tmp){
    rpe(i,7,0){
        tmp[7-i]=(x>>i)&1;
    }
}
//列混合
void MixColumns(){
    int a[4],num,now;
    bitset<8>tmp;
    rep(i,0,3){
        rep(j,0,3) a[j]=0;
        now=0,num=0;
        rep(j,0,31){
            a[now] = (a[now]<<1)+Out[j+i*32];
            num++;
            if(num==8) now++,num=0; 
        }
        IntToBitset8( GFMul(0x02,a[0]) ^ GFMul(0x03,a[1]) ^ a[2] ^a[3],tmp);
        rep(j,0,7) Out[i*32 + j] =tmp[j];
        IntToBitset8( GFMul(0x02,a[1]) ^ GFMul(0x03,a[2]) ^ a[0] ^a[3],tmp);
        rep(j,8,15) Out[i*32 + j] =tmp[j-8];
        IntToBitset8( GFMul(0x02,a[2]) ^ GFMul(0x03,a[3]) ^ a[0] ^a[1],tmp);
        rep(j,16,23) Out[i*32 + j] =tmp[j-16];
        IntToBitset8( GFMul(0x02,a[3]) ^ GFMul(0x03,a[0]) ^ a[2] ^a[1],tmp);
        rep(j,24,31) Out[i*32 + j] =tmp[j-24];
    }
}
//加密算法
void Encrypt(){
    bitset<128>key;
    rep(i,0,127) key[i]=W[i];
    AddRoundKey(key);
    rep(round,1,Nr-1){
        SubBytes();
        ShiftRows();
        MixColumns();
        rep(i,0,127)key[i]=W[round*128+i];
        AddRoundKey(key);
    }
    SubBytes();
    ShiftRows();
    rep(i,0,127)key[i]=W[Nr*128+i];
    AddRoundKey(key);
    printf("加密后的密文为:\n");
    DeBug128(Out);
}

/////////////////////////////////////////////////////////////////

void InvSubBytes(){//加密算法中的 逆 S变换
    bitset<32>x;
    rep(i,0,3){
        rep(j,0,31) x[j]=Out[i*32+j];
        x=SubByte(x,1);
        rep(j,0,31) Out[i*32+j]=x[j];
    }
}

void InvShiftRows(){// 逆 行位移
    bitset<8>x,y,z;
//第二行右移一字节
    rep(i,0,7) x[i] = Out[i+96+8];
    rep(i,0,7){
        Out[i + 96 + 8] = Out[i + 64 + 8];
        Out[i+64+8] =Out[i+32+8];
        Out[i+32+8] = Out[i+8]; 
    }
    rep(i,0,7 ) Out[i+8] = x[i];
// 第二行右移两字节
    rep(i,0,7) x[i] = Out[i + 64 + 16],y[i] = Out[i+96+16];
    rep(i,0,7){
        Out[i+96+16] = Out[i+32+16];
        Out[i+64+16] = Out[i+16];
    }
    rep(i,0,7) Out[i+16] = x[i],Out[i+16+32] = y[i];
//第三行向右移三个字节
    rep(i,0,7) x[i] = Out[i+32+24],y[i] = Out[i+64+24],z[i] = Out[i+96+24];
    rep(i,0,7) Out[i+96+24] = Out[i+24];
    rep(i,0,7) Out[i+24] = x[i],Out[i+24+32] = y[i],Out[i+24+64] = z[i];
}

void InvMixColumns(bitset<128> &x){//逆 列混合
    int a[4],num,now;
    bitset<8>tmp;
    rep(i,0,3){
        rep(j,0,3) a[j]=0;
        now=0,num=0;
        rep(j,0,31){
            a[now] = (a[now]<<1)+x[j+i*32];
            num++;
            if(num==8) now++,num=0; 
        }
        IntToBitset8( GFMul(0x0e,a[0]) ^ GFMul(0x0b,a[1]) ^ GFMul(0x0d,a[2]) ^GFMul(0x09,a[3]),tmp);
        rep(j,0,7) x[i*32 + j] =tmp[j];
        IntToBitset8( GFMul(0x09,a[0]) ^ GFMul(0x0e,a[1]) ^ GFMul(0x0b,a[2]) ^GFMul(0x0d,a[3]),tmp);
        rep(j,8,15) x[i*32 + j] =tmp[j-8];
        IntToBitset8( GFMul(0x0d,a[0]) ^ GFMul(0x09,a[1]) ^ GFMul(0x0e,a[2]) ^GFMul(0x0b,a[3]),tmp);
        rep(j,16,23) x[i*32 + j] =tmp[j-16];
        IntToBitset8( GFMul(0x0b,a[0]) ^ GFMul(0x0d,a[1]) ^ GFMul(0x09,a[2]) ^GFMul(0x0e,a[3]),tmp);
        rep(j,24,31) x[i*32 + j] =tmp[j-24];
    }
}

void InvKeyExpansion(){
    bitset<128>tmp;
    rep(i,1,Nr-1){
        rep(j,0,127) tmp[j] = W[i*128+j];
        InvMixColumns(tmp);
        rep(j,0,127) W[i*128+j] = tmp[j];
    }

}

//解密算法
void Decrypt(){
    InvKeyExpansion();
   // DebugExpasionKey();成功
    bitset<128>key;
    rep(i,0,127) key[i]=W[i+128*Nr];
    AddRoundKey(key);
   // DeBug128(key);
    rpe(round,Nr-1,1){
        InvSubBytes();
        //DeBug128(Out);成功
        InvShiftRows();
        //DeBug128(Out);成功
        InvMixColumns(Out);
        //DeBug128(Out);成功
        rep(i,0,127)key[i]=W[round*128+i];
       // DeBug128(key);
        AddRoundKey(key);
        //DeBug128(Out);
    }
    InvSubBytes();
    InvShiftRows();
    rep(i,0,127)key[i]=W[i];
    AddRoundKey(key);
    printf("解密后的明文为:\n");
    DeBug128(Out);
}
int main(){
   //freopen("in.txt","r",stdin);
    printf("----------------------------\n");
    printf("    欢迎使用AES加解密系统    \n");
    printf("----------------------------\n");
    while(1){
        printf("请输入模式 1:加密 2:解密 3:退出\n");
        cin>>flag;
        if(flag==3){
            printf("退出成功");
            break;
        }
//首先输入密钥    
        InitMainKey();
////主密钥输入成功
        GetNr();GetRcon();
        KeyExpansion();

////扩展密钥扩展成功
        Init();
////输入明文或密文成功
        if(flag==1) Encrypt();//加密算法成功
        else Decrypt();
    }   
    return 0;
}