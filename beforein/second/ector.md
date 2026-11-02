# std::vector 知识整合
> C++ 标准库动态数组的完整速查笔记
> 环境：MSYS2 MinGW-w64 g++ 16.2.0 / C++17
---
## 目录
1. [是什么](#一是什么)
2. [创建方式](#二创建方式)
3. [增删改查](#三增删改查)
4. [大小 vs 容量](#四大小-vs-容量最容易混的一对)
5. [遍历的四种写法](#五遍历的四种写法)
6. [与算法的配合](#六与算法的配合最常考的组合)
7. [二维 vector](#七二维-vector)
8. [迭代器失效](#八迭代器失效进阶但重要)
9. [常见坑清单](#九常见坑清单)
10. [复杂度速查](#十复杂度速查)
11. [完整示例程序](#十一完整示例程序)
12. [一页速记](#十二一页速记)
---
## 一、是什么
**`vector` 是 C++ 标准库的动态数组**：能自动变长变短的数组，元素在内存中**连续存放**。
```cpp
#include <vector>
vector<int> v = {5, 3, 9};
```
```
内存： [ 5 ][ 3 ][ 9 ]
下标：   0    1    2
```
两个关键性质决定了它几乎所有行为：
| 性质 | 带来的好处 | 带来的代价 |
| --- | --- | --- |
| **内存连续** | 随机访问 O(1)、缓存友好、能直接对接 `sort` 等算法 | 中间插入/删除要搬移元素，O(n) |
| **自动扩容** | 不用事先知道大小 | 扩容时要重新分配并搬运，迭代器会失效 |
---
## 二、创建方式
```cpp
#include <vector>
vector<int> v;                        // 空
vector<int> v(n);                     // n 个元素，初值 0
vector<int> v(n, x);                  // n 个元素，初值都是 x
vector<int> v{1, 2, 3};               // 初始化列表
vector<int> v = {1, 2, 3};            // 同上
vector<int> v2(v);                    // 拷贝另一个 vector
vector<int> v3(v.begin(), v.end());   // 用区间构造
// 二维
vector<vector<int>> g(n, vector<int>(m, 0));   // n 行 m 列，全 0
```
> ⚠️ **不是 `vector<int> v[n]`**，那是 C 风格数组，不是 vector。
> 二维一定写成 `vector<vector<int>>`。
### 三种"读入数据"的写法
```cpp
// 写法 1：已知个数 n（常用）
vector<int> v(n);
for (int i = 0; i < n; ++i) cin >> v[i];
// 或：for (int& x : v) cin >> x;     必须加 &
// 写法 2：不知道个数，边读边存
vector<int> v;
int x;
while (cin >> x) v.push_back(x);
// 写法 3：迭代器一次读入（少用）
vector<int> v((istream_iterator<int>(cin)), istream_iterator<int>());
```
---
## 三、增删改查
| 操作 | 写法 | 复杂度 | 说明 |
| --- | --- | --- | --- |
| 尾加 | `v.push_back(x)` | 均摊 O(1) | 最常用 |
| 尾加（省拷贝） | `v.emplace_back(args...)` | 均摊 O(1) | 原地构造，比 push_back 少一次拷贝 |
| 尾删 | `v.pop_back()` | O(1) | 空容器调用是 UB |
| 随机访问 | `v[i]` | O(1) | **不检查越界** |
| 安全访问 | `v.at(i)` | O(1) | 越界抛 `out_of_range` |
| 首元素 | `v.front()` | O(1) | 空容器调用是 UB |
| 尾元素 | `v.back()` | O(1) | 空容器调用是 UB |
| 插入 | `v.insert(v.begin() + i, x)` | O(n) | 后面元素全体后移 |
| 删除 | `v.erase(v.begin() + i)` | O(n) | 后面元素全体前移 |
| 区间删除 | `v.erase(first, last)` | O(n) | 左闭右开 |
| 清空 | `v.clear()` | O(n) | 只清元素，**capacity 不释放** |
---
## 四、大小 vs 容量（最容易混的一对）
```cpp
vector<int> v;        // size=0, capacity=0
v.push_back(1);       // size=1, capacity=1
v.push_back(2);       // size=2, capacity=2
v.push_back(3);       // size=3, capacity=4  ← 扩容了
```
| 函数 | 含义 |
| --- | --- |
| `v.size()` | **现在有几个元素** |
| `v.capacity()` | **不重新分配内存的情况下最多能放几个** |
| `v.empty()` | 是否为空，等价 `size() == 0` |
| `v.resize(n)` | 改**元素个数**：变多则补 0，变少则截断 |
| `v.reserve(n)` | 只改**容量**，`size()` 不变，元素一个都没创建 |
| `v.shrink_to_fit()` | 请求把多余容量还回去（不保证生效） |
### 扩容机制
容量不够时，申请一块更大的内存（通常是原来的 1.5～2 倍），把所有元素搬过去，释放旧内存。
所以连续 `push_back` 的**均摊**复杂度是 O(1)。
### reserve 与 resize 的三句话
1. `reserve` 只借空间，元素还不存在，**不能下标访问**；
2. `resize` 真的创建了元素，**可以下标访问**；
3. 已知要放 n 个元素时，先 `reserve(n)` 能避免反复扩容搬移。
```cpp
vector<int> v;
v.reserve(10);
// v[0] = 5;    ❌ 越界！size 仍是 0
v.push_back(5); // ✅ 只能这样加
```
---
## 五、遍历的四种写法
```cpp
// 1. 下标（能改，能拿到索引）
for (int i = 0; i < (int)v.size(); ++i) cout << v[i];
// 2. 范围 for 读取（加 const& 避免每个元素都拷贝一次）
for (const int& x : v) cout << x;
// 3. 范围 for 修改（必须加 &）
for (int& x : v) x *= 2;
// 4. 迭代器（需要在遍历中删除元素时用）
for (auto it = v.begin(); it != v.end(); ++it) cout << *it;
```
> ⚠️ `for (int x : v) x *= 2;` 改的是副本，**原 vector 一点没变**。
---
## 六、与算法的配合（最常考的组合）
```cpp
#include <algorithm>
#include <numeric>
sort(v.begin(), v.end());                      // 升序，O(n log n)
sort(v.begin(), v.end(), greater<int>());      // 降序
sort(v.begin(), v.end(), [](int a, int b) {    // 自定义规则
    return a > b;
});
stable_sort(v.begin(), v.end());               // 稳定排序（相等元素保持原序）
reverse(v.begin(), v.end());                   // 反转
find(v.begin(), v.end(), x) != v.end();        // 查找（线性）
count(v.begin(), v.end(), x);                  // 计数
max_element(v.begin(), v.end());               // 最大值的位置（迭代器）
min_element(v.begin(), v.end());               // 最小值的位置
accumulate(v.begin(), v.end(), 0);             // 求和（<numeric>）
// 有序区间上的二分查找
binary_search(v.begin(), v.end(), x);          // 是否存在
lower_bound(v.begin(), v.end(), x);            // 第一个 >= x 的位置
upper_bound(v.begin(), v.end(), x);            // 第一个 > x 的位置
```
### 排序 + 去重（标准三步）
```cpp
sort(v.begin(), v.end());                          // 1. 排序
v.erase(unique(v.begin(), v.end()), v.end());      // 2. 压缩 + 3. 截断
```
**原理**：
| 步骤 | 做什么 | 关键点 |
| --- | --- | --- |
| `sort` | 让相等元素**相邻** | 排序本身也是题目要求 |
| `unique` | 把不重复的元素挪到前面 | **不删元素**，返回新的逻辑末尾；`size()` 不变 |
| `erase` | 把尾部残留真正切掉 | 只有这一步真正改变长度 |
以 `1 1 2 3 3 3 5` 为例，`unique` 内部（`w` 写指针，`r` 读指针）：
| r | v[r] | 与 v[w-1] 比 | 动作 | w | 数组状态 |
| --- | --- | --- | --- | --- | --- |
| 0 | 1 | w=0 无前驱 | 写入 | 1 | `1 1 2 3 3 3 5` |
| 1 | 1 | 1 == 1 | 跳过 | 1 | 不变 |
| 2 | 2 | 1 != 2 | 写入 v[1]=2 | 2 | `1 2 2 3 3 3 5` |
| 3 | 3 | 2 != 3 | 写入 v[2]=3 | 3 | `1 2 3 3 3 3 5` |
| 4 | 3 | 3 == 3 | 跳过 | 3 | 不变 |
| 5 | 3 | 3 == 3 | 跳过 | 3 | 不变 |
| 6 | 5 | 3 != 5 | 写入 v[3]=5 | 4 | `1 2 3 5 3 3 5` |
`unique` 返回 `begin + 4`，前 4 个是答案，后面 `3 3 5` 是残留，`erase` 一截即得 `1 2 3 5`。
> ⚠️ `unique` 只合并**相邻**重复，所以**必须先排序**，顺序不能反。
### 自己实现去重（双指针原地压缩）
```cpp
size_t w = 0;                                  // 写指针
for (size_t r = 0; r < v.size(); ++r)          // 读指针
    if (w == 0 || v[w - 1] != v[r])            // 与"已保留的最后一个"比较
        v[w++] = v[r];
v.resize(w);                                   // 截断，等价于 erase 尾部
```
### 三种去重路线对比
| 路线 | 做法 | 时间 | 额外空间 | 结果有序 | 适用 |
| --- | --- | --- | --- | --- | --- |
| A 排序 + unique + erase | 排序后相邻去重 | O(n log n) | **O(1)** | 是 | 要求排序+去重 |
| B `set` | 自动有序去重容器 | O(n log n)，常数大 | O(n) | 是 | 代码优先、数据小 |
| C `unordered_set` | 哈希记录 + 新数组 | O(n) 期望 | O(n) | **保持原序** | 只去重、不许改顺序 |
| D 计数数组 | 值域小则开桶标记 | O(n + 值域) | O(值域) | 是 | 值域很小 |
```cpp
// 路线 B：set
set<int> s(v.begin(), v.end());
for (int x : s) cout << x << ' ';
// 路线 C：保序去重
unordered_set<int> seen;
vector<int> out;
for (int x : v)
    if (seen.insert(x).second)   // 插入成功 = 第一次见
        out.push_back(x);
```
---
## 七、二维 vector
```cpp
vector<vector<int>> g(n, vector<int>(m, 0));   // n 行 m 列，全 0
g[i][j] = 5;                    // 访问
g.size();                       // 行数
g[i].size();                    // 第 i 行的列数
g.push_back(vector<int>(m, 0)); // 追加一行
// 遍历
for (int i = 0; i < (int)g.size(); ++i)
    for (int j = 0; j < (int)g[i].size(); ++j)
        cout << g[i][j] << ' ';
// 范围 for 遍历
for (const auto& row : g) {
    for (int x : row) cout << x << ' ';
    cout << '\n';
}
```
每一行长度可以不同（锯齿数组），所以 `g[i].size()` 要逐行取，不能用统一的 `m`。
---
## 八、迭代器失效（进阶但重要）
| 操作 | 后果 |
| --- | --- |
| `push_back` 触发扩容 | **所有**迭代器、指针、引用失效 |
| `insert` / `erase` | 该位置**及之后**的全部失效 |
| `reserve` / `resize` 变大 | 触发重新分配，全部失效 |
遍历中删除元素的**错误**与**正确**写法：
```cpp
// ❌ 错误：it 已失效，++it 是 UB
for (auto it = v.begin(); it != v.end(); ++it)
    if (*it == 0) v.erase(it);
// ✅ 正确：用 erase 的返回值续上
for (auto it = v.begin(); it != v.end(); )
    if (*it == 0) it = v.erase(it);
    else ++it;
// ✅ 也可以用 remove-erase 惯用法（不用手写循环）
v.erase(remove(v.begin(), v.end(), 0), v.end());
```
---
## 九、常见坑清单
1. **空 vector 用下标**：`vector<int> v; v[0] = 1;` 越界，UB。
2. **`reserve` 后下标写**：容量有了但 size 还是 0，照样越界。
3. **范围 for 不加 `&`**：改的是副本，原数据没动。
4. **`size()` 返回无符号**：`for (int i = 0; i < v.size(); ++i)` 在 `-Wall` 下报警；写 `(int)v.size()` 或用 `size_t`。
5. **`erase` 后还用旧的 `n` 遍历**：长度已变，越界。应改用 `v.size()`。
6. **`clear()` 不释放内存**：要真释放得 `v.clear(); v.shrink_to_fit();`。
7. **函数传参不加 `&`**：`void f(vector<int> v)` 每次整体拷贝，大数据极慢，应写 `void f(const vector<int>& v)`。
8. **`vector<bool>` 是特化版本**：按位存储，`&v[i]` 不合法，行为与其他 vector 不一致，慎用。
9. **`unique` 不删元素**：只调 `unique` 不 `erase`，尾部是垃圾数据。
10. **不排序就 `unique`**：只去掉原本相邻的重复，结果错误。
11. **负数传给 `vector<int> v(n)`**：会转成巨大的无符号数，抛 `length_error`。
---
## 十、复杂度速查
| 操作 | 复杂度 |
| --- | --- |
| `v[i]` / `front()` / `back()` | O(1) |
| `push_back` / `pop_back` | 均摊 O(1) |
| `insert` / `erase`（中间） | O(n) |
| `size()` / `empty()` / `reserve()` | O(1) |
| `find` / `count` | O(n) |
| `sort` | O(n log n) |
| 遍历一遍 | O(n) |
---
## 十一、完整示例程序
**题目**：输入 n 个整数，用 vector 存储；先输出排序结果，再输出去重结果。
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main() {
    int n;
    if (!(cin >> n)) return 0;            // 没有输入就结束
    vector<int> v(n);                     // 创建 n 个 int，初值都是 0
    for (int i = 0; i < n; ++i)
        cin >> v[i];                      // 用下标赋值
    // 1. 排序后输出
    sort(v.begin(), v.end());
    cout << "排序: ";
    for (int i = 0; i < n; ++i) {         // 此时 size() == n
        if (i) cout << ' ';
        cout << v[i];
    }
    cout << '\n';
    // 2. 去重后输出
    // 已排序，unique 把不重复的元素依次搬到前面，返回"新末尾"，
    // 再 erase 把后面的重复元素真正删掉。
    v.erase(unique(v.begin(), v.end()), v.end());
    cout << "去重: ";
    for (size_t i = 0; i < v.size(); ++i) {   // 长度已变，必须用 size()
        if (i) cout << ' ';
        cout << v[i];
    }
    cout << '\n';
    return 0;
}
```
### 运行结果
输入：
```
8
5 3 9 3 1 5 9 2
```
输出：
```
排序: 1 2 3 3 5 5 9 9
去重: 1 2 3 5 9
```
### 边界测试
| 输入 | 输出 | 说明 |
| --- | --- | --- |
| `0` | `排序:` / `去重:` | n=0，循环不进入 |
| `1` + `7` | `7` / `7` | 单元素 |
| `5` + `2 2 2 2 2` | `2 2 2 2 2` / `2` | 全重复 |
| `4` + `-3 0 -3 10` | `-3 -3 0 10` / `-3 0 10` | 负数正常 |
| 空输入 | 无输出 | 第 9 行拦截 |
### 编译命令（g++）
```powershell
# 日常
g++ -std=c++17 -O2 -Wall -Wextra -o sort_unique sort_unique.cpp
# 对外分发（不依赖 MinGW 的 DLL）
g++ -std=c++17 -O2 -Wall -Wextra -static -o sort_unique sort_unique.cpp
# 运行（PowerShell 需加 .\）
.\sort_unique.exe
# 喂入输入文件（PowerShell 不支持 <，用管道）
Get-Content in.txt | .\sort_unique.exe
```
| 参数 | 作用 |
| --- | --- |
| `-std=c++17` | 指定语言标准 |
| `-O2` | 二级优化 |
| `-Wall -Wextra` | 打开常用警告 |
| `-o 名字` | 指定输出文件名 |
| `-g` | 生成调试信息 |
| `-static` | 静态链接，脱离 MinGW DLL |
---
## 十二、一页速记
```
一个动态数组，连续内存，能下标 O(1) 访问
创建：v、v(n)、v(n,x)、v{a,b,c}、v(begin,end)
      二维：vector<vector<int>> g(n, vector<int>(m, 0))
增删：push_back / emplace_back / pop_back / insert / erase / clear
访问：v[i] 不检查越界，v.at(i) 会检查，front / back
大小：size() 元素数    capacity() 容量
      reserve 只借空间（不能下标访问）
      resize  真建元素（可以下标访问）
遍历：下标 / 范围 for（要改就加 &）/ 迭代器
      删元素用 it = v.erase(it)
配算法：sort 排序
        unique + erase 去重（必须先排序）
        三步：sort → erase(unique(...), end) → 完成
复杂度：随机访问 O(1)、尾插均摊 O(1)、中间插删 O(n)、排序 O(n log n)
大坑：越界不报错、迭代器失效、范围 for 忘加 &、erase 后还用旧 n
```
---
*文档结束。建议配合 `sort_unique.cpp` 一起看，边改边跑。*