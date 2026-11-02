# include <iostream>
#include <string>
#include<sstream>
#include<vector>
using namespace std;

int main()
{
    printf("请输入一行字符串: \n");
    string line;
    if (!getline(cin,line)) return 0;

    vector<string> words;
    string w;
    istringstream iss(line);
    while (iss>> w)
        words.push_back(w);
        
        
    cout << "单词个数: " << words.size() << "\n";

    cout << "逆序输出: ";
    int n =(int)words.size();
    for(int i = n-1;i>=0;i--)
    {
        if(i!=n-1)cout<< " ";
        cout << words[i];
    }
    cout << "\n";
}