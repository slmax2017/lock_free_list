#include "AtomicList.h"


CAtomicList::CAtomicList() :m_pTail(NULL),
	m_Count(0)
{
	m_pHead = new node;
	
}

CAtomicList::~CAtomicList()
{
}

void CAtomicList::push_back(void* newNode) //�ڴ��...���Ľ�
{
	node *new_node	= new node(newNode);
	node *next		= NULL;
	node* tail		= m_pTail.load();

	/*
	 *һ��ʼû������ʱ,m_pTailΪ��,��ʱֻ��Ҫ��m_pTailָָ�򱾽ڵ㼴��
	 *����������ԭ�Ӳ������̻߳����m_pTail,����ͷָ���NEXT(����push��pop��ͬһ��Ԫ���ϵĻ���),
	 *Ȼ�󷵻�,��������ԭ�Ӳ�����
	 *�߳�ֻ�ᵥ���ĸ���tail���˳�if�ṹ.��������ִ��
	*/
	if (!m_pTail.load())
	{
		if (m_pTail.compare_exchange_weak(tail, new_node))
		{
			m_pHead.load()->next = new_node;
			m_Count.fetch_add(1);
			return;
		}
	}

	/*
	*����nextָ��,��tailָ���nextΪ��ʱ,���ʾ��ǰtailΪβԪ��
	*/
	do {
		tail = m_pTail.load();
		next = NULL;
	} while (!tail->next.compare_exchange_weak(next, new_node));

	/*
	* ��m_pTailָ��δ����ʱ,do while��ѭ���ǳ������е�.��Ϊtail->next != NULL
	1.�����һ���߳�T1������while�е�CAS����ɹ��Ļ�����ô�������е� ����̵߳�CAS����ʧ�ܣ�Ȼ��ͻ���ѭ����
	2.��ʱ�����T1 �̻߳�û�и���tailָ�룬�������̼߳���ʧ�ܣ���Ϊtail->next����NULL�ˡ�
	3.ֱ��T1�̸߳�����tailָ�룬�����������߳��е�ĳ���߳̾Ϳ��Եõ��µ�tailָ�룬�����������ˡ�
	*/
	m_pTail.compare_exchange_weak(tail, new_node);

	m_Count.fetch_add(1);
}

void* CAtomicList::front()
{
	return m_pHead.load()->next.load()->data;
}

void* CAtomicList::pop_front()
{
	node *head		= m_pHead.load();
	node *popData   = head->next;
	while (!head->next.compare_exchange_weak(popData, popData->next));

	m_Count.fetch_sub(1);
	return popData->data;
}

bool CAtomicList::empty()
{
	return m_Count.load() == 0;
}

