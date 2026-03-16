// SPDX-License-Identifier: AGPL-3.0-or-later
#include "xpertquerytocoreextractor.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

unique_ptr<Core::DrugTreatment> XpertQueryToCoreExtractor::extractDrugTreatment(
        const XpertRequestData* _xpertRequest, const XpertQueryData& _xpertQuery, std::string& _errorMessage) const
{
    // Identifier of the drug to extract.
    std::string drugId = _xpertRequest->getDrugId();

    // Get the number of matching drugs.
    auto drugsDataBegin = _xpertQuery.getpParameters().getDrugs().begin();
    auto drugsDataEnd = _xpertQuery.getpParameters().getDrugs().end();

    auto nbMatchingDrug =
            count_if(drugsDataBegin, drugsDataEnd, [&drugId](const unique_ptr<Query::DrugData>& _drugData) {
                return _drugData->getDrugID() == drugId;
            });

    // If there is no drug matching or multiple drugs matching.
    if (nbMatchingDrug != 1) {
        if (nbMatchingDrug == 0) {
            _errorMessage = "No drug matching. Could not extract drug treatment.";
        }
        else {
            _errorMessage = "Too many drugs matching. Could not extract drug treatment.";
        }
        return nullptr;
    }

    // There is only one drug matching. Extract the treatment.
    // To do this, prepare a request data so that QueryToCoreExtractor::extractDrugTreatment
    // can extract the correct treatment.
    std::string requestId;
    std::string drugModelId;
    Query::RequestData requestData{requestId, drugId, drugModelId, nullptr};

    _errorMessage = "";
    return QueryToCoreExtractor::extractDrugTreatment(_xpertQuery, requestData);
}

} // namespace Xpert
} // namespace Tucuxi
