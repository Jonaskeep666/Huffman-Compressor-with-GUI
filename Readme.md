# Huffman Compressor with GUI

This is a simple implementation of a Huffman encoding compressor and decompressor with a GUI implemented in Qt. It's my first attempt at combining C++ source code into a user-friendly graphic interface.

----

## Encoding

![Example](https://github.com/Jonaskeep666/Huffman-Compressor-with-GUI/blob/master/CompressExample.png)

1>	 Step 1：Create Huffman Tree
- Create Freq Table
- Create Min Heap
- Create Binary Tree = Huffman Tree

2>	 Step 2：Encoding 
- Create Encoding Table (Code book)
	- Traverse Huffman tree from root
- Encoding original data in Huffman code

3>	 Step 3：Write back
- Write the header info. & compressed data into file


## Decoding

![Example](https://github.com/Jonaskeep666/Huffman-Compressor-with-GUI/blob/master/decompressExample.png)

1>	 Step 1：Read file
- Read the header info. 
	- Read Freq Table (ftable)
	- Read bsize、csize
- Read compressed data into buffer array

2>	 Step 2：Create Huffman Tree
- Create Min Heap
- Create Binary Tree = Huffman Tree

3>	 Step 3：Decoding 
- Traverse Huffman tree from root

4>	 Step 4：Write back
- Write the decompressed data into file

----

### My Thought

1. 背景：採用 Qt 6.4 實作，安裝環境花費一天、學習+完成 花費兩天
2. 遇到的困難
	- 寫程式的邏輯不同：
		- 傳統C程式 以 工作流程(順序) 為核心
		- GUI程式 以 觸發/事件 為核心，所以 流程的控制 圍繞在 哪些信號被觸發
	- 高等程式語言 許多功能都被 打包好 實現，不太需要自己撰寫功能函式
		- 但缺點是需要查詢手冊 才知道對應的功能 被放在哪些現有的函數中
	- 打包成 GUI 會面對許多 額外的問題：加入 icon、互動邏輯等
	- 即使 採用 相同 語言，不同的IDE 對於 型別轉換、資料賦值 等操作，都會有些許不同
		- 平常寫程式，對於 資料的型別掌握、設定 都要仔細

3. 關鍵：遇到瓶頸 -> 慢跑 30分鐘 + 睡飽 -> 隔天會有新發現


