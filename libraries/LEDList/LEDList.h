/*
	LEDList.h - Library for Array-Based List
	Created by Blake Wewer, April 18, 2018.
	Private
*/

#ifndef LEDLIST_H
#define LEDLIST_H

#include "Arduino.h"


template <class T>
class LEDList<T> {
	private: 
		
	public: 
		LEDList();
		T array[];
		int size;
		int count;
		void add(T);
		T get(int);
		void remove(int);
		T operator[](int);
};


#endif