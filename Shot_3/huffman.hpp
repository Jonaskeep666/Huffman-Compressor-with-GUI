#ifndef _HUFFMAN_HPP_
#define _HUFFMAN_HPP_

#include <iostream>
using namespace std;

class CharCode;
class CharList;
class Hufftree;
class MinHeap;
class Buffer;

/*********** 通用函數 ***********/
template <class T>
void setBit(T &byte,int k,int bit){
    bit == 1 ? (byte |= (1<<k)):(byte &= ~(1<<k));
}

template <class T>
void printInByte(T &byte){
    for(int i=7;i>=0;--i){
        if( (byte >> i)&1 ){
            cout << '1';
        }
        else cout << '0';
    }
}
void printInSize(unsigned int num,int cdsize){
    for(int i=cdsize-1; i>=0; --i){
        if( (num >> i)&1 ){
            cout << '1';
        }
        else cout << '0';
    }
}

// 將 Int變數的 每一個bit 單獨以「字元」格式 存在 buffer 中
void saveIntBuff(unsigned int num,int cdsize,unsigned char *buff,int& pos){
    for(int i=cdsize-1; i>=0; --i){
        if( (num >> i)&1 ){
            buff[pos]= '1'; pos++;
            // cout << '1';
        }
        else{
            buff[pos]= '0'; pos++;
            // cout << '0';
        }
    }
}
// 將 Char變數的 每一個bit 單獨以「字元」格式 存在 buffer 中
void saveCharBuff(unsigned char ch,unsigned char *buff,int& pos){
    for(int i=0; i<8; ++i){
        if( (ch >> i)&1 ){
            buff[pos]= '1'; pos++;
            // cout << '1';
        }
        else{
            buff[pos]= '0'; pos++;
            // cout << '0';
        }
    }
}
// Timer 計時器：mode=1 開始計時；mode=0 結束計時，並輸出時間
void tmr(bool mode){
    static clock_t t1=0,t2=0;
    double total =0.0,sec=0.0;
    int min=0;

    if(mode){
        t1 = clock();  
    }
    else{
        t2 = clock();    
        total = ((double)(t2-t1))/CLOCKS_PER_SEC;
        min = (int)total/60;
        sec = total - min*60.0;
        cout << "Time: " << min << "m " << sec << "s" << endl;
    }
}

/*********** Class ***********/

/*------- Filename -------*/
class Filename{
    public:
        void setOrg(const char* str){
            short n;
            for(n=0;str[n]!='\0';++n);
            opath = n+fpath;
            origFile = new char[opath];
            sprintf(origFile,"%s%s",folder,str);

            // bpath = opath + 4;
            // binFile = new char[bpath];
            // sprintf(binFile,"%s.bin",origFile);

            cout << origFile << endl;
            // cout << binFile << endl;
        }
        void setBin(const char* str){
            short n;
            for(n=0;str[n]!='\0';++n);
            bpath = n+fpath;
            binFile = new char[bpath];
            sprintf(binFile,"%s%s",folder,str);

            // opath = bpath - 4;
            // origFile = new char[opath];
            // for(int i=0;i<opath;++i)
            //     origFile[i] = binFile[i];

            cout << binFile << endl;
            // cout << origFile << endl;
        }
        void setPath(const char* str){
            short n;
            for(n=0;str[n]!='\0';++n);
            fpath = n;
            folder = new char[n];
            sprintf(folder,"%s",str);
            // cout << folder << endl;
        }
        char* bin(){return binFile;}
        short binlen(){return bpath;}
        char* org(){return origFile;}
        short orglen(){return opath;}
    private:
        char *origFile; // 指向 未壓縮的檔案 名稱/路徑 的字元陣列
        short opath;    // 未壓縮檔名 字元陣列 的元素個數
        char *binFile;  // 指向 壓縮檔案 名稱/路徑 的字元陣列
        short bpath;    // 壓縮檔名 字元陣列 的元素個數
        char *folder;   // 目前未使用（指向 資料夾路徑）
        short fpath;    // 目前未使用
};

/*------- CharCode -------*/

class CharCode{
    friend class CharList;
    friend class MinHeap;
    friend class Hufftree;
    friend class Buffer;
    friend ostream& operator<<(ostream&,CharCode);
    friend ostream& operator<<(ostream&,CharList);
    friend class MainWindow;

