# include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main(){
    int a;
    printf("请输入数字总数：\n");
    cin >> a;
    vector <int> v(a);
     printf("请输入数字：\n");
    for (int i=0; i<a;i++)
    cin >> v[i];

    sort(v.begin(),v.end());
    cout << "排序：" ;
    for (int i=0; i<a;i++)
    {
    if (i) cout << " ";
    cout << v[i];
    }
    cout << endl;

    v.erase(unique(v.begin(),v.end()),v.end());
    cout << "去重：" ;
    for (int i=0; i<v.size();i++)
    {
        if (i) cout << " ";
        cout << v[i];
    }
    cout<< endl;
    return 0;
}