#ifndef TRACKERPOINTER_H
#define TRACKERPOINTER_H

//n'en allouer qu'avec new, pas de new[] ou d'instanciation directe les enfants
template<class T>
class TrackerPointer
{
	public:
		TrackerPointer(T* t, bool del);

		T* getPointer(void);
		int getNumRef(void);
		void grab(void);
		void release(void);
	private:
		T* t;
		int numRef;
		bool del;
};

template<class T>
TrackerPointer<T>::TrackerPointer(T* t, bool del):
	t(t),
	del(del),
	numRef(0)
{
	grab();
}

template<class T>
T* TrackerPointer<T>::getPointer(void)
{
	return t;
}

template<class T>
int TrackerPointer<T>::getNumRef(void)
{
	return numRef;
}

template<class T>
void TrackerPointer<T>::grab(void)
{
	numRef++;
}

template<class T>
void TrackerPointer<T>::release(void)
{
	numRef--;
	if(numRef<=0)
	{
		if(del)delete t;
		delete this;
	}
}

#endif
