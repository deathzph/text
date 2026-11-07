# 第一次使用markdown
## 主要语法与使用
***
**一部分的语法会包含在vscode里**
首先，标题有六级
# 标题一
## 标题二
### 标题三
#### 标题四
##### 标题五
###### 标题六
>这是一段引用

有序列表：
1.aa
2.bb
3.cc

无序列表：
- aa
- bb
- cc

任务列表：
- [ ] 佛耶戈
- [ ] 千珏
- [x] 劫

代码快：
```c
#include<iostream>
#include<iomanip>
int main(){
    for (int i=1;i<=9;++i)
{  
        for (int j=1;j<=i;++j)
{
            std::cout<<j<<"*"<<i<<"="<<std::setw(2)<<i*j<<" ";
}
               std::cout<<std::endl;
    
        }
        return 0;
    }        

```

数学公式：

$$
\frac{\partial f}{\partial x}=2\sqrt{a}x
$$


表格：
|姓名|年龄|成绩|
|:---|---:|:---:|
|a|14|77|
|b|16|98|

脚注：
一键三连[^三连]

[^三连]:点赞投币收藏

横线：
***
哈哈哈

[B站](bilibili.com"一个视频平台")

引用链接：用于一个链接出现多次，方便更改时可以一键更改

[百度][id]

[id]:baidu.com"一个视频平台"

请参考[标题一](#标题一)

URL：
http://www.baidu.com

![warframe](/home/miku/图片/Warframe0000.png)
![百度](http://www.baidu.com/img/bd_logo1.png)

*斜体*
**加粗**
行内语法
`printf()`
<u>下划线</u>
表情
:smile:
$\theta=x²$
H~2~O
X^2^
==高亮==
