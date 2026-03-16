// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include "xpertqueryresult.h"

#include "tucucdss-core/query/xpertquerytocoreextractor.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

XpertQueryResult::XpertQueryResult(unique_ptr<XpertQueryData> _xpertQuery, std::string _outputPath)
    : m_computationTime(_xpertQuery->getpQueryDate()), m_adminData(_xpertQuery->moveAdminData()),
      m_outputPath(std::move(_outputPath)), m_requestIndexBeingProcessed(-1)

{
    XpertQueryToCoreExtractor extractor;

    // For each xpertRequest, extract its treatment.
    for (size_t i = 0; i < _xpertQuery->getXpertRequests().size(); ++i) {
        std::string errorMessage;
        unique_ptr<Core::DrugTreatment> drugTreatment =
                extractor.extractDrugTreatment(_xpertQuery->getXpertRequests()[i].get(), *_xpertQuery, errorMessage);

        m_xpertRequestResults.emplace_back(
                *this, _xpertQuery->moveXpertRequest(i), std::move(drugTreatment), errorMessage);
    }
}


XpertQueryResult::XpertQueryResult(
        unique_ptr<XpertQueryData> _xpertQuery,
        std::string _outputPath,
        std::string _templateName,
        std::string _templatePath,
        bool _jsonDump)
    : m_computationTime(_xpertQuery->getpQueryDate()), m_adminData(_xpertQuery->moveAdminData()),
      m_outputPath(std::move(_outputPath)), m_templateName(std::move(_templateName)),
      m_templatePath(std::move(_templatePath)), m_requestIndexBeingProcessed(-1), m_jsonDump(_jsonDump)

{
    XpertQueryToCoreExtractor extractor;

    // For each xpertRequest, extract its treatment.
    for (size_t i = 0; i < _xpertQuery->getXpertRequests().size(); ++i) {
        std::string errorMessage;
        unique_ptr<Core::DrugTreatment> drugTreatment =
                extractor.extractDrugTreatment(_xpertQuery->getXpertRequests()[i].get(), *_xpertQuery, errorMessage);

        m_xpertRequestResults.emplace_back(
                *this, _xpertQuery->moveXpertRequest(i), std::move(drugTreatment), errorMessage);
    }
}

Common::DateTime XpertQueryResult::getComputationTime() const
{
    return m_computationTime;
}

const AdminData* XpertQueryResult::getAdminData() const
{
    return m_adminData.get();
}

vector<XpertRequestResult>& XpertQueryResult::getXpertRequestResults()
{
    return m_xpertRequestResults;
}

std::string XpertQueryResult::getOutputPath() const
{
    return m_outputPath;
}

std::string XpertQueryResult::getTemplateName() const
{
    return m_templateName;
}

bool XpertQueryResult::getJsonDumpEnabled() const
{
    return m_jsonDump;
}

std::string XpertQueryResult::getTemplatePath() const
{
    return m_templatePath;
}

int XpertQueryResult::getRequestIndexBeingProcessed() const
{
    return m_requestIndexBeingProcessed;
}

int XpertQueryResult::incrementRequestIndexBeingProcessed()
{
    return ++m_requestIndexBeingProcessed;
}

} // namespace Xpert
} // namespace Tucuxi
