#include <iostream>
#include<iomanip>

int main(){
    int i=1;
    while(i<=9)
    {
        int j=1;
        while (j<=i){
            std::cout<<j<<"*"<<i<<"="<<std::setw(2)<<i*j<<" ";
            ++j;
        }
            std::cout<<std::endl;
            ++i;
}
    return 0;
}    