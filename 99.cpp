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