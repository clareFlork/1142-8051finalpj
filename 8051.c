#include <8051.h>

#define nop() __asm nop __endasm
//mode 0 =>10進位    mode 1=>8進位  mode 2=>2進位 
//  0 1   2      3     0 1 2 3      0 1 x x      
//  4 5   6      7     4 5 6 7      x x x x       
//  8 9   +      -     x x + -      x x + -       
//  * / history mode   * / h m      * / h m      

unsigned char digit[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const unsigned char num[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,0x40};
unsigned char rem[10]={0};
int store[5]={0};           // 只存5筆整數結果，共10 bytes
unsigned char store_count=0;// 存了幾筆
unsigned char bin_1[8]={0};
unsigned char bin_2[8]={0};
unsigned char bin_R[8]={0};

unsigned char first=0;
unsigned char index=7;
unsigned char carry=0;
unsigned char j=0;
unsigned char k=0;
unsigned char mode=0;//0是10進位 1是八進位 2是2進位 3是16進位
unsigned char load=0;//確定數字存好
int total=0;
int temp_1=0;
int temp_2=0;
void bin_calculate(unsigned char h);//2進位加減運算函數
void add(unsigned char carry_1);//加法的函數
char status = 0xff;//控制閃爍
void blink(void);//二進位閃爍
void clear(void);//清理資料
void store_num(int result);//存資料
void delay(unsigned int ms) 
{
    unsigned int x, y;
    for (x = 0; x < ms; x++)
        for (y = 0; y < 120; y++);
}

void display(void) 
{
    unsigned char i;
    for (i = 0; i < 8; i++) 
    {
        P1 = i;
        if (digit[i] == 0xFF)
            P2 = 0x00;
        else
            P2 = num[digit[i]];
        delay(70);
        P2 = 0x00;
    }
}
// 去彈跳等待函數（等待期間畫面依然穩定顯示）
void wait_stable(void)
{
    unsigned char i;
    for (i = 0; i < 5; i++){
        display(); 
    }
}

unsigned char keyscan(void) 
{
    unsigned char col, row, key_value = 0xFF;
    unsigned char scan_code[4] = {0xFE, 0xFD, 0xFB, 0xF7};

    for (col = 0; col < 4; col++) 
    {
        P0 = scan_code[col];
        nop(); nop(); nop(); nop();
        row = P0 & 0xF0;

        if (row != 0xF0) 
        {
            delay(10);
            row = P0 & 0xF0;

            if (row != 0xF0) 
            {
                //10進位
                if(mode==0)
                {
                    if (row == 0xE0) {
                        if(col == 0)key_value = 0;
                        else if (col == 1)key_value = 1;
                        else if (col == 2) key_value = 2;
                        else if (col == 3) key_value = 3;
                        rem[j++]=key_value;
                    }
                    else if (row == 0xD0) {
                        if(col == 0) key_value = 4;
                        else if (col == 1) key_value = 5;
                        else if (col == 2) key_value = 6;
                        else if (col == 3) key_value = 7;
                        rem[j++]=key_value;
                    }
                    else if (row == 0xB0) {
                        if(col == 0) 
                        {
                            key_value = 8;
                            rem[j++]=key_value;
                        }
                        else if (col == 1) 
                        {   
                            key_value = 9;
                            rem[j++]=key_value;
                        }
                        else if (col == 2)
                        {
                             clear();
                             total=temp_1+temp_2;
                             store_num(total);
                             if (total == 0)
                              {
                                    digit[7] = 0;
                              } 
                              else {
                                
                                unsigned char d = 7;
                                while (total > 0 && d < 255) {
                                    digit[d] = total % 10;
                                    total /= 10;
                                    d--;
                                }
                                temp_1=temp_2=0;
                            }
                           /* store[k]=rem[j-2]+rem[j-1];
                            key_value=store[k];
                            k++;
                            */
                        }
                        else if(col==3)
                        {
                            clear();
                               total=(temp_1>temp_2)?temp_1-temp_2:temp_2-temp_1;
                                store_num(total);
                                if (total == 0)
                                {
                                        digit[7] = 0;
                                } 
                                else {
                                    
                                    unsigned char d = 7;
                                    while (total > 0 && d < 255) {
                                        digit[d] = total % 10;
                                        total /= 10;
                                        d--;
                                    }
                                    if(temp_1<temp_2)
                                    {
                                        digit[d]=10;
                                    }
                                    temp_1=temp_2=0;
                                }
                           
                            /*store[k] = (rem[j-2] > rem[j-1]) ? (rem[j-2] - rem[j-1]) : (rem[j-1] - rem[j-2]);
                            key_value=store[k];
                            k++;
                            */
                        }
                    }
                    else if(row == 0x70)
                    {
                        if(col==0)
                        {
                            clear();
                            total=temp_1*temp_2;
                            store_num(total);
                            if (total == 0)
                            {
                                digit[7] = 0;
                            } 
                            else {
                                unsigned char d = 7;
                                while (total > 0 && d < 255) {
                                    digit[d] = total % 10;
                                    total /= 10;
                                    d--;
                                }
                                temp_1=temp_2=0;
                           /* store[k]=rem[j-2]*rem[j-1];
                            key_value=store[k];
                            k++;
                            */}
                        }
                        else if(col==1)
                        {
                            clear();
                            total=(temp_1>temp_2)?temp_1/temp_2:temp_2/temp_1;
                            store_num(total);
                             if (total == 0)
                              {
                                    digit[7] = 0;
                              } 
                              else {
                                
                                unsigned char d = 7;
                                while (total > 0 && d < 255) {
                                    digit[d] = total % 10;
                                    total /= 10;
                                    d--;
                                }
                                temp_1=temp_2=0;
                            }
                            /*store[k] = (rem[j-2] > rem[j-1]) ? (rem[j-2] / rem[j-1]) : (rem[j-1] / rem[j-2]);
                            key_value=store[k];
                            k++;
                            */
                        }
                        else if(col==2)
                        {
                            if (store_count > 0)
                            {
                                store_count--;
                                int prev = store[store_count % 5];
                                clear();
                                if (prev == 0)
                                {
                                    digit[7] = 0;
                                }
                                else
                                {
                                    unsigned char d = 7;
                                    while (prev > 0 && d < 255) {
                                        digit[d] = prev % 10;
                                        prev /= 10;
                                        d--;
                                    }
                                }
                            }
                        }
                        else if(col==3)
                        {
                            mode=1;
                        }
                    }
                }
                //8進位
                else if( mode==1)
                {
                    if (row == 0xE0)
                     {
                        if(col == 0)key_value = 0;
                        else if (col == 1)key_value = 1;
                        else if (col == 2) key_value = 2;
                        else if (col == 3) key_value = 3;
                        rem[j++]=key_value;
                    } 
                    else if (row == 0xD0)
                     {
                        if(col == 0) key_value = 4;
                        else if (col == 1) key_value = 5;
                        else if (col == 2) key_value = 6;
                        else if (col == 3) key_value = 7;
                        rem[j++]=key_value;
                    }
                    else if (row == 0xB0) {
                        if (col == 2)
                        {
                            clear();
                            total=temp_1+temp_2;
                            store_num(total);
                             if (total == 0)
                              {
                                    digit[7] = 0;
                              } 
                              else {
                                unsigned char d = 7;
                                while (total > 0 && d < 255) {
                                    digit[d] = total % 8;
                                    total /= 8;
                                    d--;
                                }
                                temp_1=temp_2=0;
                            }                       
                           /* store[k]=rem[j-2]+rem[j-1];
                            key_value=store[k];
                            k++;
                            */
                        }
                        else if(col==3)
                        {
                            clear();
                            total=(temp_1>temp_2)?temp_1-temp_2:temp_2-temp_1;
                            store_num(total);
                             if (total == 0)
                              {
                                    digit[7] = 0;
                              } 
                              else {
                                
                                unsigned char d = 7;
                                while (total > 0 && d < 255) {
                                    digit[d] = total % 8;
                                    total /= 8;
                                    d--;
                                }
                                temp_1=temp_2=0;
                            }
                            /*store[k] = (rem[j-2] > rem[j-1]) ? (rem[j-2] - rem[j-1]) : (rem[j-1] - rem[j-2]);
                            key_value=store[k];
                            k++;
                            */
                        }
                    }
                    else if(row == 0x70)
                    {
                        if(col==0)
                        {
                            clear();
                            store_num(total);
                            total=temp_1*temp_2;
                            if (total == 0)
                            {
                                digit[7] = 0;
                            } 
                            else {
                                unsigned char d = 7;
                                while (total > 0 && d < 255) 
                                {
                                    digit[d] = total % 8;
                                    total /= 8;
                                    d--;
                                }
                                temp_1=temp_2=0;
                            }
                            /*store[k]=rem[j-2]*rem[j-1];
                            key_value=store[k];
                            k++;
                            */
                        }
                        else if(col==1)
                        {
                            /*store[k] = (rem[j-2] > rem[j-1]) ? (rem[j-2] / rem[j-1]) : (rem[j-1] / rem[j-2]);
                            key_value=store[k];
                            k++;
                            */
                           clear();
                           total=(temp_1>temp_2)?temp_1/temp_2:temp_2/temp_1;
                           store_num(total);
                            if (total == 0)
                            {
                                digit[7] = 0;
                            } 
                            else { 
                                unsigned char d = 7;
                                while (total > 0 && d < 255) {
                                    digit[d] = total % 8;
                                     total /= 8;
                                     d--;
                                }
                            temp_1=temp_2=0;
                            }
                            
                        }
                        else if(col==2)
                        {
                            if (store_count > 0)
                            {
                                store_count--;
                                int prev = store[store_count % 5];
                                clear();
                                if (prev == 0)
                                {
                                    digit[7] = 0;
                                }
                                else
                                {
                                    unsigned char d = 7;
                                    while (prev > 0 && d < 255) {
                                        digit[d] = prev % 10;
                                        prev /= 10;
                                        d--;
                                    }
                                }
                            }
                        }
                        else if(col==3)
                        {
                            mode=2;
                        }
                    }
                }
                //2進位
                else if(mode==2)
                {
                    if(row==0x70&&col==3)
                    {
                        if(col==3)
                        {
                          mode=0;
                        }
                        else if(col==1)//乘法
                        {
                            bin_calculate(2);
                        }
                    }
                    else if (row == 0xE0) 
                    {
                        if(col == 0)key_value = 0;
                        else if (col == 1)key_value = 1;
                    }
                    else if (row == 0xB0) 
                    {
                        //加法
                        if (col == 2)
                        {
                            bin_calculate(0);  
                                                      
                        }
                        //減法
                        else if(col==3)
                        {
                       
                            bin_calculate(1);
                            
                        
                        }   
                        for (unsigned char z = 0; z < 8; z++) 
                        {
                            digit[z] = bin_R[z];  // 一次顯示全部8位結果
                        }  
                    }
                    else if(row == 0x70)
                   {
                        if(col==2)
                        {
                           if (store_count > 0)
                            {
                                store_count--;
                                int prev = store[store_count % 5];
                                for (signed char z = 7; z >=0; z--) 
                                {
                                    bin_R[z] =prev%2;
                                    prev/=2; 
                                    digit[z] = bin_R[z];
                                }
                            }
                        }
                   }    
                }
                
            }
                // 等放開，期間繼續 display 維持顯示
                // 先把 P1 恢復安全狀態再讀 P0
                P1 = 0xFF;
                P2 = 0x00;
                while (1) {
                    P0 = scan_code[col];
                    nop(); nop(); nop(); nop();
                    if ((P0 & 0xF0) == 0xF0) break;  // 放開了
                    display();  // 等待期間繼續顯示
                }

                return key_value;
        }
    }
    return 0xFF;
}

void main(void) {
    unsigned char btn_prev;   /* 上一次按鍵狀態 (0=壓著, 1=放開) */
    unsigned char btn_now;    /* 本次按鍵狀態   (0=壓著, 1=放開) */
    //1(放開)	0 (按下)	按下瞬間 (Edge Down)	準備開始去彈跳
    //0 (按下)	0 (按下)	持續壓著 (Hold)	什麼都不做（忽略，防止重複觸發）
    //0 (按下)	1 (放開)	放開瞬間 (Edge Up) 就在此時執行計算與儲存！
     //1 (放開)	1 (放開)	持續放開 (Idle)	沒事發生
   // unsigned int  scan_count; /* 掃描次數計數器 */
    unsigned char base=1;
    unsigned char key, i;
    
    EA = 0;
    P3_2 = 1;     /* P3.2 設為輸入 (按鍵腳位) */
    btn_prev   = 1; /* 初始視為放開狀態 */

    while (1) {
        key = keyscan();
            
        /* ---- Step 2: 讀取按鍵當下狀態 ---- */
        /* P3.2 低電位=壓著=0，高電位=放開=1 */
        btn_now = (P3_2 == 0) ? 0 : 1;
        if (btn_prev == 0 && btn_now == 1)  /* 放開瞬間 */
        {
            wait_stable();          /* 等彈跳結束 */
            if (P3_2 == 1)          /* 確認真的放開，非彈跳雜訊 */
            {
                load=1;
            }
            else
            {
                btn_now = 0;        /* 彈跳雜訊，尚未真正放開 */
            }
        }
        if (btn_prev == 1 && btn_now == 0)       /* 按下瞬間 */
        {
            wait_stable();          /* 等彈跳結束 */
            if (P3_2 == 0)          /* 確認真的按下，非彈跳雜訊 */
            {
               load=0;
            }
            else
            {
                btn_now = 1;        /* 彈跳雜訊，忽略此次按下 */
            }
        }

        btn_prev = btn_now;         /* 更新按鍵歷史狀態 */
        if(load==1)
        {
            load = 0;
            total = 0;
            base = 1;

            unsigned char d;
            for (d = 0; d < 8; d++) {
                digit[d] = 0xFF;
            }
            if(first==0){
                if(mode==0){
                    for (signed char d = j - 1; d >= 0; d--) {
                    temp_1 += rem[d] * base;
                    base *= 10;
                    }
                }
                else if(mode==1)
                {
                    for (signed char d = j - 1; d >= 0; d--) {
                    temp_1 += rem[d] * base;
                    base *= 8;
                    }
                }
                first=1;
                for (unsigned char i = 0; i < 10; i++)
                    rem[i] = 0;
            }
            else
            {
                if(mode==0){
                    for (signed char d = j - 1; d >= 0; d--) {
                    temp_2 += rem[d] * base;
                    base *= 10;
                    }
                }
                else if(mode==1)
                {
                    for (signed char d = j - 1; d >= 0; d--) {
                    temp_2 += rem[d] * base;
                    base *= 8;
                    }
                }
                //total=temp_1+temp_2;
                first=0;
                for (unsigned char i = 0; i < 10; i++)
                    rem[i] = 0;
                
            }
            j = 0;
    
           
        }
        for (unsigned char d = 0; d <20; d++) {
            display();
        }
        if (key != 0xFF&&(mode== 0||mode==1)) 
        {
          
            for (i = 0; i < 7; i++) {
                digit[i] = digit[i + 1];
            }
            if(mode==0)
            {
                if (key>= 10) {
                digit[6] = key / 10;  // 十位
                digit[7] = key % 10;  // 個位
                } 
                else 
                {
                digit[7] = key;       // 個位
                }
            }
            else if(mode==1)
            {
                if (key>= 8) {
                digit[6] = key / 8;  // 十位
                digit[7] = key % 8;  // 個位
                } 
                else 
                {
                digit[7] = key;       // 個位
                }
            }  
        }
        if(key != 0xFF&&mode==2)
        {
            if(first==0)
            {
                bin_1[index]=key;
                digit[index]=key;
                index--;
                if(index==255)
                {
                   first=1;
                   index=7;
                   //輸入完成就閃爍
                   blink();
                }
            }
            else if(first==1)
            {
                bin_2[index]=key;
                digit[index]=key;
                index--;
                if(index==255)
                {
                   first=0;
                   index=7;
                   //輸入完成就閃爍
                   blink();
                }
                
            }
        }
    }
}
void bin_calculate(unsigned char h) 
{
    if(h==0)
    {
        add(0);
    }
    else if(h==1)
    { 
       for(signed char z = 7; z >=0; z--) 
        {
            bin_2[z] = (bin_2[z] == 0) ? 1 : 0;               
        }
        add(1);
    }
    else if(h==2)
    {

    }
    // 計算完成後，統一將結果同步到顯示陣列中
    for (unsigned char z = 0; z < 8; z++) 
    {
        digit[z] = bin_R[z];
    }
    int result = 0;
    unsigned char base=1;
    for (unsigned char z = 7; z !=255; z--) {
        result += bin_R[z]*base;
        base*=2;
    }
    store_num(result);
}
void add(unsigned char carry_1)
{
        carry = carry_1; // 每次開始計算前，先將進位清零
        for(signed char z = 7; z >= 0; z--) 
        {
            if(bin_1[z] == 1 && bin_2[z] == 1) 
            {
                if(carry == 1) 
                {
                    carry = 1;
                    bin_R[z] = 1;
                } 
                else 
                {
                    carry = 1;
                    bin_R[z] = 0;
                }
            } 
            else if((bin_1[z] == 0 && bin_2[z] == 1) || (bin_1[z] == 1 && bin_2[z] == 0)) 
            {
                if(carry == 1) 
                {
                    carry = 1;
                    bin_R[z] = 0;
                } 
                else 
                {
                    carry = 0;
                    bin_R[z] = 1;
                }
            } 
            else // 兩者皆為 0 的情況
            {
                if(carry == 0) 
                {
                    carry = 0;
                    bin_R[z] = 0;
                } 
                else 
                {
                    carry = 0;
                    bin_R[z] = 1;
                }
            }
        }
}
void blink(void)
{
    P2 = 0x00;
    for (unsigned i = 0; i < 100; i ++) {
        for (unsigned j = 0; j < 255; j ++);
    }
    status = ~status;
}
void clear(void)
{
    for (unsigned char i= 0; i < 8; i++) {
    digit[i] = 0xFF; // 先把 8 個七段顯示器全部熄滅（清空舊畫面）
    }
}
void store_num(int result)
{
    store[store_count % 5] = result;
    store_count++;
}
