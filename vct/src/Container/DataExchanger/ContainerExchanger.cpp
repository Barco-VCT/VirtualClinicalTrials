#include "ContainerExchanger.h"
#include <ErrorManagement/ErrorLogger.h>

using namespace std;

ContainerExchanger::ContainerExchanger()
{
}

ContainerExchanger::~ContainerExchanger()
{
	//! Clean-up Containers map
	for (map<string, DSContainer *>::iterator it = m_containersMap.begin(); it != m_containersMap.end(); it++)
	{
		delete it->second;
	}
}

int ContainerExchanger::getNumberOfContainers()
{
	//! Return
	return m_containersMap.size();
}

int ContainerExchanger::getNumberOfComponents(const std::string key)
{
	DSContainer * pContainer = 0;
	
	try
	{
		//! Get Container from Containers map
		pContainer = m_containersMap[key];
	}
	catch (...)
	{
		ErrorLogger::logError("[ContainerExchanger::getNumberOfComponents] Getting Container from Containers map failed");
	}

	//! Return
	return pContainer->GetNumberOfComponents();
}

string ContainerExchanger::lookUpContainer(DSContainer * pContainer)
{
	string key = "";

	//! Lookup Container in Containers map
	for (map<string, DSContainer *>::iterator it = m_containersMap.begin(); it != m_containersMap.end(); it++)
	{
		if (pContainer == it->second)
		{
			key = it->first;
			break;
		}
	}

	//! Return
	return key;
}

void ContainerExchanger::addContainer(const string key, DSContainer * pContainer)
{
	try
	{
		//! Add Container to Containers map
		m_containersMap[key] = pContainer;
	}
	catch (...)
	{
		ErrorLogger::logError("[ContainerExchanger::addContainer] Adding Container to Containers map failed");
	}
}

DSContainer * ContainerExchanger::getContainer(const string key)
{
	DSContainer * pContainer = 0;
	
	try
	{
		//! Get Container from Containers map
		pContainer = m_containersMap[key];
		m_containersMap.erase(key);
	}
	catch (...)
	{
		ErrorLogger::logError("[ContainerExchanger::getContainer] Getting Container from Containers map failed");
	}

	//! Return
	return pContainer;
}
