/*
 * Enrollment
 *
 *    Bernat Gaston         <bernat.gaston@i2cat.net>
 *    Eduard Grasa          <eduard.grasa@i2cat.net>
 *    Francesco Salvestrini <f.salvestrini@nextworks.it>
 *    Vincenzo Maffione <v.maffione@nextworks.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */

#ifndef LIBRINA_ENROLLMENT_H
#define LIBRINA_ENROLLMENT_H

#ifdef __cplusplus

#include "irm.h"
#include "timer.h"
#include "security-manager.h"
#include "rib_v2.h"

namespace rina {

/**
 * The App Manager requests the application to enroll to a DAF,
 * through neighbour neighbourName, which can be reached by allocating
 * a flow through the supportingDIFName
 */
class EnrollToDAFRequestEvent: public IPCEvent {
public:
        /** The DAF to enroll to */
        ApplicationProcessNamingInformation dafName;

        /** The N-1 DIF name to allocate a flow to the member */
        ApplicationProcessNamingInformation supportingDIFName;

        /** The neighbor to contact */
        ApplicationProcessNamingInformation neighborName;

        EnrollToDAFRequestEvent() { };
        EnrollToDAFRequestEvent(
                const ApplicationProcessNamingInformation& daf,
                const ApplicationProcessNamingInformation& supportingDIF,
                const ApplicationProcessNamingInformation& neighbor,
                unsigned int sequenceNumber) : IPCEvent(ENROLL_TO_DIF_REQUEST_EVENT, sequenceNumber),
                	dafName(daf), supportingDIFName(supportingDIF),
                	neighborName(neighbor) { };
};

/// Contains the information of an enrollment request
class EnrollmentRequest
{
public:
	EnrollmentRequest(Neighbor * neighbor) : neighbor_(neighbor),
		ipcm_initiated_(false) { };
	EnrollmentRequest(Neighbor * neighbor,
                          const EnrollToDAFRequestEvent & event) : neighbor_(neighbor),
                 event_(event), ipcm_initiated_(true) { };

	/// The neighbor to enroll to
	Neighbor * neighbor_;

	/// The request event that triggered the Enrollment
	EnrollToDAFRequestEvent event_;

	/// True if the enrollment request came via the IPC Manager
	bool ipcm_initiated_;
};

class NeighborRIBObj: public rib::RIBObj {
public:
	NeighborRIBObj(Neighbor* neigh);
	const std::string get_displayable_value() const;
	const std::string& get_class() const {
		return class_name;
	};

	const static std::string class_name;
	const static std::string object_name_prefix;

private:

	Neighbor * neighbor;
};

class NeighborsRIBObj: public rib::RIBObj {
public:
	NeighborsRIBObj(ApplicationProcess * app,
			rib::RIBDaemonProxy * rib_daemon,
		        rib::rib_handle_t rib_handle);
	const std::string& get_class() const {
		return class_name;
	};

	//Create
	void create(const cdap_rib::con_handle_t &con,
		    const std::string& fqn,
		    const std::string& class_,
		    const cdap_rib::filt_info_t &filt,
		    const int invoke_id,
		    const ser_obj_t &obj_req,
		    ser_obj_t &obj_reply,
		    cdap_rib::res_info_t& res);

	const static std::string class_name;
	const static std::string object_name;

private:
	ApplicationProcess * app_;
	rib::RIBDaemonProxy * ribd;
	rib::rib_handle_t rib;
};

/// Interface that must be implementing by classes that provide
/// the behavior of an enrollment task
class IEnrollmentTask : public rina::ApplicationEntity,
			public rina::cacep::AppConHandlerInterface {
public:
	IEnrollmentTask() : rina::ApplicationEntity(ApplicationEntity::ENROLLMENT_TASK_AE_NAME) { };
	virtual ~IEnrollmentTask() { };
	virtual const std::list<Neighbor *> get_neighbors() const = 0;
	virtual const std::list<std::string> get_enrolled_app_names() const = 0;

	/// Process a request to initiate enrollment with a new Neighbor, triggered by the IPC Manager
	/// @param event
	virtual void processEnrollmentRequestEvent(EnrollToDAFRequestEvent * event) = 0;

	/// Starts the enrollment program
	/// @param cdapMessage
	/// @param cdapSessionDescriptor
	virtual void initiateEnrollment(EnrollmentRequest * request) = 0;

	/// Called by the enrollment state machine when the enrollment request has been completed,
	/// either successfully or unsuccessfully
	/// @param neighbor the App process we were trying to enroll to
	/// @param enrollee true if this App process is the one that initiated the
	/// enrollment sequence (i.e. it is the application process that wants to
	/// join the DIF)
	virtual void enrollmentCompleted(const Neighbor& neighbor,
                                         bool enrollee) = 0;

	/// Called by the enrollment state machine when the enrollment sequence fails
	/// @param remotePeer
	/// @param portId
	/// @param enrollee
	/// @param sendMessage
	/// @param reason
	virtual void enrollmentFailed(const ApplicationProcessNamingInformation& remotePeerNamingInfo,
                                      int portId,
                                      const std::string& reason,
                                      bool sendReleaseMessage) = 0;

	/// Finds out if the App process is already enrolled to the App process identified by
	/// the provided apNamingInfo
	/// @param apNamingInfo
	/// @return
	virtual bool isEnrolledTo(const std::string& applicationProcessName) = 0;

	/// Callback used to signal the enrollment task that authentication completed
	/// successfully (if success = true) or failed (if success = false)
	virtual void authentication_completed(int port_id, bool success) = 0;
};

}

#endif

#endif
