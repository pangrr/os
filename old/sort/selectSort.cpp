#include<iostream>


void selectSort(int r[], int n)
{
    for (int i = 1; i < n; i++)
    {
        int min = i;
        
        for (int j = i; j < n; j++)
        {
            if (r[j] < r[min])
            {
               min = j;                            
            }
        }  
        
        r[0] = r[i];
        r[i] = r[min];
        r[min] = r[0];
    } 
}

int main()
{
    int r[10] = {0,5,1,3,9,2,4,7,8,6};
    
    selectSort(r, 10);
    
    for (int i = 1; i < 10; i++)
        std::cout << r[i] << " ";
        
        
    getchar();
    return 0;
}
