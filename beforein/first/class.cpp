#include <iostream>
using namespace std;

class MyClass {
public:
    // 查找数组最小值，并与末尾元素交换
    void fun(int arr[], int n) {
        if (n <= 0) return;

        int minIndex = 0;
        for (int i = 1; i < n; i++) {
            if (arr[i] < arr[minIndex]) {
                minIndex = i;
            }
        }

        // 将最小值与末尾元素交换
        int temp = arr[minIndex];
        arr[minIndex] = arr[n - 1];
        arr[n - 1] = temp;
    }
};

int main() {
    int n;
    cout << "输入数组长度: ";
    cin >> n;

    if (n <= 0) {
        cout << "数组长度必须大于 0" << endl;
        return 1;
    }

    int* arr = new int[n];
    cout << "输入 " << n << " 个数组元素: ";
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    // 打印交换前的数组
    cout << "交换前数组: ";
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    // 实例化类对象并调用成员函数
    MyClass obj;
    obj.fun(arr, n);

    // 打印交换后的数组
    cout << "交换后数组: ";
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    delete[] arr;
    return 0;
}