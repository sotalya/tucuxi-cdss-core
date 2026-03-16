// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/query/configimport.h"


namespace Tucuxi {
namespace Xpert {

class XpertConfigImportTest : public testing::Test
{
public:
    std::string xmlString = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <cdssConfig>
            <configs>
                <config>
                    <configId>vancomycin.colin2019</configId>
                    <drugId>vancomycin</drugId>
                    <drugModelId>ch.tucuxi.vancomycin.colin2019</drugModelId>
                    <covariates>
                        <covariate>
                            <covariateId>age</covariateId>
                            <covariateType>ageInYears</covariateType>
                            <dataType>double</dataType>
                            <validRange>
                                <constraint>
                                    <value>16</value>
                                    <operator>gt</operator>
                                </constraint>
                                <constraint>
                                    <value>130</value>
                                    <operator>lt</operator>
                                </constraint>
                            </validRange>
                            <typicalRange>
                                <constraint>
                                    <value>18</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>110</value>
                                    <operator>leq</operator>
                                </constraint>
                            </typicalRange>
                        </covariate>
                        <covariate>
                            <covariateId>bodyweight</covariateId>
                            <covariateType>standard</covariateType>
                            <dataType>double</dataType>
                            <validRange>
                                <constraint>
                                    <value>45</value>
                                    <operator>gt</operator>
                                </constraint>
                                <constraint>
                                    <value>135</value>
                                    <operator>lt</operator>
                                </constraint>
                            </validRange>
                            <typicalRange>
                                <constraint>
                                    <value>50</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>130</value>
                                    <operator>leq</operator>
                                </constraint>
                            </typicalRange>
                        </covariate>
                        <covariate>
                            <covariateId>sex</covariateId>
                            <covariateType>sex</covariateType>
                            <dataType>double</dataType>
                            <validRange>
                                <constraint>
                                    <value>0.0</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>1.0</value>
                                    <operator>leq</operator>
                                </constraint>
                            </validRange>
                            <typicalRange>
                                <constraint>
                                    <value>0.0</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>1.0</value>
                                    <operator>leq</operator>
                                </constraint>
                            </typicalRange>
                        </covariate>
                    </covariates>
                </config>
            </configs>
        </cdssConfig>
    )";

    std::string wrongRoot = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <wrongRoot>
            <configs>
                <config>
                    <configId>vancomycin.colin2019</configId>
                    <drugId>vancomycin</drugId>
                    <drugModelId>ch.tucuxi.vancomycin.colin2019</drugModelId>
                    <covariates>
                        <covariate>
                            <covariateId>age</covariateId>
                            <covariateType>ageInYears</covariateType>
                            <validRange>
                                <constraint>
                                    <value>16</value>
                                    <operator>gt</operator>
                                </constraint>
                                <constraint>
                                    <value>130</value>
                                    <operator>lt</operator>
                                </constraint>
                            </validRange>
                            <typicalRange>
                                <constraint>
                                    <value>18</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>110</value>
                                    <operator>leq</operator>
                                </constraint>
                            </typicalRange>
                        </covariate>
                        <covariate>
                            <covariateId>bodyweight</covariateId>
                            <covariateType>standard</covariateType>
                            <validRange>
                                <constraint>
                                    <value>45</value>
                                    <operator>gt</operator>
                                </constraint>
                                <constraint>
                                    <value>135</value>
                                    <operator>lt</operator>
                                </constraint>
                            </validRange>
                            <typicalRange>
                                <constraint>
                                    <value>50</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>130</value>
                                    <operator>leq</operator>
                                </constraint>
                            </typicalRange>
                        </covariate>
                        <covariate>
                            <covariateId>sex</covariateId>
                            <covariateType>sex</covariateType>
                            <validRange>
                                <constraint>
                                    <value>0.0</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>1.0</value>
                                    <operator>leq</operator>
                                </constraint>
                            </validRange>
                            <typicalRange>
                                <constraint>
                                    <value>0.0</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>1.0</value>
                                    <operator>leq</operator>
                                </constraint>
                            </typicalRange>
                        </covariate>
                    </covariates>
                </config>
            </configs>
        </wrongRoot>
    )";

