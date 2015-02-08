#include<iostream>


void shellSort(int r[], int n)
{
     for (int d = n/2; d > 0; d = d/2)
     {
        for (int i = d+1; i < n; i++)
        {
            r[0] = r[i];
            
            int j;
            for (j = i-d; r[0] < r[j] && j > 0; j-=d)
            {
                r[j+d] = r[j];
            }
            r[j+d] = r[0];       
        } 
     }
}

int main()
{
    int r[10] = {0,5,1,3,9,2,4,7,8,6};
    
    shellSort(r, 10);
    
    for (int i = 1; i < 10; i++)
        std::cout << r[i] << " ";
        
        
    getchar();
    return 0;
}
