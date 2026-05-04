// SPDX-License-Identifier: AGPL-3.0-or-later
#include "xpertquerydata.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

XpertQueryData::XpertQueryData(
        const std::string& _queryId,
        const std::string& _clientId,
        const Common::DateTime& _queryDate,
        const std::string& _language,
        unique_ptr<AdminData> _admin,
        unique_ptr<Query::DrugTreatmentData> _parameters,
        vector<unique_ptr<Query::RequestData>>& _requests,
        vector<unique_ptr<XpertRequestData>>& _xpertRequests)
    : Query::QueryData(_queryId, _clientId, _queryDate, _language, std::move(_parameters), _requests),
      m_admin(std::move(_admin)), m_xpertRequests(std::move(_xpertRequests))
{
}

unique_ptr<AdminData> XpertQueryData::moveAdminData()
{
    return std::move(m_admin);
}

unique_ptr<XpertRequestData> XpertQueryData::moveXpertRequest(size_t _i)
{
    if (_i < m_xpertRequests.size()) {
        return std::move(m_xpertRequests[_i]);
    }

    throw out_of_range("Cannot move XpertRequestData. Out of range index.");
}

const AdminData* XpertQueryData::getAdminData() const
{
    return m_admin.get();
}

const vector<unique_ptr<XpertRequestData>>& XpertQueryData::getXpertRequests() const
{
    return m_xpertRequests;
}

} // namespace Xpert
} // namespace Tucuxi
