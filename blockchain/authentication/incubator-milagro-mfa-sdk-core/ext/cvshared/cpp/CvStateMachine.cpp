/* 
 * File:   CvStateMachine.cpp
 * Author: mony
 * 
 * Created on September 21, 2012, 4:54 PM
 */

#include <string>
#include <list>

#include "CvStateMachine.h"

#include "CvLogger.h"

namespace CvShared
{

////////////////////////////////////////////////////////////////////////////////

CvStateMachine::CvStateMachine( const char* apName ) :
	m_name(apName), m_mutex( (CvString("sm-")+apName).c_str() )
{
	m_mutex.Create();
}
	
CvStateMachine::~CvStateMachine( )
{
	CMapTransitions::iterator itr = m_mapTransitions.begin();	
	while ( itr != m_mapTransitions.end() )
	{
		CMapTransitions::iterator itrTmp = itr;
		
		++itr;
		
		CvSmTransition* pTransition = itrTmp->second;

		m_mapTransitions.erase( itrTmp );
		
		if ( pTransition != NULL )
			delete pTransition;		
	}
}

bool CvStateMachine::AddTransition( State_t aState, Event_t aEvent, const CvSmTransition& aTransition )
{
	CvMutexLock lock(m_mutex);

	if ( m_mapStates.count(aState) < 1 )
	{
		LogMessage( enLogLevel_Error, "SM [%s]: Cannot add transition - state [%d] not found", m_name.c_str(), aState );
		return false;
	}
	
	if ( m_mapEvents.count(aEvent) < 1 )
	{
		LogMessage( enLogLevel_Error, "SM [%s]: Cannot add transition - event [%d] not found", m_name.c_str(), aEvent );
		return false;
	}

	m_mapTransitions[ MakeTransitionKey(aState,aEvent) ] = aTransition.Duplicate();
	
	return true;
}
	
bool CvStateMachine::Start( State_t aInitialState )
{
	CvMutexLock lock(m_mutex);

	if ( m_mapStates.count(aInitialState) < 1 )
	{
		LogMessage( enLogLevel_Error, "SM [%s]: Cannot start - initial state [%d] not found", m_name.c_str(), aInitialState );
		return false;
	}
	
	m_state = aInitialState;
	
	LogMessage( enLogLevel_Info, "SM [%s] started with initial state [%s]", m_name.c_str(), m_mapStates[m_state].c_str() );
	
	return true;
}

bool CvStateMachine::Signal( Event_t aEvent, void* apData )
{
	CvMutexLock lock(m_mutex);
	
	if ( m_mapStates.count(m_state) < 1 )
	{
		LogMessage( enLogLevel_Error, "SM [%s]: Cannot signal - current state [%d] is not valid. Probably state machine was not started yet",
				m_name.c_str(), m_state );
		return false;
	}

	if ( m_mapEvents.count(aEvent) < 1 )
	{
		LogMessage( enLogLevel_Error, "SM [%s]: Cannot signal - event [%d] not found", m_name.c_str(), aEvent );
		return false;
	}
	
	CTransitionKey transitionKey = MakeTransitionKey( m_state, aEvent );
	
	if ( m_mapTransitions.count(transitionKey) < 1 )
	{
		LogMessage( enLogLevel_Error, "SM [%s]: No transition defined for state [%s] and event [%s]",
				m_name.c_str(), m_mapStates[m_state].c_str(), m_mapEvents[aEvent].c_str() );
		return false;
	}
	
	if ( m_mapTransitions[transitionKey] == NULL )	
	{
		LogMessage( enLogLevel_Error, "SM [%s]: Transition for state [%s] and event [%s] is NULL",
				m_name.c_str(), m_mapStates[m_state].c_str(), m_mapEvents[aEvent].c_str() );
		return false;
	}
	
	State_t newState = INVALID_STATE;
	
	bool bOk = m_mapTransitions[transitionKey]->Execute( apData, newState );
	
	if ( bOk )
	{
		if ( m_mapStates.count( newState ) < 1 )
		{
			LogMessage( enLogLevel_Error, "SM [%s]: Cannot change state - new state [%d] not found", m_name.c_str(), newState );
			return false;
		}
		
		LogMessage( enLogLevel_Info, "SM [%s]: State [%s] Event [%s] -> State [%s]",
				m_name.c_str(), m_mapStates[m_state].c_str(), m_mapEvents[aEvent].c_str(), m_mapStates[newState].c_str() );
		
		m_state = newState;
	}
	else
	{
		if ( newState != INVALID_STATE )
		{
			if ( m_mapStates.count( newState ) < 1 )
			{
				LogMessage( enLogLevel_Error, "SM [%s]: Cannot change state - ERROR state [%d] not found", m_name.c_str(), newState );
				return false;
			}
			
			LogMessage( enLogLevel_Info, "SM [%s]: State [%s] Event [%s] -> ERROR state [%s]",
					m_name.c_str(), m_mapStates[m_state].c_str(), m_mapEvents[aEvent].c_str(), m_mapStates[newState].c_str() );

			m_state = newState;			
		}
		else
		{
			LogMessage( enLogLevel_Info, "SM [%s]: State [%s] Event [%s] -> ERROR (stays in current state)",
					m_name.c_str(), m_mapStates[m_state].c_str(), m_mapEvents[aEvent].c_str() );
		}
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////

CvSmTransitionSimple::CvSmTransitionSimple( const CvSmTransitionSimple& aOther ) :
	m_pHandler(aOther.m_pHandler), m_listActions(aOther.m_listActions),
	m_newState(aOther.m_newState), m_errorState(aOther.m_errorState),
	m_errorAction(aOther.m_errorAction)
{
}

CvSmTransition*	CvSmTransitionSimple::Duplicate() const
{
	return new CvSmTransitionSimple(*this);
}

bool CvSmTransitionSimple::Execute( void* apData, OUT CvStateMachine::State_t& aNewState )
{
	for ( CListActions::iterator itr = m_listActions.begin();
		 itr != m_listActions.end();
		 ++itr )
	{
		CvSmEventHandler::Action_t action = *itr;
		
		if ( !(m_pHandler->*action)( apData ) )
		{
			if ( m_errorAction != NULL )
				!(m_pHandler->*m_errorAction)( apData );
			
			aNewState = m_errorState;
			return false;			
		}
	}
	
	aNewState = m_newState;	
	return true;
}

////////////////////////////////////////////////////////////////////////////////

CvSmTransitionConditional::CvSmTransitionConditional( const CvSmTransitionConditional& aOther ) :
	m_pHandler(aOther.m_pHandler), m_condition(aOther.m_condition),
	m_transitionOnTrue(aOther.m_transitionOnTrue), m_transitionOnFalse(aOther.m_transitionOnFalse)
{
}

CvSmTransition*	CvSmTransitionConditional::Duplicate() const
{
	return new CvSmTransitionConditional(*this);
}

bool CvSmTransitionConditional::Execute( void* apData, OUT CvStateMachine::State_t& aNewState )
{
	if ( (m_pHandler->*m_condition)( apData ) )
		return m_transitionOnTrue.Execute( apData, aNewState );
	else
		return m_transitionOnFalse.Execute( apData, aNewState );
}

}	//namespace CvShared