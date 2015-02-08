#include<iostream>


void insertSort(int r[], int n)
{
     for (int i = 2; i < n; i++)
     {
         r[0] = r[i];
         
         int j;
         for (j = i-1; j > 0 && r[0] < r[j]; j--)
         {
            r[j+1] = r[j];     
         }
               
         r[j+1] = r[0];
     }     
}

int main()
{
    int r[10] = {0,5,1,3,9,2,4,7,8,6};
    
    insertSort(r, 10);
    
    for (int i = 1; i < 10; i++)
        std::cout << r[i] << " ";
        
        
    getchar();
    return 0;
}
