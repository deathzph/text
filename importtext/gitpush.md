# git使用方法（已初始化仓库后）
## 日常代码推送到github仓库
```bash
git add . #将当前目录下代码添加到暂存区
git commit -m ¨Initial commit¨ #双引号内为本次提交的描述内容
git push #将代码推送到github
```

## 克隆仓库
```
cd ~ #回到家目录
cd xxx #xxx为要将仓库克隆到的文件夹
git clone git@github.com:deathzph/try.git
```

## 初始化仓库
```
cd xxxx #xxxx即为要初始化的仓库
git init #在目录中创建一个隐藏的.git文件夹
git add . #添加当前目录下所有文件到暂存区，如果想添加特定文件，可以用个git add xxx
git commit -m ¨xxx¨ #引号内是本次提交的描述
```
github上新建仓库
*注意不要勾选initialize this repository with a README*
```
git remote add origin xxxxxxx  #xxxxx即为刚建的仓库地址
```

```
git config --global init.defaultBranch main #新建仓库时消除master分支警告
```
```
git branch -M main #将当前仓库统一为main
```

## 查看仓库
1.
```
git status
```
- 红色字体：表示文件是未跟踪（Untracked）或已修改（Modified）的。比如你截图里的 99.cpp 等文件，如果现在敲这个命令，它们都会显示为红色。
- 绿色字体：表示文件已经暂存（Changes to be committed），也就是你执行了 git add . 之后的文件。
- 如果显示 nothing to commit, working tree clean，说明所有文件都提交完毕了，很干净。

2.
```
git ls-files #查看已经被git管理的文件列表
```

3.
```
git log #查看历史提交记录
git log --oneline #一行一个的简要查看
```

4.
```
git branch #查看当前本地有哪些分支，带*即为当前所在的分支
```