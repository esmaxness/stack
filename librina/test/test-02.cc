//
// Test 02
//
//    Eduard Grasa          <eduard.grasa@i2cat.net>
//    Francesco Salvestrini <f.salvestrini@nextworks.it>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301  USA
//

#include <iostream>
#include "librina.h"
#include "core.h"

using namespace rina;

bool checkIPCProcesses(unsigned int expectedProcesses) {
	std::vector<IPCProcess *> ipcProcesses = ipcProcessFactory
			->listIPCProcesses();
	if (ipcProcesses.size() != expectedProcesses) {
		std::cout << "ERROR: Expected " << expectedProcesses
				<< " IPC Processes, but only found " + ipcProcesses.size()
				<< "\n";
		return false;
	}

	return true;
}

int main() {
	std::cout << "TESTING LIBRINA-IPCMANAGER\n";

	/* TEST LIST IPC PROCESS TYPES */
	std::list<std::string> ipcProcessTypes = ipcProcessFactory->getSupportedIPCProcessTypes();
	std::list<std::string>::const_iterator iterator;
	for (iterator = ipcProcessTypes.begin();
			iterator != ipcProcessTypes.end();
			++iterator) {
		std::cout<<*iterator<<std::endl;
	}

	/* TEST CREATE IPC PROCESS */
	ApplicationProcessNamingInformation * ipcProcessName1 =
			new ApplicationProcessNamingInformation(
					"/ipcprocess/i2CAT/Barcelona", "1");
	ApplicationProcessNamingInformation * ipcProcessName2 =
			new ApplicationProcessNamingInformation(
					"/ipcprocess/i2CAT/Barcelona/shim", "1");
	ApplicationProcessNamingInformation * sourceName =
			new ApplicationProcessNamingInformation("/apps/test/source", "1");
	ApplicationProcessNamingInformation * destinationName =
			new ApplicationProcessNamingInformation("/apps/test/destination",
					"1");
	ApplicationProcessNamingInformation * difName =
			new ApplicationProcessNamingInformation("/difs/Test.DIF", "");

	IPCProcess * ipcProcess1 = ipcProcessFactory->create(*ipcProcessName1,
			"normal");
	IPCProcess * ipcProcess2 = ipcProcessFactory->create(*ipcProcessName2,
			"shim-ethernet");

	/* TEST LIST IPC PROCESSES */
	if (!checkIPCProcesses(2)) {
		return 1;
	}

	/* TEST DESTROY */
	ipcProcessFactory->destroy(ipcProcess2->getId());
	if (!checkIPCProcesses(1)) {
		return 1;
	}

	/* TEST INITIALIZE */
	ipcProcess1->setInitialized();

	/* TEST ASSIGN TO DIF */
	DIFInformation * difInformation = new DIFInformation();
	ipcProcess1->assignToDIF(*difInformation);
	ipcProcess1->assignToDIFResult(true);

	/* TEST REGISTER APPLICATION */
	unsigned int handle = ipcProcess1->registerApplication(*sourceName, 1);
	ipcProcess1->registerApplicationResult(handle, true);

	/* TEST UNREGISTER APPLICATION */
	handle = ipcProcess1->unregisterApplication(*sourceName);
	ipcProcess1->unregisterApplicationResult(handle, true);

	/* TEST ALLOCATE FLOW */
	FlowSpecification *flowSpec = new FlowSpecification();
	FlowRequestEvent * flowRequest = new FlowRequestEvent(*flowSpec,
			true, *sourceName, *difName, 1234, 4545);
	flowRequest->setPortId(430);
	ipcProcess1->allocateFlow(*flowRequest);

	/* TEST QUERY RIB */
	ipcProcess1->queryRIB("list of flows",
			"/dif/management/flows/", 0, 0, "");

	/* TEST APPLICATION REGISTERED */
	ApplicationRegistrationInformation appRegInfo =
			ApplicationRegistrationInformation(APPLICATION_REGISTRATION_SINGLE_DIF);
	appRegInfo.setDIFName(*difName);
	ApplicationRegistrationRequestEvent * event = new
			ApplicationRegistrationRequestEvent(appRegInfo, 34);
	applicationManager->applicationRegistered(*event, *difName, 0);

	/* TEST APPLICATION UNREGISTERED */
	ApplicationUnregistrationRequestEvent * event2 = new
			ApplicationUnregistrationRequestEvent(*sourceName, *difName, 34);
	applicationManager->applicationUnregistered(*event2, 0);

	/* TEST FLOW ALLOCATED */
	FlowRequestEvent * flowEvent = new FlowRequestEvent(25, *flowSpec,
			true, *sourceName, *destinationName, *difName, 3, 2323);
	applicationManager->flowAllocated(*flowEvent);

	ipcProcessFactory->destroy(ipcProcess1->getId());
	if (!checkIPCProcesses(0)) {
		return 1;
	}

	delete ipcProcessName1;
	delete ipcProcessName2;
	delete sourceName;
	delete destinationName;
	delete difName;
	delete difInformation;
	delete flowSpec;
	delete flowRequest;
	return 0;
}
