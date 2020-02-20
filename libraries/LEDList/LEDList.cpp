#include "LEDList.h"
#include "Arduino.h"
using namespace std;

template <class T>
LEDList<T>::LEDList()
{
	array[10];
	size = 10;
	count = 0;
}

template <class T>
void LEDList<T>::add(T a)
{
	int x = size;
	if(count >= size - 1)
	{
		T newArray[x*2];
		for(int i = 0; i < size; i++)
		{
			newArray[i] = array[i];
		}
		*array = newArray;
	}
	size = x * 2;
	array[count] = a;
	count++;
}

template <class T>
T LEDList<T>::get(int i)
{
	return array[i];
}

template <class T>
void LEDList<T>::remove(int i)
{
	for(int j = i; j < size - 1; j++)
	{
		array[j] = array[j + 1];
	}
	count--;
}

template <class T>
T LEDList<T>::operator[](int i)
{
	return array[i];
}
























