// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef XPERTQUERYDATA_H
#define XPERTQUERYDATA_H

#include "tucuquery/querydata.h"

#include "tucucdss-core/query/admindata.h"
#include "tucucdss-core/query/xpertrequestdata.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This class extends the queryData of tucuxi-core to include AdminData class and
///        the custom request element "xpertReequest" as XpertRequestData class.
///        In other words, it contains the content of a query for TuberXpert.
class XpertQueryData : public Query::QueryData
{
public:
    /// \brief Constructor of an xpert query.
    /// \param _queryId Identifier of the query. Just for QueryData compatibility.
    /// \param _clientId Identifier of the client. Just for QueryData compatibility.
    /// \param _queryDate Date and time to be used as "present" time.
    /// \param _language The language supported by the client. Just for QueryData compatibility.
    /// \param _admin A pointer to the admin data.
    /// \param _parameters A pointer to the medical data used for computation.
    /// \param _requests The requests that tucuxi-core would execute. Just for QueryData compatibility.
    /// \param _xpertRequests The xpert requests for TuberXpert.
    XpertQueryData(
            const std::string& _queryId,
            const std::string& _clientId,
            const Common::DateTime& _queryDate,
            const std::string& _language,
            std::unique_ptr<AdminData> _admin,
            std::unique_ptr<Query::DrugTreatmentData> _parameters,
            std::vector<std::unique_ptr<Query::RequestData>>& _requests,
            std::vector<std::unique_ptr<XpertRequestData>>& _xpertRequests);

    /// \brief Copy constructor is not supported because of unique pointers.
    XpertQueryData(const XpertQueryData& _other) = delete;

    /// \brief Give the admin data unique pointer ownership.
    /// \return A right value reference on the administrative data unique pointer.
    std::unique_ptr<AdminData>&& moveAdminData();

    /// \brief Give the ownership of an XpertRequest unique pointer.
    /// \param _i Index of the XpertRequest to give. _i must be smaller than the number of xpertRequests.
    /// \return A right value reference on the xpertRequest unique pointer.
    /// \throw out_of_range if _i is bigger or equal to the number of xpertRequests.
    std::unique_ptr<XpertRequestData>&& moveXpertRequest(size_t _i);

    // Getters

    /// \brief Get the admin data.
    /// \return The admini data as a pointer that may be nullptr.
    [[nodiscard]] const AdminData* getAdminData() const;

    /// \brief Get the custom requests for TuberXpert.
    /// \return A vector of xpertRequestData. There is one element per request.
    [[nodiscard]] const std::vector<std::unique_ptr<XpertRequestData>>& getXpertRequests() const;

protected:
    /// \brief Admin information.
    std::unique_ptr<AdminData> m_admin;

    /// \brief Custom requests for TuberXpert.
    std::vector<std::unique_ptr<XpertRequestData>> m_xpertRequests;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // XPERTQUERYDATA_H
