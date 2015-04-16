//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ContainerExchanger.h
 * \brief Implementation of the class ContainerExchanger
 * \date 11-Jan-2010
 * (C) Copyright 2014 Barco NV and others.
 *
 * This software code is licensed to you under the Barco VCT Research License. The text of this license
 * is found in the file License.TXT which is available in the same repository, in absence of which you need
 * to contact Barco first before you are allowed to engage in any act of familiarization or use of whatever
 * kind. Any familiarization or use of whatever kind of this software implies acceptance of the pre-mentioned
 * license. You warrant that the modifications you make are “original” and are able to be subject to the provisions
 * of the pre-mentioned license. Any contribution containing “non-original” modification require the prior written
 * approval from Barco.
 *
 */

#ifndef CONTAINEREXCHANGER_H
#define CONTAINEREXCHANGER_H

#include <map>

#include "../Container/DSContainer.h"

/*!
 * \class ContainerExchanger
 * \brief Class providing features to exchange Containers along Modules and Simulations
 */
class ContainerExchanger
{

public:

	/*!
	 * Constructor
	 */
	ContainerExchanger();

	/*!
	 * Destructor
	 */
	~ContainerExchanger();

	/*!
	 * \fn getNumberOfContainers
	 * \brief accessor for getting number of Containers in the Containers map
	 * \return int Number of Containers in the Containers map
	 */
	int getNumberOfContainers();

	/*!
	 * \fn getNumberOfComponents
	 * \brief accessor for getting the number of components in the requested container
	 * \param key: Identifier key of Container object
	 * \return int: number of components in the requested Container
	 */
	int getNumberOfComponents(const std::string key);

	/*!
	 * \fn lookUpContainer
	 * \brief Lookup Container in Containers map
	 * \param pContainer: (Pointer to) Container object
	 * \return string: identifier key of Container object; empty if not found
	 */
	std::string lookUpContainer(DSContainer * pContainer);

	/*!
	 * \fn addContainer
	 * \bried Add Container to Containers map
	 * \param key: Identifier key of Container object
	 * \param pContainer: (Pointer to) Container object
	 * \return void
	 */
	void addContainer(const std::string key, DSContainer * pContainer);

	/*!
	 * \brief accessor for getting container from Containers map
	 * \param key: Identifier key of Container object
	 * \return  DSContainer*:  (Pointer to) Container object
	 */
	DSContainer* getContainer(const std::string key);

protected:

private:
	std::map<std::string, DSContainer *> m_containersMap; //!< (Pointers to) Containers map

};

#endif // CONTAINEREXCHANGER_H