    public:
        void setChar(unsigned int f, unsigned char c, unsigned int cd, unsigned char s){
            freq = f; ch = c; code = cd; cdsize = s;
        }
    private:
        unsigned int freq;      //字元出現的次數
        unsigned char ch;       //字元
        unsigned int code;      //對應的 huffman編碼 
        unsigned char cdsize;   //huffman編碼 的 位元數 ex. [10010] cdsize=5
}; 

/*------- CharList -------*/

class CharList{
    friend ostream& operator<<(ostream&,CharList);
    friend class MinHeap;
    friend class Hufftree;
    friend class Buffer;
    friend void printInfo(Buffer& Input,CharList& clist);
    friend class MainWindow;

    public:
        CharList(){ 
            // 1 byte 最多只有256種可能，而 head[0]不存值，故規劃257個空格
            counTable = new CharCode[257];
            ftable = NULL;
            table_size = 0;
            for(int i=0;i<257;++i)
                counTable[i].setChar(0,0,0,0); 
        }

        // Build Freq Table
        void countChar(unsigned char c);
        void buildFreqTable(Buffer& dcInput);

        // I/O ftable
        void printFtable();
        void writeFtable(FILE* eptr);   //寫入檔案
        void readFtable(FILE* eptr);    //自檔案讀出

        // search ftable
        int iBinSearch(unsigned char ch);

    private:
        CharCode* counTable;    // 暫存 字元 & 出現次數 的陣列
        CharCode* ftable;       // 儲存 字元 & 出現次數 的陣列(只存至少出現一次的字元)
        unsigned short table_size;  // ftable的大小，i=1 ~ i<table_size，ftable[0] 不存值
        unsigned int freq_size;     // 要用多大的空間儲存「出現次數」ex. freq>256 至少需要 2 bytes
};

/*------- Buffer -------*/
class Buffer{
    friend class CharList;
    friend class Hufftree;
    friend void readDecodeFile(Buffer& dcInput,Filename& path);
    friend void encodeFile(Buffer& dcInput,CharList& clist,MinHeap& mhp,Hufftree& tree);
    friend void readEncodeFile(CharList& clist,Buffer& ecInput,Filename& path);
    friend void printInfo(Buffer& Input,CharList& clist);
    friend class MainWindow;

    public:
        // 宣告 陣列的大小
        void createBuffer(int n);
        void createCharArr(int n);
        // 印出 陣列
        void printBuffer();
        void printCharArr();

        // bsize & csize 的 檔案 讀寫
        void writeSize(FILE* eptr);
        void readSize(FILE* eptr);

        // 將 存著 壓縮碼 的 buffer 寫入 檔案中
        void writeEncodeBuff(FILE* eptr);
        // 將 存著 壓縮碼 的 檔案 存入 buffer 中
        void readEncodeBuff(FILE* eptr);
        // 將 存著 解壓縮的資料 寫入 檔案中
        void writeDecodeChar(FILE* eptr);

    private:
        unsigned char* buffer;  //儲存 壓縮過的編碼 ex. '0','1','0','0','1','1',...
        int bsize;              //buffer陣列的元素個數  i=0 ~ i<bsize
        unsigned char *charArr; //儲存 未壓縮過的編碼 ex. 'A','B','A','C'
        int csize;              //charArr陣列的元素個數 i=0 ~ i<csize

};

/*------- HuffNode -------*/
class HuffNode{
    friend class Hufftree;
    friend class MinHeap;
    friend ostream& operator<<(ostream& os ,MinHeap &minheap);

    HuffNode(unsigned char ch,unsigned int w,HuffNode* L,HuffNode* R ){
        data = ch; weight = w; LChild = L; RChild = R;
    }
    void set(unsigned char ch,unsigned int w,HuffNode* L,HuffNode* R ){
        data = ch; weight = w; LChild = L; RChild = R;
    }
    private:
        unsigned char data;     //儲存「字元」
        unsigned int weight;    //儲存「出現次數」
        HuffNode *LChild, *RChild;
};

/*------- Hufftree -------*/
class Hufftree{
    friend class MinHeap;
    public:
        Hufftree(){ root = NULL; size = 0;}

        // Build Tree：將 左右兩個子樹 合併為一個Tree，並回傳 root 的位址
        HuffNode* buildTree(HuffNode* L,HuffNode* R);

        // Traversal
        void visit(HuffNode* node);
        void inorder(){inorder(root);};
        void inorder(HuffNode* node);
        void preorder(){preorder(root);};
        void preorder(HuffNode* node);

