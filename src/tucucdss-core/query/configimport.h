// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef CONFIGIMPORT_H
#define CONFIGIMPORT_H

#include <string>
#include <vector>

#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlimporter.h"
#include "tucucommon/xmlnode.h"

#include "configuration.h"

namespace Tucuxi {
namespace Xpert {

class ConfigImport : public Tucuxi::Common::XMLImporter
{
public:
    ///
    /// \brief ConfigImport
    /// Constructs a ConfigImport object.
    ///
    ConfigImport();

    ///
    /// \brief ~ConfigImport
    /// Destructor for ConfigImport object.
    ///
    virtual ~ConfigImport();

    ///
    /// \brief importFromFile
    /// Imports configuration from a specified file.
    /// \param _fileName The name of the file containing the configuration.
    /// \param _configurations The Configurations object to be populated with the imported data.
    /// \param _computationLimits Computation limits that could be overwritten by the imported data.
    /// \return Status indicating the success or failure of the import operation.
    ///
    Status importFromFile(
            const std::string& _fileName, Configurations& _configurations, ComputationLimits& _computationLimits);

    ///
    /// \brief importFromString
    /// Imports configuration from a given string.
    /// \param _str The string containing the configuration.
    /// \param _configurations The Configurations object to be populated with the imported data.
    /// \param _computationLimits Computation limits that could be overwritten by the imported data.
    /// \return Status indicating the success or failure of the import operation.
    ///
    Status importFromString(
            const std::string& _str, Configurations& _configurations, ComputationLimits& _computationLimits);

private:
    std::vector<Configuration> m_configurations;

    const std::vector<std::string>& ignoredTags() const override;

    ///
    /// \brief importDocument
    /// Imports an XML document and initializes the configuration data.
    /// \param _document The XML document to be imported.
    /// \param configurations The Configurations object to be populated with the imported data.
    /// \param _computationLimits Computation limits that could be overwritten by the imported data.
    /// \return Status indicating the success or failure of the import operation.
    ///
    Status importDocument(
            Tucuxi::Common::XmlDocument& _document,
            Configurations& _configurations,
            ComputationLimits& _computationLimits);

    ///
    /// \brief parseConstraints
    /// Parses a range node and extracts the constraints.
    /// \param _rangeNode An iterator pointing to the range node in the XML document.
    /// \return A vector of Constraint objects parsed from the range node.
    ///
    std::vector<Constraint> parseConstraints(Common::XmlNodeIterator _rangeNode);
};

} // namespace Xpert
} // namespace Tucuxi

#endif // CONFIGIMPORT_H
