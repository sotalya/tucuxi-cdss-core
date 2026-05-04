// SPDX-License-Identifier: AGPL-3.0-or-later

#include "configimport.h"

namespace Tucuxi {
namespace Xpert {

ConfigImport::ConfigImport() = default;
ConfigImport::~ConfigImport() = default;

const std::vector<std::string>& ConfigImport::ignoredTags() const
{
    static std::vector<std::string> ignored;
    return ignored;
}

ConfigImport::Status ConfigImport::importFromFile(
        const std::string& _fileName, Configurations& _configurations, ComputationLimits& _computationLimits)
{
    setStatus(Status::Ok);

    Common::XmlDocument document;
    if (!document.open(_fileName)) {
        setStatus(Status::CantOpenFile, "File could not be opened");
        return Status::CantOpenFile;
    }

    return importDocument(document, _configurations, _computationLimits);
}

ConfigImport::Status ConfigImport::importFromString(
        const std::string& _str, Configurations& _configurations, ComputationLimits& _computationLimits)
{
    setStatus(Status::Ok);

    Common::XmlDocument document;
    if (!document.fromString(_str)) {
        setStatus(Status::CantOpenFile, "File could not be opened");
        return getStatus();
    }

    return importDocument(document, _configurations, _computationLimits);
}

ConfigImport::Status ConfigImport::importDocument(
        Common::XmlDocument& _document, Configurations& _configurations, ComputationLimits& _computationLimits)
{
    static constexpr const char* CONFIG_ROOT_NAME = "cdssConfig";
    static constexpr const char* CONFIGS_NODE_NAME = "configs";
    static constexpr const char* CONFIG_NODE_NAME = "config";
    static constexpr const char* GENERAL_CONFIG_NODE_NAME = "generalConfig";
    static constexpr const char* COMPUTATION_LIMITS_NODE_NAME = "computationLimits";

    auto root = _document.getRoot();
    if (root.getName() != CONFIG_ROOT_NAME) {
        setStatus(Status::Error, "Root of xml is not correct");
        return getStatus();
    }

    // The general config node is optional, we might use it to specify some
    // parameters that affect the operations, e.g., the  computational limits to
    // be passed to the core.
    auto generalConfigTag = root.getChildren(GENERAL_CONFIG_NODE_NAME);
    if (generalConfigTag->getName() == GENERAL_CONFIG_NODE_NAME) {
        auto computationLimitsTag = generalConfigTag->getChildren(COMPUTATION_LIMITS_NODE_NAME);
        if (computationLimitsTag->getName() == COMPUTATION_LIMITS_NODE_NAME) {

            static constexpr const char* PRED_POINTS_NODE_NAME = "nbPredictionPoints";
            static constexpr const char* PCT_POINTS_NODE_NAME = "nbPercentilesPoints";
            static constexpr const char* DOSAGE_POSSIBILITIES_NODE_NAME = "nbDosagePossibilities";

            auto predPointsConfig = computationLimitsTag->getChildren(PRED_POINTS_NODE_NAME);
            if (predPointsConfig->getName() == PRED_POINTS_NODE_NAME) {
                size_t nbPredictionPoints = getChildSizeT(computationLimitsTag, PRED_POINTS_NODE_NAME);
                _computationLimits.setPredPointsNo(nbPredictionPoints);
            }
            auto pctPointsConfig = computationLimitsTag->getChildren(PCT_POINTS_NODE_NAME);
            if (pctPointsConfig->getName() == PCT_POINTS_NODE_NAME) {
                size_t nbPercentPoints = getChildSizeT(computationLimitsTag, PCT_POINTS_NODE_NAME);
                _computationLimits.setPctPointsNo(nbPercentPoints);
            }
            auto dosagePossibConfig = computationLimitsTag->getChildren(DOSAGE_POSSIBILITIES_NODE_NAME);
            if (dosagePossibConfig->getName() == DOSAGE_POSSIBILITIES_NODE_NAME) {
                size_t nbDosagePossibilities = getChildSizeT(computationLimitsTag, DOSAGE_POSSIBILITIES_NODE_NAME);
                _computationLimits.setDosagePossibilitiesNo(nbDosagePossibilities);
            }
        }
    }

    auto configs = root.getChildren(CONFIGS_NODE_NAME);
    if (configs->getName() != CONFIGS_NODE_NAME) {
        setStatus(Status::Error, "No <configs> node present");
        return getStatus();
    }
    for (auto allConfigs = configs->getChildren(CONFIG_NODE_NAME); allConfigs != Common::XmlNodeIterator::none();
         ++allConfigs) {

        auto configName = getChildString(allConfigs, "configId");
        auto drugId = getChildString(allConfigs, "drugId");
        auto drugModelId = getChildString(allConfigs, "drugModelId");

        if (getStatus() != ConfigImport::Status::Ok) {
            return getStatus();
        }

        std::vector<ConfigCovariate> covariates;

        auto covariatesNode = allConfigs->getChildren("covariates");

        if (covariatesNode->getName() != "covariates") {
            setStatus(Status::Error, "No <covariates> node present");
            return getStatus();
        }

        for (auto allCovariates = covariatesNode->getChildren("covariate");
             allCovariates != Common::XmlNodeIterator::none();
             ++allCovariates) {

            auto covariateId = getChildString(allCovariates, "covariateId");
            auto covariateType = getChildString(allCovariates, "covariateType");
            auto dataType = getChildString(allCovariates, "dataType");
            std::vector<Constraint> validConstraint = {Constraint(0, "geq"), Constraint(1, "leq")};
            std::vector<Constraint> typicalConstraint = {Constraint(0, "geq"), Constraint(1, "leq")};
            if (dataType != "bool") {
                validConstraint = parseConstraints(allCovariates->getChildren("validRange"));
                typicalConstraint = parseConstraints(allCovariates->getChildren("typicalRange"));

                if (getStatus() != ConfigImport::Status::Ok) {
                    return getStatus();
                }
            }
            covariates.emplace_back(covariateId, covariateType, validConstraint, typicalConstraint);
        }

        if (covariates.empty()) {
            setStatus(Status::Error, "No <covariate> description given");
            return getStatus();
        }
        _configurations.addConfiguration(Configuration(configName, drugId, drugModelId, covariates));
    }
    return getStatus();
}

std::vector<Constraint> ConfigImport::parseConstraints(Common::XmlNodeIterator _rangeNode)
{
    std::vector<Constraint> constraints;
    for (; _rangeNode != Common::XmlNodeIterator::none(); ++_rangeNode) {
        for (auto constraintNode = _rangeNode->getChildren("constraint");
             constraintNode != Common::XmlNodeIterator::none();
             ++constraintNode) {

            double value = getChildDouble(constraintNode, "value");
            std::string operatorStr = getChildString(constraintNode, "operator");

            if (getStatus() != ConfigImport::Status::Ok) {
                return constraints;
            }

            try {
                constraints.emplace_back(value, operatorStr);
            }
            catch (const std::invalid_argument& e) {
                setStatus(Status::Error, "One constraint as an invalid operator");
            }
        }
    }

    if (constraints.empty()) {
        setStatus(Status::Error, "No constraint given for at least one range.");
    }

    return constraints;
}

} // namespace Xpert
} // namespace Tucuxi