        // Traversal & Generate Code Table
        // 採用 遞迴 走訪 的概念，走訪整棵樹 直到Leaf 直到求得 Huffman code
        void genCode(CharList& flist);
        void genCode(CharList& flist,HuffNode* node,unsigned int code,unsigned char cdsize);

        // Encoding & decoding
        void encode(CharList& flist,Buffer& dcInput);
        void decode(Buffer& ecInput);

    private:
        HuffNode* root;
        int size;
};

/*------- MinHeap -------*/
class MinHeap{
    friend ostream& operator<<(ostream& os ,MinHeap &minheap);
    friend class MainWindow;
    public:
        MinHeap(){
            // heap 初始要儲存 所有 出現過的char
            // 因為 1 byte 最多只有256種可能(0000,0000～1111,1111) -> 出現過的char只有256種
            // 其中 heap[0] 不存值，故宣告 heap 的大小為 257
            heap = new HuffNode*[257];    
            size = 0;
        }

        bool isEmpty(){return size==0;}

        // Build Heap from Freq Table：將 ftable 的資料 存入 HuffNode，並插入 Heap
        void buildHeap(CharList& flist);

        // Maintainance
        HuffNode* Pop();
        void Push(HuffNode* newNode);
        void adjust(int rootIdx);

        // Build Huffman Tree：建立 Huffman Tree 的主要演算法
        void buildHufftree(Hufftree& hufftree);

    private:
        HuffNode **heap;
        int size;
};

/*********** Class methods or friend function ***********/

/*------- CharCode function -------*/

ostream& operator<<(ostream& os ,CharCode ccd){
    os << "(" << (char)ccd.ch << "," << ccd.freq << ")[";
    printInSize(ccd.code,ccd.cdsize);
    cout << "],{" << (int)ccd.cdsize << "}";
    return os;
}


/*------- CharList function -------*/

ostream& operator<<(ostream& os ,CharList flist){
    for(int i=1;i<=257;++i){
        // if(flist.counTable[i].freq>=1){
            if(!((i-1)%10)) os << "\n";
            os << "(" << flist.counTable[i].ch << "," << flist.counTable[i].freq << ")\t";
        // }
    }
    return os;
}

void CharList::countChar(unsigned char c){
    unsigned int idx = c+1;
    counTable[idx].ch = c;
    counTable[idx].freq++;
}

void CharList::buildFreqTable(Buffer& dcInput){

    // 計算 字串元素 的 出現次數
    for(int i=0;i<dcInput.csize;++i){
        countChar(dcInput.charArr[i]);
    }

    // 只儲存有值的元素 在 ftable
    table_size=1;   //初始化 table_size，[0][1]...[table_size-1]
    for(int i=1;i<=257;++i)
        if(counTable[i].freq>=1) table_size++;
    // cout << table_size <<endl; 
    
    ftable = new CharCode[table_size];
    int j=1;
    unsigned int maxf = 0;
    for(int i=1;j<table_size;++i){
        if(counTable[i].freq>=1){
            ftable[j]=counTable[i];
            if(ftable[j].freq > maxf) maxf = ftable[j].freq;
            j++;
        }
    }

    // 根據 max_freq 決定 要規劃多大空間(freq_size) 儲存 ftable[i].freq(出現次數)
    if(maxf < 255) freq_size = 1;
    else if(maxf > 255 && maxf < 65535) freq_size = 2;
    else freq_size = 4;

    // cout << maxf << endl;
}

void CharList::printFtable(){
    for(int i=1;i<table_size;++i) cout << ftable[i] << endl;
}

void CharList::writeFtable(FILE* eptr){
    
    fwrite(&table_size,2,1,eptr);

    // 根據 freq_size 決定 要規劃多大空間 儲存 ftable
    fwrite(&freq_size,1,1,eptr);

    // 寫入 ftable
    for(int i=1;i<table_size;++i){
        fwrite(&ftable[i].ch,sizeof(unsigned char),1,eptr);
        fwrite(&ftable[i].freq,freq_size,1,eptr);
    }
}

