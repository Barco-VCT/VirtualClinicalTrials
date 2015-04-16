#include "Channel.h"

#include <cassert>
#include <cstring>

#include "TypeTraits.h"

Channel* Channel::extractChannel(DSContainer* container, 
								 unsigned nComponent, unsigned nFrame, 
								 unsigned nChannel)
{
#ifdef _DEBUG
	// for debug purpose, check that the input parameters are correct
	assert(container);
	unsigned int realNChannels, realNFrames, realNComponents;
	realNComponents = container->GetNumberOfComponents();
	assert(nComponent<realNComponents);
	container->GetComponentNumberOfFrames(nComponent,&realNFrames);
	assert(nFrame<realNFrames);
	enum ComponentType element_type;
	container->GetComponentElementType(nComponent,&element_type);
	realNChannels = container->GetNbChannel(element_type);
	assert(nChannel<realNChannels);

#endif //_DEBUG

	TypeTraits::init();
	Channel* toReturn = new Channel();
	container->GetComponentElementType(nComponent,&(toReturn->m_dataType));
	container->GetComponentNumberOfColumnElementsPerFrame(
		nComponent,&(toReturn->m_width));
	container->GetComponentNumberOfRowElementsPerFrame(
		nComponent,&(toReturn->m_height));

	unsigned typeSize = TypeTraits::getTypeSize(toReturn->m_dataType); //size of the nChannel * the size of type of one element

	toReturn->m_elementSize = TypeTraits::getChannelSize(toReturn->m_dataType); //size of the type of one element
	unsigned long dataSize = toReturn->m_width*toReturn->m_height
	*toReturn->m_elementSize;		

	toReturn->m_data = new char[dataSize];

	char* buffer = new char[typeSize];
	unsigned offset = toReturn->m_elementSize*nChannel;

	for(unsigned int i=0;i<dataSize;i=i+toReturn->m_elementSize)
	{
		container->GetComponentElement(nComponent,i/toReturn->m_elementSize,nFrame,buffer);
		memcpy(&(toReturn->m_data[i]),&(buffer[offset]),toReturn->m_elementSize);
	}

	delete [] buffer;
	return toReturn;
}

char* Channel::getElement(unsigned row, unsigned col) const
{
	char* toReturn = new char[m_elementSize];
	getElement(row,col,toReturn);
	return toReturn;
}

void Channel::getElement(unsigned int row, unsigned int col, 
						 char *element) const
{
	unsigned offset = (row*m_width+col)*m_elementSize;
	memcpy(element,&(m_data[offset]),m_elementSize);
}

Channel::Channel():m_data(0){}

//Channel& Channel::operator =(const Channel &other)
//{
//	if(this == &other)
//		return *this;
//	else
//		return Channel(other);
//}

Channel::~Channel()
{
	delete [] m_data;
}