    std::string noConfigs = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <cdssConfig>
            <policies>
            </policies>
        </cdssConfig>
    )";

    std::string oneConfigMissing = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <cdssConfig>
            <configs>
                <config>
                    <configId>vancomycin.colin2019</configId>
                    <drugModelId>ch.tucuxi.vancomycin.colin2019</drugModelId>
                </config>
            </configs>
        </cdssConfig>
    )";

    std::string covariatesMissing = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <cdssConfig>
            <configs>
                <config>
                    <configId>vancomycin.colin2019</configId>
                    <drugId>vancomycin</drugId>
                    <drugModelId>ch.tucuxi.vancomycin.colin2019</drugModelId>
                </config>
            </configs>
        </cdssConfig>
    )";

    std::string covariatesChildWrong = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <cdssConfig>
            <configs>
                <config>
                    <configId>vancomycin.colin2019</configId>
                    <drugId>vancomycin</drugId>
                    <drugModelId>ch.tucuxi.vancomycin.colin2019</drugModelId>
                    <covariates>
                        <falseCovariate>
                        </falseCovariate>
                    </covariates>
                </config>
            </configs>
        </cdssConfig>
    )";

    std::string covariateMissingInfo = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <cdssConfig>
            <configs>
                <config>
                    <configId>vancomycin.colin2019</configId>
                    <drugId>vancomycin</drugId>
                    <drugModelId>ch.tucuxi.vancomycin.colin2019</drugModelId>
                    <covariates>
                        <covariate>
                            <covariateId>sex</covariateId>
                            <validRange>
                                <constraint>
                                    <value>0.0</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>1.0</value>
                                    <operator>leq</operator>
                                </constraint>
                            </validRange>
                            <typicalRange>
                                <constraint>
                                    <value>0.0</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>1.0</value>
                                    <operator>leq</operator>
                                </constraint>
                            </typicalRange>
                        </covariate>
                    </covariates>
                </config>
            </configs>
        </cdssConfig>
    )";

    std::string covariateMissingConstraints = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <cdssConfig>
            <configs>
                <config>
                    <configId>vancomycin.colin2019</configId>
                    <drugId>vancomycin</drugId>
                    <drugModelId>ch.tucuxi.vancomycin.colin2019</drugModelId>
                    <covariates>
                        <covariate>
                            <covariateId>sex</covariateId>
                            <covariateType>sex</covariateType>
                            <dataType>double</dataType>
                            <validRange>
                                <constraint>
                                    <value>0.0</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>1.0</value>
                                    <operator>leq</operator>
                                </constraint>
                            </validRange>
                        </covariate>
                    </covariates>
                </config>
            </configs>
        </cdssConfig>
    )";

    std::string covariateWrongOp = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <cdssConfig>
            <configs>
                <config>
                    <configId>vancomycin.colin2019</configId>
                    <drugId>vancomycin</drugId>
                    <drugModelId>ch.tucuxi.vancomycin.colin2019</drugModelId>
                    <covariates>
                        <covariate>
                            <covariateId>sex</covariateId>
                            <covariateType>sex</covariateType>
                            <dataType>double</dataType>
                            <validRange>
                                <constraint>
                                    <value>0.0</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>1.0</value>
                                    <operator>aled</operator>
                                </constraint>
                            </validRange>
                            <typicalRange>
                                <constraint>
                                    <value>0.0</value>
                                    <operator>geq</operator>
                                </constraint>
                                <constraint>
                                    <value>1.0</value>
                                    <operator>leq</operator>
                                </constraint>
                            </typicalRange>
                        </covariate>
                    </covariates>
                </config>
            </configs>
        </cdssConfig>
    )";
};


// IMPORT TESTS

TEST_F(XpertConfigImportTest, ImportCorrectXML)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status = importer.importFromString(xmlString, *configurations, computationLimits);

    EXPECT_EQ(status, ConfigImport::Status::Ok);
}

TEST_F(XpertConfigImportTest, ImportWrongXmlRoot)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status = importer.importFromString(wrongRoot, *configurations, computationLimits);

    EXPECT_EQ(status, ConfigImport::Status::Error);
    EXPECT_EQ(importer.getErrorMessage(), "Root of xml is not correct\n");
}

TEST_F(XpertConfigImportTest, ImportMissingConfigs)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status = importer.importFromString(noConfigs, *configurations, computationLimits);

    EXPECT_EQ(status, ConfigImport::Status::Error);
    EXPECT_EQ(importer.getErrorMessage(), "No <configs> node present\n");
}

TEST_F(XpertConfigImportTest, ImportMissingConfigCoreInfo)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status = importer.importFromString(oneConfigMissing, *configurations, computationLimits);

    EXPECT_EQ(status, ConfigImport::Status::Error);
    EXPECT_EQ(importer.getErrorMessage(), "<drugId> not found in xml input\n");
}

TEST_F(XpertConfigImportTest, ImportMissingCovariatesList)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status = importer.importFromString(covariatesMissing, *configurations, computationLimits);

    EXPECT_EQ(status, ConfigImport::Status::Error);
    EXPECT_EQ(importer.getErrorMessage(), "No <covariates> node present\n");
}

TEST_F(XpertConfigImportTest, ImportMissingCovariateNode)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status = importer.importFromString(covariatesChildWrong, *configurations, computationLimits);

    EXPECT_EQ(status, ConfigImport::Status::Error);
    EXPECT_EQ(importer.getErrorMessage(), "No <covariate> description given\n");
}

TEST_F(XpertConfigImportTest, ImportMissingCovariateInfo)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status = importer.importFromString(covariateMissingInfo, *configurations, computationLimits);

    EXPECT_EQ(status, ConfigImport::Status::Error);
    EXPECT_EQ(
            importer.getErrorMessage(),
            "<covariateType> not found in xml input\n<dataType> not found "
            "in xml input\n");
}

TEST_F(XpertConfigImportTest, ImportMissingCovariateConstraint)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status =
            importer.importFromString(covariateMissingConstraints, *configurations, computationLimits);

    EXPECT_EQ(status, ConfigImport::Status::Error);
    EXPECT_EQ(importer.getErrorMessage(), "No constraint given for at least one range.\n");
}

TEST_F(XpertConfigImportTest, ImportConstraintUnknownOp)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status = importer.importFromString(covariateWrongOp, *configurations, computationLimits);

    EXPECT_EQ(status, ConfigImport::Status::Error);
    EXPECT_EQ(importer.getErrorMessage(), "One constraint as an invalid operator\n");
}

// ConfigImporter features.

TEST_F(XpertConfigImportTest, TestConfigExtractor)
{
    std::unique_ptr<Configurations> configurations = std::make_unique<Configurations>();

    ConfigImport importer;
    ComputationLimits computationLimits;
    ConfigImport::Status status = importer.importFromString(xmlString, *configurations, computationLimits);

    Configuration conf1 = configurations->getConfiguration("vancomycin.colin2019");
    Configuration conf2 = configurations->getConfiguration("non_existant_config");

    EXPECT_EQ(conf1.isValid(), true);
    EXPECT_EQ(conf2.isValid(), false);
}

} // namespace Xpert
} // namespace Tucuxi