void CharList::readFtable(FILE* eptr){

    fread(&table_size,2,1,eptr);

    // 根據 讀入的 freqSize 決定 要規劃多大空間 儲存 ftable
    unsigned char freqsize;
    fread(&freqsize,1,1,eptr);
    freq_size = freqsize;

    ftable = new CharCode[table_size];

    for(int i=1; i < table_size; ++i){
        fread(&ftable[i].ch,sizeof(unsigned char),1,eptr);
        if(freqsize == 1) {
            unsigned char temp;
            fread(&temp,freq_size,1,eptr);
            ftable[i].freq = temp;
        }
        else if(freqsize == 2){
            unsigned short temp;
            fread(&temp,freq_size,1,eptr);
            ftable[i].freq = temp;
        }
        else if(freqsize == 4){
            fread(&ftable[i].freq,freq_size,1,eptr);
        }
    }
}

int CharList::iBinSearch(unsigned char ch){
    unsigned short left=1,right=table_size-1,mid;
    
    while(left <= right){
        mid = (left+right)/2;
        if(ch > ftable[mid].ch) left = mid+1;
        else if(ch < ftable[mid].ch) right = mid-1;
        else return mid;
    }
    return 0;
}

/*------- Buffer function -------*/

void Buffer::createBuffer(int n){
    buffer = new unsigned char[n]; 
    bsize = n;
}
void Buffer::createCharArr(int n){
    charArr = new unsigned char[n]; 
    csize = n;
}
void Buffer::printBuffer(){
    for(int i=0;i<bsize;++i) cout << buffer[i] <<",";
    cout << endl;
}
void Buffer::printCharArr(){
    for(int i=0;i<csize;++i) cout << charArr[i] <<",";
    cout << endl;
}

void Buffer::writeSize(FILE* eptr){
    fwrite(&bsize,sizeof(int),1,eptr);
    fwrite(&csize,sizeof(int),1,eptr);
}
void Buffer::readSize(FILE* eptr){
    bsize = 0; csize = 0;
    fread(&bsize,sizeof(int),1,eptr);
    fread(&csize,sizeof(int),1,eptr);
}
void Buffer::writeEncodeBuff(FILE* eptr){
    int i;
    int rmd = bsize % 8;        // 求得 buffer陣列 中 最後方不足8個 的元素個數
    unsigned char buff = 0;

    // 先將 8個為一組 寫入檔中
    for(i=0;i<(bsize-rmd);++i){ 
        if(buffer[i]=='0') setBit(buff,i%8,0);
        else if(buffer[i]=='1') setBit(buff,i%8,1);
        if((i%8)==7){
            fwrite(&buff,1,1,eptr);
            // printInByte(buff);
            // cout << ",";
            buff = 0;
        }
    }
    // 再將 剩下不足8個的元素「補上0」揍滿 1byte 寫入檔案中
    for(int j=0;j<8;++j,++i){
        if(j<rmd){
            if(buffer[i]=='0') setBit(buff,i%8,0);
            else if(buffer[i]=='1') setBit(buff,i%8,1);
        }
        else setBit(buff,i%8,0);
    }
    fwrite(&buff,1,1,eptr);
}
void Buffer::readEncodeBuff(FILE* eptr){
    unsigned char buff = 0;

    // 算得 壓縮檔案中 buffer部分 所佔用的 bytes 數 
    // ex. bsize = 18, len = (18 - 2 + 8)/8 = 24/8 = 3
    int len = (bsize-(bsize%8)+8)/8;

    // 讀出 len 個 bytes，並存回 buffer 陣列
    for(int i=0,j=0;i<len;++i){
        fread(&buff,1,1,eptr);
        // printInByte(buff);
        // cout << ",";
        saveCharBuff(buff,buffer,j);
    }
}
void Buffer::writeDecodeChar(FILE* eptr){
    for(int i=0;i<csize;++i)
        fwrite(&charArr[i],sizeof(unsigned char),1,eptr);
}

/*------- Hufftree function -------*/

HuffNode* Hufftree::buildTree(HuffNode* L,HuffNode* R){
    HuffNode* parent = new HuffNode(L->data, L->weight + R->weight , L, R);
    size++;
    return parent;
}

void Hufftree::visit(HuffNode* node){
    cout << "(" << node->data << "," << node->weight << ")\t";

}
void Hufftree::inorder(HuffNode* node){ //LDR
    if(node){
        inorder(node->LChild);
        visit(node);
        inorder(node->RChild);
    }
}
void Hufftree::preorder(HuffNode* node){ //LRD
    if(node){
        visit(node);
        preorder(node->LChild);
        preorder(node->RChild);   
    }
}

