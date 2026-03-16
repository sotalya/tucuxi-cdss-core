// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef ABSTRACTXPERTREQUESTRESULTEXPORT_H
#define ABSTRACTXPERTREQUESTRESULTEXPORT_H

#include "tucucdss-core/result/xpertrequestresult.h"

namespace Tucuxi {
namespace Xpert {

/// \brief This class is an abstract class that represents the set of
///        exporters of XpertRequestResult.
class AbstractXpertRequestResultExport
{
public:
    /// \brief Destructor.
    virtual ~AbstractXpertRequestResultExport(){};

    /// \brief Export the XpertRequestResult to a file.
    /// \param _xpertRequestResult Result of the xpertRequest to export.
    virtual void exportToFile(XpertRequestResult& _xpertRequestResult) = 0;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // ABSTRACTXPERTREQUESTRESULTEXPORT_H
