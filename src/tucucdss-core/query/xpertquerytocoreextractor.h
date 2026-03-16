// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef XPERTQUERYTOCOREEXTRACTOR_H
#define XPERTQUERYTOCOREEXTRACTOR_H

#include <string>

#include "tucucore/drugtreatment/drugtreatment.h"

#include "tucuquery/querytocoreextractor.h"

#include "tucucdss-core/query/xpertquerydata.h"
#include "tucucdss-core/query/xpertrequestdata.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This class extends the QueryToCoreExtractor of Tucuxi.
///        The main point of this class is to provide a method
///        to extract a DrugTreatment for a given XpertRequestData.
class XpertQueryToCoreExtractor : Query::QueryToCoreExtractor
{
public:
    /// \brief Extract a DrugTreatment for a given XpertRequestData based on
    ///        an XpertQueryData. This method simply adds a few verification steps
    ///        before calling the QueryToCoreExtractor::extractDrugTreatment method.
    ///        There must be precisely one drug that matches the drug identifier
    ///        in the XpertRequest.
    /// \param _xpertRequest XpertRequest that contains the identifier of the drug to extract.
    /// \param _xpertQuery Xpert query where to extract the drug treatment.
    /// \param _errorMessage Error message to store information if something goes wrong.
    ///                      It is an empty string if everything is fine.
    /// \return A unique pointer to the generated DrugTreatment.
    ///         May be nullptr if it fails.
    std::unique_ptr<Core::DrugTreatment> extractDrugTreatment(
            const XpertRequestData* _xpertRequest, const XpertQueryData& _xpertQuery, std::string& _errorMessage) const;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // XPERTQUERYTOCOREEXTRACTOR_H