void Hufftree::genCode(CharList& flist){
    genCode(flist,root,0,0);
}
void Hufftree::genCode(CharList& flist,HuffNode* node,unsigned int code,unsigned char cdsize){
    int idx=0;
    if(node){
        // 若 node 無子，將 現在紀錄的code 寫入 ftable中
        if(!(node->LChild) && !(node->RChild)){
            idx = flist.iBinSearch(node->data);
            if(idx!=0){
                flist.ftable[idx].code = code;
                flist.ftable[idx].cdsize = cdsize;
            }
            else cout << "Search failed!" <<endl;
        }
            
        code = code << 1;
        cdsize++;
        unsigned int Lcd = code;
        unsigned int Rcd = code | 1;
        genCode(flist,node->LChild,Lcd,cdsize);
        genCode(flist,node->RChild,Rcd,cdsize);   
    }
}

void Hufftree::encode(CharList& flist,Buffer& dcInput){

    // 計算 buffer 陣列 共會有 多少個「0/1」
    int bsize = 0;
    for(int i=1;i<flist.table_size;++i)
        bsize += flist.ftable[i].freq * flist.ftable[i].cdsize;

    // 配置 buffer陣列的空間
    dcInput.createBuffer(bsize);

    // 依序將「未壓縮資料 = 字元陣列」讀入，透過 BinSearch 找到對應的編碼，再依序 存入 buffer 中
    int idx=0;
    for(int i=0,j=0;i<dcInput.csize;++i){
        idx = flist.iBinSearch(dcInput.charArr[i]);
        if(idx!=0){
            saveIntBuff(flist.ftable[idx].code,flist.ftable[idx].cdsize,dcInput.buffer,j);
            // printInSize(flist.ftable[idx].code,flist.ftable[idx].cdsize);
            // cout << ",";
        }
    }
}
void Hufftree::decode(Buffer& ecInput){
    HuffNode* current = root;

    int j=0;
    // 根據 buffer陣列的 壓縮碼，依序走訪整棵Huffman樹，值到碰到Leaf，將結果存回 charArr陣列
    for(int i=0;i<ecInput.bsize;++i){

        switch (ecInput.buffer[i]) {
            case '0':
                current = current->LChild;
                break;
            case '1':
                current = current->RChild;
                break;
        }
        // 當 游標 current 走到 Leaf 時 -> 將該Leaf的字元 存入 charArr陣列
        if(!current->LChild && !current->RChild){
            ecInput.charArr[j] = current->data;
            j++;
            current = root;
        }
    }
    // cout << endl;
}

/*------- MinHeap function -------*/

ostream& operator<<(ostream& os ,MinHeap &minheap){
    os << "Heap Size:" << minheap.size;
    for(int i = 1; i <= minheap.size; ++i){
        if(!((i-1)%10)) os << "\n";
        os << "(" << minheap.heap[i]->data << "," << minheap.heap[i]->weight << ")\t";
    }
    return os;
}

void MinHeap::adjust(int rootIdx){
    HuffNode* subroot = heap[rootIdx];
    int current = rootIdx;
    int minChild = 2*current;

    while(minChild <= size){
        if(minChild < size){
            if(heap[minChild]->weight > heap[minChild+1]->weight) minChild = minChild+1;
            // 不同處1：為規範 Huffman樹 的規則 -> 權重相同，但「字元值」小，也屬於「小者」
            else if((heap[minChild]->weight == heap[minChild+1]->weight) &&
                    (heap[minChild]->data > heap[minChild+1]->data)) minChild = minChild+1;
        }

        if(subroot->weight < heap[minChild]->weight) break;
        // 不同處2：為規範 Huffman樹 的規則 -> 權重相同，但「字元值」小，也屬於「小者」
        else if((subroot->weight == heap[minChild]->weight)
                 && (subroot->data < heap[minChild]->data)) break;
        else{
            heap[current] = heap[minChild];
            current = minChild;
            minChild = 2*current;
        }
    }
    heap[current] = subroot;
}

HuffNode* MinHeap::Pop(){
    if(isEmpty()){
        cout << "Empty!" <<endl; return 0;
    }
    HuffNode* min = heap[1];
    heap[1] = heap[size];
    size--;
    adjust(1);
    return min;
}

