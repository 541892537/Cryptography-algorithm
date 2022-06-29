#include "des.h"


void Get_sKey(){
    bitset<56>key,result;
    rep(i,0,55) key[i] = Key[ key_table[i] - 1 ];//置换选择1后的密文
    rep(T,0,15){//循环移位16次
        //处理边缘位        
        rep(i,0,bit_shift[T]-1){
            result[i] = key[i] ;
			result[28+i] = key[28 + i] ;
        }
        key>>=bit_shift[T];
        rep(i,0,bit_shift[T]-1){
            key[27-bit_shift[T]+i+1] = result[i];
            key[55-bit_shift[T]+i+1] = result[i+28];
        }
        rep(i,0,47){
            sKey[T][i] = key[comp_perm[i]-1];   
        }
    }

}

void first_ip(){
    rep(i,0,31) L[i] = In[ip_table[i] -1];
    rep(i,0,31) R[i] = In[ip_table[i+32] - 1];
}

void second_ip(){
    rep(i,0,63){
        if(rip_table[i]<=32) Out[i] = L[rip_table[i] - 32 - 1] ;
        else Out[i] = R[rip_table[i]  -1];
    }
}

void des_turn(bitset<32> &L,bitset<32> &R,bitset<48> &K){
    bitset<48> block;//选择运算块
    rep(i,0,47){
        block[i] = R[expa_perm[i]-1];
    }//48位中间结果
    block^=K;//与子密钥异或
    
    //进行S盒替代
    rep(i,0,7){
        int row = block[6*i]*2 + block[6*i+5];
        int col = block[6*i+1]*8 + block[6*i+2] *4 + block[6*i +3] *2 +block[6*i+4];
        int val=sbox[i][row*16+col];
        rpe(j,3,0) block[4*i+(3-j)]=(val&(1<<j)); 
    }
    //进行置换运算P
    bitset<32>tmp;
    rep(i,0,31) tmp[i] = block[p_table[i]-1];
    L^=tmp;
}
void solve(int sign){
    first_ip();//初始置换
    
    if(sign==1) {//加密
        rep(i,0,15){
            des_turn(L,R,sKey[i]);
            if(i!=15) swap(L,R);   
        }
    }
    else{//解密
        rpe(i,15,0){
            des_turn(L,R,sKey[i]);
            if(i!=0) swap(L,R);
        }
    }
    
    second_ip();
}

int main(){
    char tmp[64];
    printf("------------------------------\n");
    printf("      欢迎使用des加密系统      \n");
    printf("------------------------------\n");
    while(1){     
        printf("请选择你的操作: 1 加密 2 解密 3 退出\n");
        int sign;
        cin>>sign;
        if(sign == 3) {
            printf("退出成功");
            break;
        }
        if(sign==1) printf("请输入64位明文:\n");
        else printf("请输入64位密文\n");
        scanf("%s",tmp);
        rep(i,0,63) In[i]=(unsigned long long ) (tmp[i] - '0');
        printf("请输入64位密钥\n");
        scanf("%s",tmp);
        rep(i,0,63) Key[i]=(unsigned long long ) (tmp[i] - '0');
        clock_t starttime=clock();
//得到子密钥
        Get_sKey();
//开始解决
        solve(sign);
//输出
        if(sign==2) printf("明文:");
        else printf("密文:");
        rep(i,0,63) cout<<Out[i];
        pts;
        clock_t endtime=clock();
        if(sign==2) printf("解密");
        else printf("加密");
       // cout<<(double)(endtime - starttime)<<endl;
        printf("所用时间:%lf s\n",1.0*(endtime - starttime)/CLOCKS_PER_SEC);
    }
    return 0;
}