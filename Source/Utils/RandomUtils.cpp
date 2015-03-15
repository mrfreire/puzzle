#include "RandomUtils.h"

#include <stdlib.h>

void RandomUtils::Shuffle(int* a, int count)
{
    for (int i = 1; i < count; ++i)
    {
        const int j = rand() % (i + 1);
        int aux = a[i];
        a[i] = a[j];
        a[j] = aux;
    }
}