void MinHeap::Push(HuffNode* newNode){
    if(size == 256){
        cout << "Full!" <<endl; return;
    }
    size++;
    int newNodeIdx = size;
    while(!(newNodeIdx==1) && (newNode->weight < heap[newNodeIdx/2]->weight)){
        heap[newNodeIdx] = heap[newNodeIdx/2];  //Parent 下沉
        newNodeIdx = newNodeIdx/2;
    }
    heap[newNodeIdx] = newNode;
}

void MinHeap::buildHeap(CharList& flist){
    HuffNode* newNode;
    // 將 ftable 的資料 存入 HuffNode，並插入 Heap
    for(short i=1; i < (short)flist.table_size; ++i){
        newNode = new HuffNode(flist.ftable[i].ch,flist.ftable[i].freq,0,0);
        Push(newNode);
    }
}

void MinHeap::buildHufftree(Hufftree& hufftree){
    while(size>1){
        HuffNode* first = Pop();
        HuffNode* second = Pop();
        if(first->data > second->data)
            hufftree.root = hufftree.buildTree(second,first);
        else
            hufftree.root = hufftree.buildTree(first,second);
        Push(hufftree.root);
    }
    if(size==1) hufftree.root = heap[size];
}


/*********** 讀寫檔案/編碼解碼 主要函數 ***********/

// 壓縮 步驟 1：將 原始檔案的「大小」「內容」存入 dcInput.CharArr 中
void readDecodeFile(Buffer& dcInput,Filename& path){
    cout << "Step1: Reading File..." <<endl;

    // 開啟檔案
    FILE *dptr;
    char *filepath = new char[path.orglen()];
    for(int i=0;i<path.orglen();i++) filepath[i] = path.org()[i];

    dptr = fopen(filepath,"rb");
    if(dptr == NULL){
        cout << "Open failed!" << endl;
        return;
    }

    unsigned char buff = 0;

    // 計算 未壓縮檔案 的大小(bytes)，並建立 CharArr陣列 來暫存檔案資料
    fseek(dptr,0L,SEEK_END);
    int csize = ftell(dptr);
    rewind(dptr);
    // cout << "File size: " << csize << " bytes" << endl;

    dcInput.createCharArr(csize);

    for(int i=0;i<csize;++i){
        fread(&buff,1,1,dptr);
        // printInByte(buff);
        // cout << ",";
        dcInput.charArr[i] = buff;
    }
    fclose(dptr);
}

// 壓縮 步驟 2：建立 Huffman Tree + 生成 編碼表，並根據編碼表 壓縮資料 存入 buffer
void encodeFile(Buffer& dcInput,CharList& clist,MinHeap& mhp,Hufftree& tree){
    cout << "Step2: Building Huffman Tree & Encoding..." <<endl;

    // Phase 1：建立 Huffman Tree
    clist.buildFreqTable(dcInput);
    // clist.printFtable();
    // cout << clist <<endl;
    mhp.buildHeap(clist);

    // cout << mhp <<endl;
    mhp.buildHufftree(tree);
    // cout << mhp <<endl;

    // Phase 2：Encode 壓縮
    tree.genCode(clist);
    // clist.printFtable();
    // cout << dcInput.charArr <<endl;

    tree.encode(clist,dcInput);
    // dcInput.printBuffer();
}

// 壓縮 步驟 3：將 Header資料 & 壓縮碼 寫入 檔案中
void writeEncodeFile(CharList& clist,Buffer& dcInput,Filename& path){
    cout << "Step3: Save Encoded File..." <<endl;

    // 開啟檔案
    FILE* eptr;
    char *filepath = new char[path.binlen()];
    for(int i=0;i<path.binlen();i++) filepath[i] = path.bin()[i];
    eptr = fopen(filepath,"wb+");

    // 寫入 Freq table size[2] & Freq table [(1+freq_size)*table_size]
    rewind(eptr);
    clist.writeFtable(eptr);

    // 寫入 bsize[4](原檔案壓縮後 不含標頭 的 bit 數) & csize(原始檔案 bytes 數)[4]
    dcInput.writeSize(eptr);

    // 寫入 壓縮後的資料(buffer)
    dcInput.writeEncodeBuff(eptr);
    
    fclose(eptr);
}

