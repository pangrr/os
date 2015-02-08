#include<iostream>

int Partition(int r[], int begin, int end)
{
    int i = begin, j = end;
    
    while (i < j)
    {
          while (i < j && r[i] <= r[j]) j--;
          if (i < j)
          {  
             r[0] = r[i];
             r[i] = r[j];
             r[j] = r[0];
             
             i++;    
          }  
          
          while (i < j && r[i] <= r[j]) i++;
          
          if (i < j)
          {  
             r[0] = r[i];
             r[i] = r[j];
             r[j] = r[0];
             
             j--;    
          }  
    }
    return i;
}

void quickSort(int r[], int begin, int end)
{
    if (begin < end)
    {
          int pivot =  Partition(r, begin, end);
          quickSort(r, begin, pivot-1);
          quickSort(r, pivot+1, end);            
    }
}

int main()
{
    int r[10] = {0,5,1,3,9,2,4,7,8,6};
    
    quickSort(r, 1, 9);  
    
    for (int i = 1; i < 10; i++)
        std::cout << r[i] << " ";
        
        
    getchar();
    return 0;
}
