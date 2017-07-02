#pragma once

#include <atomic>
#include <windows.h>
using namespace std;


struct node
{
	node():data(NULL), next(NULL) {};
	node(void *data):data(data), next(NULL){}
	void *data;
	atomic<node*> next;
};

class CLockFreeList
{
public:
	CLockFreeList();				
	~CLockFreeList();
	void push_back(void* newNode);
	void* front();
	void* pop_front();
	bool empty();
	unsigned int size();
private:
	atomic<node*> m_pHead;
	atomic<node*> m_pTail;
	atomic<unsigned int> m_Count;
};

