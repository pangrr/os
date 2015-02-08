#include<iostream>


void bubbletSort(int r[], int n)
{
     int exchange = n, bound;
     
     while (exchange > 0)
     {
           bound = exchange;
           exchange = 0;
           for (int i = 1; i < bound; i++)
           {
               if (r[i] > r[i+1])
               {
                     r[0] = r[i];
                     r[i] = r[i+1];
                     r[i+1] = r[0];
                     exchange = i;               
               }              
           }           
     }       
}

int main()
{
    int r[10] = {0,5,1,3,9,2,4,7,8,6};
    
    bubbletSort(r, 10);
    
    for (int i = 1; i < 10; i++)
        std::cout << r[i] << " ";
        
        
    getchar();
    return 0;
}
