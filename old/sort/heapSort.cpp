#include <iostream>

void sift(int r[], int begin, int end)
{
    int i = begin, j = begin * 2;
    
    while (j <= end) {
        if (j+1 <= end && r[j] < r[j+1]) {
            j++;
        }
        
        if (r[i] < r[j]) {
            r[0] = r[i];
            r[i] = r[j];
            r[j] = r[0];
            
            i = j;
            j = i*2;
        }
        else
            break;
      }
}

void initialHeap(int r[], int end)
{
    void sift(int r[], int begin, int end);
    
    for (int i = end/2; i > 0; i--) {
        sift(r, i, end);
    }
}

void heapSort(int r[], int end)
{
    void initialHeap(int r[], int end);
    void sift(int r[], int begin, int end);

    
    initialHeap(r, end);
    
    for (int i = end; i > 1; i--) {
        r[0] = r[1];
        r[1] = r[i];
        r[i] = r[0];
        
        sift(r, 1, i-1);
    }
}

int main(int argc, const char * argv[])
{
    int r[10] = {0,5,1,3,9,2,4,7,8,6};
    
    heapSort(r, 9);
    
    for (int i = 1; i < 10; i++)
        std::cout << r[i] << " ";
    
    getchar();
    return 0;
}