// 解壓 步驟 1：將 Header資料 & 壓縮碼 讀出 並存入 Charlist & Buffer 中
void readEncodeFile(CharList& clist,Buffer& ecInput,Filename& path){
    cout << "Step1: Reading File..." <<endl;

    // 開啟檔案
    FILE *eptr;
    char *filepath = new char[path.binlen()];
    for(int i=0;i<path.binlen();i++) filepath[i] = path.bin()[i];

    eptr = fopen(filepath,"rb");
    if(eptr == NULL){
        cout << "Open failed!" << endl;
        return;
    }
    
    // 從 FILE 讀進 Freq table size[2] & Freq table [(1+freq_size)*table_size]
    rewind(eptr);
    clist.readFtable(eptr);
    // clist.printFtable();

    // 從 FILE 讀進 bsize[4](原檔案壓縮後 不含標頭 的 bit 數) & csize(原始檔案 bytes 數)[4]
    ecInput.readSize(eptr);
    // cout << ecInput.bsize << "," << ecInput.csize << endl;

    // 配置 buffer & charArr 的空間
    ecInput.createBuffer(ecInput.bsize);
    ecInput.createCharArr(ecInput.csize);

    // 從 FILE 讀進 壓縮碼 存入 buffer
    ecInput.readEncodeBuff(eptr);
    // ecInput.printBuffer();

    fclose(eptr);
}
// 解壓 步驟 2：透過 頻率表(ftable) 建立 Huffman Tree + 走訪 樹 以解碼 並存入 ecInput.CharArr
void decodeFile(Buffer& ecInput,CharList& clist,MinHeap& mhp,Hufftree& tree){
    cout << "Step2: Building Huffman Tree & Decoding..." <<endl;

    // Phase 1：建立 Huffman Tree
    mhp.buildHeap(clist);

    // cout << mhp <<endl;
    mhp.buildHufftree(tree);
    // cout << mhp <<endl;

    // Phase 2：Decode 解碼
    tree.decode(ecInput);

    // ecInput.printCharArr();
    // cout << endl;
}

// 解壓 步驟 3：將 解碼的資料 寫入 檔案
void writeDecodeFile(Buffer& ecInput,Filename& path){
    cout << "Step3: Save Decoded File..." <<endl;

    // 開啟檔案
    FILE* eptr;
    char *filepath = new char[path.orglen()];
    for(int i=0;i<path.orglen();i++) filepath[i] = path.org()[i];
    eptr = fopen(filepath,"wb+");

    ecInput.writeDecodeChar(eptr);

    fclose(eptr);
}

// 印出 壓縮 資訊
void printInfo(Buffer& Input,CharList& clist){
    int noheaderSize = (Input.bsize/8 + ((Input.bsize%8)?1:0));
    int totalSize = 11 + ((1+clist.freq_size) * clist.table_size) + noheaderSize;

    double ratio = (((double)Input.csize-totalSize)/Input.csize) * 100.0;

    cout << "Original File size:\t" << Input.csize << " bytes" << endl;
    cout << "Encoded file Size (without header):\t";
    cout << noheaderSize << " bytes" <<endl;

    cout << "Encoded file Size (with header):\t";
    cout << totalSize << " bytes" <<endl;

    cout << "Compression ratio: " << ratio << "%" << endl;
}

/*********** 主程式讀入參數 ***********/
// 依據 傳入主程式的參數 決定程式功能
/*
    argv[0]: ./a.out
    argv[1]: huffman
    argv[2]: -c / -u    [*]
    argv[3]: -i         [*]
    argv[4]: infile_name
    argv[5]: -o         [*]
    argv[6]: outfile_name
    argv[argc] = NULL (引數的結尾)
*/
bool setMode(bool& mode,int& argc, char *argv[],Filename& path){
    if(argc == 7){
        path.setPath("");
        // path.setPath("/Users/jonaswu/cfile/");

        // 壓縮
        if(argv[2][1]=='c'){
            mode = true;
            if(argv[3][1]=='i') path.setOrg(argv[4]);
            else if(argv[3][1]=='o') path.setBin(argv[4]);
            if(argv[5][1]=='i') path.setOrg(argv[6]);
            else if(argv[5][1]=='o') path.setBin(argv[6]);
        }
        // 解壓縮
        else if(argv[2][1]=='u'){
            mode =false;
            if(argv[3][1]=='i') path.setBin(argv[4]);
            else if(argv[3][1]=='o') path.setOrg(argv[4]);
            if(argv[5][1]=='i') path.setBin(argv[6]);
            else if(argv[5][1]=='o') path.setOrg(argv[6]);
        }
        return true;
    }
    else{ cout << "Command isn't right!!!" << endl; return false;}
}

#endif
