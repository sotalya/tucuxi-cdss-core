// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <string>

#include "tucucdss-core/flow/general/generalxpertflowstepprovider.h"

/// \brief Class that regroups common elements used by several tests.
class TestUtils
{
public:
    /// \brief General flow step provider used to get diffrent flow steps.
    static const Tucuxi::Xpert::GeneralXpertFlowStepProvider flowStepProvider;

    /// \brief Format used to create date and time during tests.
    static const std::string date_format;

    /// \brief Standard drug model string of the imatinib.
    static const std::string originalImatinibModelString;

    /// \brief Standard drug model string of the imatinib, but without the age covariate.
    static const std::string imatinibModelStringNoAge;

    /// \brief Standard drug model string of the busulfan.
    static const std::string originalBusulfanModelString;

    /// \brief Standard english translations file.
    static const std::string englishTranslationFile;

    /// \brief Set up the test environment. Load the query, make the
    ///        XpertQueryResult object, load the model, assign it to the first XpertRequestResult
    ///        of the XpertQueryResult and loads an english translationFile.
    /// \param _queryString Query string to load.
    /// \param _modelString Model string to put as drug model of the XpertRequestResult of the first request.
    /// \param _xpertQueryResult XpertQueryResult where to put the execution result of this method.
    static void setupEnv(
            const std::string& _queryString,
            const std::string& _modelString,
            std::unique_ptr<Tucuxi::Xpert::XpertQueryResult>& _xpertQueryResult);

    /// \brief Set up the test environment. Load the query, make the
    ///        XpertQueryResult object and load the models without assigning them.
    /// \param _queryString Query string to load.
    /// \param _modelStrings Drug models to load.
    /// \param _xpertQueryResult XpertQueryResult where to put the execution result of this method.
    static void setupEnv(
            const std::string& _queryString,
            const std::vector<std::string>& _modelStrings,
            std::unique_ptr<Tucuxi::Xpert::XpertQueryResult>& _xpertQueryResult);

    /// \brief Cleans the test environment
    ///
    /// This function allows to clean the test environment created by setupEnv, to avoid
    /// memory leaks.
    static void cleanEnv();

    /// \brief Load a given translations file.
    /// \param _translationsFileXml Xml string of the translations file to load.
    static void loadTranslationsFile(const std::string& _translationsFileXml);
};

#endif // TESTUTILS_H
