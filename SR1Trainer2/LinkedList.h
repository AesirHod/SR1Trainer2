#ifndef INC_LINKED_LIST_H
#define INC_LINKED_LIST_H

#include "Types.h"

template <class T>
class LinkedList;
template <class T>
class LinkedListIterator;

template <class T>
class ListNode
{
	friend class LinkedList<T>;
	friend class LinkedListIterator<T>;

public:
	ListNode(T* pxData);
	T* Data();

private:
	T* m_pxData;
	ListNode* m_pxNext;
};

template <class T>
ListNode<T>::ListNode(T* pxData)
{
	m_pxData = pxData;
	m_pxNext = NULL;
}

template <class T>
T* ListNode<T>::Data()
{
	return m_xData;
}

template <class T>
class LinkedList
{
	friend class LinkedListIterator<T>;

public:
	LinkedList();
	~LinkedList();
	void Add(T* pxData);
	void Clear();
	T* FindIndex(int i);

private:
	ListNode<T>* m_pxFirst;
	ListNode<T>* m_pxLast;
	bool IsEmpty();
};

template <class T>
LinkedList<T>::LinkedList()
{
	m_pxFirst = NULL;
	m_pxLast = NULL;
}

template <class T>
LinkedList<T>::~LinkedList()
{
	Clear();
}

template <class T>
void LinkedList<T>::Add(T* pxData)
{
	ListNode<T>* pxNew = new ListNode<T>(pxData);
	if (IsEmpty())
	{
		m_pxFirst = pxNew;
	}
	else
	{
		m_pxLast->m_pxNext = pxNew;
	}

	m_pxLast = pxNew;
}

template <class T>
void LinkedList<T>::Clear()
{
	if (!IsEmpty())
	{
		ListNode<T>* pxCurrent = m_pxFirst;
		ListNode<T>* pxTemp = NULL;

		while (pxCurrent)
		{  
			pxTemp = pxCurrent;
			pxCurrent = pxCurrent->m_pxNext;
			delete pxTemp;
		}
	}

	m_pxFirst = NULL;
	m_pxLast = NULL;
}

template <class T>
bool LinkedList<T>::IsEmpty()
{
	return (m_pxLast == NULL);
}

template <class T>
T* LinkedList<T>::FindIndex(int i)
{
	for (LinkedListIterator<CProcessDesc> xIt(*this); !xIt.Done(); xIt.Next())
	{
		if (i == 0)
		{
			return xIt.Current();
		}
				
		i--;
	}

	return NULL;
}

template <class T>
class LinkedListIterator
{
public:
	LinkedListIterator(LinkedList<T>& xList);
	T* Current();
	void Next();
	bool Done();
	T* FindIndex(int i);
private:
	LinkedList<T>& m_xList;
	ListNode<T>* m_pxCurrent;
};

template <class T>
LinkedListIterator<T>::LinkedListIterator(LinkedList<T>& xList)
 : m_xList(xList)
{
	m_pxCurrent = m_xList.m_pxFirst;
}

template <class T>
T* LinkedListIterator<T>::Current()
{
	if (m_pxCurrent)
	{
		return m_pxCurrent->m_pxData;
	}

	return NULL;
}

template <class T>
void LinkedListIterator<T>::Next()
{
	if (m_pxCurrent)
	{
		m_pxCurrent = m_pxCurrent->m_pxNext;
	}
}

template <class T>
bool LinkedListIterator<T>::Done()
{
	return (m_pxCurrent == NULL);
}

#endif