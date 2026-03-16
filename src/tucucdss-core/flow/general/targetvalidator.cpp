// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "targetvalidator.h"

#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucore/drugtreatment/target.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

void TargetValidator::perform(XpertRequestResult& _xpertRequestResult)
{

    // Check if there is a treatment.
    if (_xpertRequestResult.getTreatment() == nullptr) {
        _xpertRequestResult.setErrorMessage("No treatment set.");
        return;
    }

    // Check if there is drug model.
    if (_xpertRequestResult.getDrugModel() == nullptr) {
        _xpertRequestResult.setErrorMessage("No drug model set.");
        return;
    }

    // Save the active moieties of the drug model.
    const Core::ActiveMoieties& modelActiveMoieties = _xpertRequestResult.getDrugModel()->getActiveMoieties();

    // This map keeps track of the active moiety seen and, for each of them, save the corresponding target type.
    // This allows us to check, by iterating over the targets, if a similar target has already
    // been seen.
    map<Core::ActiveMoietyId, vector<Core::TargetType>> seenActiveMoietyToTargetType;

    // For each patient target, verify  that two of them do not overlap and that the active moiety belongs to the drug model.
    for (const unique_ptr<Core::Target>& target : _xpertRequestResult.getTreatment()->getTargets()) {

        // Checks if the active moiety already exists in the map.
        auto mapEntryIt = seenActiveMoietyToTargetType.find(target->getActiveMoietyId());

        // If another target with same active moiety has already been seen
        // and if the target type has already been seen.
        if (mapEntryIt != seenActiveMoietyToTargetType.end()
            && count(mapEntryIt->second.begin(), mapEntryIt->second.end(), target->getTargetType()) > 0) {
            _xpertRequestResult.setErrorMessage(
                    "Two patient's targets with the same active moiety and the same target type detected.");
            return;
        }

        // Adding the target type to the map.
        if (mapEntryIt != seenActiveMoietyToTargetType.end()) {
            mapEntryIt->second.emplace_back(target->getTargetType());
        }
        else {
            seenActiveMoietyToTargetType.emplace(
                    make_pair(target->getActiveMoietyId(), vector<Core::TargetType>{target->getTargetType()}));
        }

        // Check if the target active moiety belongs to the drug model active moieties.
        auto activeMoietiesIt =
                find_if(modelActiveMoieties.begin(),
                        modelActiveMoieties.end(),
                        [&target](const unique_ptr<Core::ActiveMoiety>& _modelActiveMoiety) {
                            return _modelActiveMoiety->getActiveMoietyId() == target->getActiveMoietyId();
                        });

        // If no active moiety found.
        if (activeMoietiesIt == modelActiveMoieties.end()) {
            _xpertRequestResult.setErrorMessage(
                    "A target is related to an active moiety (" + target->getActiveMoietyId().toString()
                    + ") that does not belong to the drug model: "
                    + _xpertRequestResult.getDrugModel()->getDrugModelId());
            return;
        }
    }
}

} // namespace Xpert
} // namespace Tucuxi
