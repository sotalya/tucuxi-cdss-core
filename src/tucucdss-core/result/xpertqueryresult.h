// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef XPERTQUERYRESULT_H
#define XPERTQUERYRESULT_H

#include <memory>

#include "tucucommon/datetime.h"

#include "tucucdss-core/query/xpertquerydata.h"
#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This is a class that contains the results of a TuberXpert query.
///        The class is built with an XpertQueryData object. It acquires the AdminData.
///        Then, for each xpertRequest found, it creates an XpertRequestResult which is stored in an
///        internal vector.
///
///        Its main objective is to store the elements common or necessary to XpertRequestResult during
///        their lifetime.
class XpertQueryResult
{
public:
    /// \brief Constructor. Take the AdminData
    ///        unique pointer property of _xpertQuery and move each XpertRequest
    ///        unique pointer into an XpertRequestResult.
    /// \param _xpertQuery Object from which the admin data, drug treatment, the xpert requests
    ///                    and the computation time can be extracted.
    /// \param _outputPath Path to export reports.
    XpertQueryResult(
            std::unique_ptr<XpertQueryData> _xpertQuery,
            std::string _outputPath,
            std::string _templateName,
            std::string _templatePath,
            bool _jsonDump);

    XpertQueryResult(std::unique_ptr<XpertQueryData> _xpertQuery, std::string _outputPath);

    /// \brief Copy constructor is not supported because of unique pointers.
    XpertQueryResult(const XpertQueryResult& _other) = delete;

    // Getters

    /// \brief Get the computation time. This is the date element
    ///        at top of an xpert query. This value allows to define the present time.
    /// \return The computation time
    [[nodiscard]] Common::DateTime getComputationTime() const;

    /// \brief Get the admin data.
    /// \return A constant reference on the admin data.
    [[nodiscard]] const AdminData* getAdminData() const;

    /// \brief Get the XpertRequestResult objects.
    ///        This is not constant because the objects must be modifiable by the
    ///        TuberXpert flow steps.
    /// \return A reference on the vector of the XpertRequestResult objects.
    std::vector<XpertRequestResult>& getXpertRequestResults();

    /// \brief Get the path to export reports.
    /// \return The path to export reports.
    [[nodiscard]] std::string getOutputPath() const;

    [[nodiscard]] std::string getTemplateName() const;

    [[nodiscard]] bool getJsonDumpEnabled() const;

    [[nodiscard]] std::string getTemplatePath() const;

    /// \brief Get the index of the xpertRequest being processed.
    /// \return The index of the xpertRequest from the internal vector.
    ///         If no xpertRequest is processed (i.e. incrementRequestIndexBeingHandled
    ///         has not been called once), then it returns -1.
    [[nodiscard]] int getRequestIndexBeingProcessed() const;

    // Setters

    /// \brief Increment the index of the xpertRequest being processed and get the
    ///        incremented number.
    /// \return The number of the xpertRequest to process.
    int incrementRequestIndexBeingProcessed();

protected:
    /// \brief Time of computation. This field stores the value in the first date element of
    ///        the xpert query. It is usefull to get the same result (as long as the field is
    ///        not modified) when xpertRequests are executed several times but not the same day/year/month.
    Common::DateTime m_computationTime;

    /// \brief Administrative information moved from query.
    std::unique_ptr<AdminData> m_adminData;

    /// \brief Vector of the XpertRequestResult objects edited along the TuberXpert flow steps.
    std::vector<XpertRequestResult> m_xpertRequestResults;

    /// \brief The path to export reports.
    std::string m_outputPath;

    bool m_jsonDump{false};

    std::string m_templateName;
    std::string m_templatePath;
    /// \brief Index of the XpertRequestResult being processed. Used for logging and
    ///        report naming purposes only.
    int m_requestIndexBeingProcessed;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // XPERTQUERYRESULT_H
