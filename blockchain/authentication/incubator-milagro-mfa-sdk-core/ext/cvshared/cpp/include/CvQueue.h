/* 
 * File:   CvQueue.h
 * Author: mony
 *
 * Created on September 26, 2012, 5:29 PM
 */

#ifndef CVQUEUE_H
#define	CVQUEUE_H

#include "CvSemaphore.h"
#include "CvMutex.h"

#include "CvLogger.h"

#include <string>
#include <list>

namespace CvShared
{
	
////////////////////////////////////////////////////////////////////
//	CvQueue<T>
/// @brief	This templates defines a waiting queue class for elements of type T.
///	The waiting queue is thread-safe and various threads can push
///	elements into its tail.
///	Threads that desire to read an element from the queue's head will
///	be blocked if the queue is empty and when a new element is pushed,
///	the reader will be unblocked.
////////////////////////////////////////////////////////////////////
template <class T>
class CvQueue
{
public:
	CvQueue( const char* apName  = "" );
	~CvQueue();

	static const TimeValue_t TIMEOUT_INFINITE = -1;
	static const TimeValue_t TIMEOUT_NO_WAIT = 0;
	
	void	Push( const T& element );
	void	PushFront( const T& element );	
	bool	Pop( T& element, const Millisecs& aTimeout = TIMEOUT_INFINITE );
	int		Size();

private:
	typedef std::list<T>	CList;
	
	String			m_name;			///< The queue name (recommended to be unique)
	CList			m_list;			///< Holds the queue elements
	CvSemaphore		m_semaphore;	///< Represents the queue elements count and provides the blocking mechanism
	CvMutex			m_mutex;		///< Syncronizes multi-threaded access
};

template <class T>
CvQueue<T>::CvQueue( const char* apName ) :
	m_name(apName),
	m_semaphore( (String("sema4-") + apName).c_str() ),
	m_mutex( (String("mutex-") + apName).c_str() )
{
	if ( !m_semaphore.Create(0) )
	{
		LogMessage( enLogLevel_Error, "ERROR: Failed to create semaphore for queue [%s]", m_name.c_str() );
	}

	if ( !m_mutex.Create() )
	{
		LogMessage( enLogLevel_Error, "ERROR: Failed to create mutex for queue [%s]", m_name.c_str() );
	}
}

template <class T>
CvQueue<T>::~CvQueue()
{}

template <class T>
void CvQueue<T>::Push(const T& element)
{
	CvMutexLock lock(m_mutex);

	m_list.push_back(element);
	m_semaphore.Post();
}

template <class T>
void CvQueue<T>::PushFront(const T& element)
{
	CvMutexLock lock(m_mutex);

	m_list.push_front(element);
	m_semaphore.Post();
}

template <class T>
bool CvQueue<T>::Pop( T& element, const Millisecs& aTimeout )
{
	if ( !m_semaphore.Pend( aTimeout ) )
		return false;

	CvMutexLock lock(m_mutex);

	if ( m_list.empty() )
	{
		LogMessage( enLogLevel_Warning, "ERROR: Queue [%s] is empty, while it shouldn't be", m_name.c_str() );
		return false;
	}

	element = m_list.front();
	m_list.pop_front();

	return true;
}

template <class T>
int CvQueue<T>::Size()
{
	CvMutexLock lock(m_mutex);
	int size = m_list.size();
	return size;
}

}	// namespace CvShared

#endif	/* CVQUEUE_H */

