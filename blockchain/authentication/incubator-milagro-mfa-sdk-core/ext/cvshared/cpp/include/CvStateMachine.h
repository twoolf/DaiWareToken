/* 
 * File:   CvStateMachine.h
 * Author: mony
 *
 * Created on September 21, 2012, 4:54 PM
 */

#ifndef CVSTATEMACHINE_H
#define	CVSTATEMACHINE_H

#include "CvMutex.h"
#include "CvString.h"

#include <map>
#include <list>

namespace CvShared
{

class CvSmEventHandler
{
public:
	typedef bool (CvSmEventHandler::*Action_t)( void* );
	typedef bool (CvSmEventHandler::*Condition_t)( void* ) const;
};

class CvSmTransition;

class CvStateMachine
{
public:
	typedef int		State_t;
	typedef int		Event_t;

	static const State_t INVALID_STATE = -1;
	
	CvStateMachine( const char* apName = "(unnamed)" );
	virtual ~CvStateMachine();
	
	void	AddState( State_t aState, const CvString& aStateName )	{ CvMutexLock lock(m_mutex); m_mapStates[aState] = aStateName; }
	void	AddEvent( Event_t aEvent, const CvString& aEventName )	{ CvMutexLock lock(m_mutex); m_mapEvents[aEvent] = aEventName; }
	bool	AddTransition( State_t aState, Event_t aEvent, const CvSmTransition& aTransition );
	
	bool	Start( State_t aInitialState );
	
	bool	Signal( Event_t aEvent, void* apData = NULL );
	
private:
	CvStateMachine(const CvStateMachine& orig)	{}
	
	typedef std::map<State_t,CvString>					CMapStates;
	typedef std::map<Event_t,CvString>					CMapEvents;
	typedef std::pair<State_t, Event_t>					CTransitionKey;
	typedef std::map<CTransitionKey, CvSmTransition*>	CMapTransitions;	
	
	CTransitionKey	MakeTransitionKey( State_t aState, Event_t aEvent ) const	{ return std::make_pair( aState, aEvent ); }
	
	CvString		m_name;
	State_t			m_state;
	
	CMapStates		m_mapStates;
	CMapEvents		m_mapEvents;
	CMapTransitions	m_mapTransitions;
	
	CvMutex			m_mutex;
};

class CvSmTransition
{
	friend class CvStateMachine;
	
public:
	CvSmTransition()	{}
	virtual ~CvSmTransition()	{}
	
protected:
	CvSmTransition( const CvSmTransition& aOther )	{}

	virtual CvSmTransition*	Duplicate() const = 0;
	virtual bool			Execute( void* apData, OUT CvStateMachine::State_t& aNewState ) = 0;	
};

class CvSmTransitionSimple : public CvSmTransition
{
	friend class CvSmTransitionConditional;
	
public:
	CvSmTransitionSimple( CvSmEventHandler* apHandler ) :
		m_pHandler(apHandler),
		m_newState(CvStateMachine::INVALID_STATE),
		m_errorState(CvStateMachine::INVALID_STATE),
		m_errorAction(NULL)
	{}
		
	virtual ~CvSmTransitionSimple()	{}
	
	void	AppendAction( CvSmEventHandler::Action_t aAction )			{ m_listActions.push_back(aAction); }
	void	SetNewState( CvStateMachine::State_t aNewState )			{ m_newState = aNewState; }
	void	SetErrorState( CvStateMachine::State_t aErrorState )		{ m_errorState = aErrorState; }
	void	SetErrorAction( CvSmEventHandler::Action_t aErrorAction )	{ m_errorAction = aErrorAction; }
	
protected:
	CvSmTransitionSimple( const CvSmTransitionSimple& aOther );

	typedef std::list<CvSmEventHandler::Action_t>	CListActions;

	virtual CvSmTransition*	Duplicate() const;
	virtual bool			Execute( void* apData, OUT CvStateMachine::State_t& aNewState );
	
	CvSmEventHandler*			m_pHandler;
	CListActions				m_listActions;
	CvStateMachine::State_t		m_newState;
	CvStateMachine::State_t		m_errorState;
	CvSmEventHandler::Action_t	m_errorAction;
};

class CvSmTransitionConditional : public CvSmTransition
{
public:
	CvSmTransitionConditional( CvSmEventHandler* apHandler, CvSmEventHandler::Condition_t aCondition )
		: m_pHandler(apHandler), m_transitionOnTrue(apHandler), m_transitionOnFalse(apHandler), m_condition(aCondition)
	{}
		
	virtual ~CvSmTransitionConditional()	{}
	
	CvSmTransitionSimple&	GetTransitionOnTrue()	{ return m_transitionOnTrue; }
	CvSmTransitionSimple&	GetTransitionOnFalse()	{ return m_transitionOnFalse; }
	
protected:
	CvSmTransitionConditional( const CvSmTransitionConditional& aOther );
	
	virtual CvSmTransition*	Duplicate() const;
	virtual bool			Execute( void* apData, OUT CvStateMachine::State_t& aNewState );
	
	CvSmEventHandler*				m_pHandler;
	CvSmEventHandler::Condition_t	m_condition;
	
	CvSmTransitionSimple			m_transitionOnTrue;
	CvSmTransitionSimple			m_transitionOnFalse;
};

}	//namespace CvShared

#endif	/* CVSTATEMACHINE_H */

