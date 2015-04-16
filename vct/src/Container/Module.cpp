#include "Module.h"

Module::Module()
:m_debug(false),
 m_pUtilitiesExchanger(0),
 m_pContainerExchanger(0),
 m_pMevicLogger(0)
{
}

void Module::SetUtilitiesExchanger(UtilitiesExchanger * pUtilitiesExchanger)
{
	this->m_pUtilitiesExchanger = pUtilitiesExchanger;
}

void Module::SetContainerExchanger(ContainerExchanger * pContainerExchanger)
{
	this->m_pContainerExchanger = pContainerExchanger;
}

void Module::SetMevicLogger(MevicLogger * pMevicLogger)
{
	this->m_pMevicLogger = pMevicLogger;
}

Module::~Module()
{
}