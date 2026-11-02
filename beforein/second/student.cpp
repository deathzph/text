#include <iostream>
#include <string>
using namespace std;

class Student{
private:
    string name;
    string id;
    int* scores;
    int numScores;
    
public:
    Student(const string& n, const string& i,const int* sc, int   num)
    : name(n), id(i), scores(nullptr), numScores(num >0 ? num : 0){
        if(numScores > 0){
            scores = new int [numScores];
            for (int k =0; k < numScores; k++)
                scores[k] = sc ? sc[k] : 0;
        }
    }
Student(const Student& other)
    : name(other.name), id(other.id),scores(nullptr),numScores(other.numScores){
        if(numScores > 0){
            scores = new int[numScores];
            for(int k=0; k < numScores; k++)
                scores[k] = other.scores[k];
        }
    }
Student& operator=(const Student& other){
    if(this != &other){
        int* tmp=(other.numScores > 0) ? new int[other.numScores]: nullptr;
        for(int k=0; k < other.numScores; k++)
            tmp[k] = other.scores[k];
        delete[] scores;
        scores = tmp;
        numScores = other.numScores;
        name = other.name;
        id =other.id;
    }
    return *this;
    }
~Student(){
    delete[] scores;
    scores = nullptr;
    }

void print() const{
    cout << "姓名: " << name << ", 学号: " << id << ", 成绩: ";
    for (int k=0; k < numScores; k++){
        if (k) cout << " ";
        cout << scores[k];
    }
    cout << endl;
    }

void setScore(int idx,int v){
    if (idx >= 0 && idx < numScores) scores[idx] = v;
}
const int* data()const{return scores;}
};

int main(){
    int s[3] = {85,90,78};
    Student stu1("norden", "2026001",s, 3);
    cout << "stu1创建后: " << endl;
    stu1.print();
    cout << endl;

    Student stu2(stu1);
    cout << "用stu1拷贝创建stu2后: " << endl;
    stu1.print();
    stu2.print();
    cout << "scores地址:stu1=" <<  static_cast<const void*>(stu1.data()) 
    << ", stu2=" << static_cast<const void*>(stu2.data()) 
    << "不同->深拷贝生效 " << endl <<endl;
    
    stu1.setScore(0, 60);
    cout << "stu1第一门成绩修改后: " << endl;
    stu1.print();
    stu2.print();
    cout << "  结论： stu2仍是原来的成绩，未被stu1影响，说明stu2是深拷贝的" << endl << endl;
}

