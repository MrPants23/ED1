#include "EventManager.h"
#include "MurmurHash3.h"

#include "../EDUtilities/InternalOutput.h"

#ifdef _DEBUG
#include <map>
#include <algorithm>
#endif

namespace EDEventManager
{
	EventManager * EventManager::instance = nullptr;

	EventManager * EventManager::GetInstance()
	{
		// Some destructors were calling GetInstance() even after eventmanager was destroyed,
		//   causing another copy to be made. Static local is simpler anyways.
		static EventManager theEventManager;
		return &theEventManager;
	}

	void EventManager::PostEvent(const std::string &eventName, void *data, EventPriority priority, EventFlag flags)
	{
		PostEvent(GetEventId(eventName), data, priority, flags);
	}
	
	void EventManager::PostEvent(unsigned int eventId, void *data, EventPriority priority, EventFlag flags)
	{
		if(flags & EM_FLAG_IMMEDIATE)
		{
			// Instantly send this event
			PostEventToListeners(PriorityEvent(eventId, data, priority, flags));
		}
		else if(flags & EM_FLAG_PERSISTENT)
		{
			// Add to persistent events
			m_PersistentEventMap.insert(EventMapPair(eventId, PriorityEvent(eventId, data, priority, flags)));
		}
		else
		{
			// Add event to be sent out on the next update
			m_EventSet.insert(PriorityEvent(eventId, data, priority, flags));
		}
	}

	void EventManager::Update()
	{
		// Insert persistent events into eventset 
		for(auto eventIter = m_PersistentEventMap.begin(); eventIter != m_PersistentEventMap.end(); ++eventIter)
		{
			m_EventSet.insert(eventIter->second);
		}

		// Process scheduled events
		for(auto eventIter = m_EventSet.begin(); eventIter != m_EventSet.end(); ++eventIter)
		{
			PostEventToListeners(*eventIter);
		}

		m_EventSet.clear();
	}

	unsigned int EventManager::RegisterForEvent(const std::string &eventName, void *listener, 
		EM_CALLBACK callback)
	{
		unsigned int id = GetEventId(eventName);
		m_EventListenerMap.insert(ListenerMapPair(id,Listener(listener,callback)));
		return id;
	}

	void EventManager::RegisterForEvent(unsigned int eventId, void *listener, EM_CALLBACK callback)
	{
		m_EventListenerMap.insert(ListenerMapPair(eventId,Listener(listener,callback)));
	}

	void EventManager::UnRegisterForEvent(const std::string &eventName, void *listener)
	{
		unsigned int id = GetEventId(eventName);
		UnRegisterForEvent(id, listener);
	}

	void EventManager::UnRegisterForEvent(unsigned int eventId, void *listener)
	{
		auto range = m_EventListenerMap.equal_range(eventId);
		for(auto iter = range.first; iter != range.second; ++iter)
		{
			if ((*iter).second.listener == listener)
			{
				m_EventListenerMap.erase(iter);
				break;
			}
		}
	}

	void EventManager::DeletePersistentEvent(unsigned int eventId, void *eventData)
	{
		auto eventsFound = m_PersistentEventMap.equal_range(eventId);

		for(auto eventIter = eventsFound.first; eventIter != eventsFound.second; ++eventIter)
		{
			if((*eventIter).second.eventData == eventData)
			{
				m_PersistentEventMap.erase(eventIter);
				return;
			}
		}
	}

	void EventManager::DeletePersistentEvent(const std::string &eventName, void *eventData)
	{
		unsigned int id = GetEventId(eventName);
		DeletePersistentEvent(id, eventData);
	}

	void EventManager::DeletePersistentEvent(unsigned int eventId)
	{
		m_PersistentEventMap.erase(eventId);
	}

	void EventManager::DeletePersistentEvent(const std::string &eventName)
	{
		unsigned int id = GetEventId(eventName);
		DeletePersistentEvent(id);
	}

	unsigned int EventManager::GetEventId(const std::string &eventName)
	{		
		uint32_t hash = 0;
		MurmurHash3_x86_32(eventName.c_str(), static_cast<int>(eventName.length()), 42, &hash);

#if EM_DEBUG_HASHING
		DebugCheckHash(eventName, hash);
#endif

		return hash;
	}

	void EventManager::PostEventToListeners(const PriorityEvent &eventToPost)
	{
		auto eventListeners = m_EventListenerMap.equal_range(eventToPost.eventId);
		for(auto eventListenerIter = eventListeners.first; eventListenerIter != eventListeners.second; ++eventListenerIter)
		{
			Listener &currentListener = (*eventListenerIter).second;

			currentListener.callback(eventToPost.eventId, eventToPost.eventData, (*eventListenerIter).second.listener);
		}
	}

	void EventManager::DebugCheckHash(const std::string &eventName, unsigned int hash)
	{
#if EM_DEBUG_HASHING
		if(registeredHashes.count(hash) != 0)
		{
			if(registeredHashes[hash] != eventName)
			{
				EDUtilities::InternalOutput::GetReference() << " ----------------------------------------------------\n";
				EDUtilities::InternalOutput::GetReference() << " ***** Warning: Hash Collision! \n" << registeredHashes[hash] << " == " << eventName << "\n";
				EDUtilities::InternalOutput::GetReference() << " ----------------------------------------------------\n";

				MessageBoxA(0, "Hash collision detected. Check output for more details.", "EventManager Error", MB_ICONERROR | MB_OK);
			}

			return;
		}

		// This is a new hash - Check the lowercase version of this eventName to see if it already exists. This may be an
		//  eventName that only differs by case and is likely a user error
		std::string eventNameLower = eventName;
		uint32_t hashLower = 0;

		// registeredHashesAllLower[...].first = lowercase eventname
		// registeredHashesAllLower[...].second = original eventname
		std::transform(eventNameLower.begin(), eventNameLower.end(), eventNameLower.begin(), tolower);
		MurmurHash3_x86_32(eventNameLower.c_str(), static_cast<int>(eventNameLower.length()), 42, &hashLower);

		if(registeredHashesAllLower.count(hashLower) != 0 && registeredHashesAllLower[hashLower].first == eventNameLower)
		{
			if(registeredHashesAllLower[hashLower].second != eventName)
			{
				EDUtilities::InternalOutput::GetReference() << " ----------------------------------------------------\n";
				EDUtilities::InternalOutput::GetReference() << " ** Warning: Multiple events differing only by case: \n  '" << registeredHashesAllLower[hashLower].second << "' VS. '" << eventName << "'\n";
				EDUtilities::InternalOutput::GetReference() << " ----------------------------------------------------\n";

				MessageBoxA(0, "Multiple events with same name differing only by case. Check output for more details.", "EventManager Error", MB_ICONERROR | MB_OK);
			}
		}
		else
		{
			registeredHashesAllLower[hashLower] = std::make_pair<std::string, std::string>(eventNameLower, eventName);
		}

		registeredHashes[hash] = eventName;
#endif
	}
}