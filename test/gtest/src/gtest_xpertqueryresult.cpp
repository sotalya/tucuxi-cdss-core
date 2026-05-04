// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "tucucdss-core/query/xpertqueryimport.h"
#include "tucucdss-core/result/xpertqueryresult.h"

/// \brief Gtest of XpertQueryResult.
///        Additional tests for XpertQueryResult class.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined fixture for the tests          *
 ***********************************************************/

class XpertQueryResultAdditionalTest : public testing::Test
{
public:
    std::string queryStringSingleRequest = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                     <query version="1.0"
                                         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                         xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                         <date>2018-07-11T13:45:30</date>

                                         <drugTreatment>
                                             <patient>
                                                 <covariates>
                                                 </covariates>
                                             </patient>
                                             <drugs>
                                                 <drug>
                                                     <drugId>imatinib</drugId>
                                                     <activePrinciple>something</activePrinciple>
                                                     <brandName>somebrand</brandName>
                                                     <atc>something</atc>
                                                     <treatment>
                                                         <dosageHistory>
                                                         </dosageHistory>
                                                     </treatment>
                                                     <samples>
                                                     </samples>
                                                     <targets>
                                                     </targets>
                                                 </drug>
                                             </drugs>
                                         </drugTreatment>
                                         <requests>
                                             <xpertRequest>
                                                 <drugId>imatinib</drugId>
                                                 <configId>imatinib.gotta2012</configId>
                                                 <output>
                                                     <format>xml</format>
                                                     <language>en</language>
                                                 </output>
                                             </xpertRequest>
                                         </requests>
                                     </query>)";

    std::string queryStringMultipleRequests = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                     <query version="1.0"
                                         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                         xsi:noNamespaceSchemaLocation="tuberxpert_computing_query.xsd">

                                         <date>2018-07-11T13:45:30</date>

                                         <drugTreatment>
                                             <patient>
                                                 <covariates>
                                                 </covariates>
                                             </patient>
                                             <drugs>
                                                 <drug>
                                                     <drugId>imatinib</drugId>
                                                     <activePrinciple>something</activePrinciple>
                                                     <brandName>somebrand</brandName>
                                                     <atc>something</atc>
                                                     <treatment>
                                                         <dosageHistory>
                                                         </dosageHistory>
                                                     </treatment>
                                                     <samples>
                                                     </samples>
                                                     <targets>
                                                     </targets>
                                                 </drug>
                                                 <drug>
                                                     <drugId>rifampicin</drugId>
                                                     <activePrinciple>something</activePrinciple>
                                                     <brandName>somebrand</brandName>
                                                     <atc>something</atc>
                                                     <treatment>
                                                         <dosageHistory>
                                                         </dosageHistory>
                                                     </treatment>
                                                     <samples>
                                                     </samples>
                                                     <targets>
                                                     </targets>
                                                 </drug>
                                             </drugs>
                                         </drugTreatment>
                                         <requests>
                                             <xpertRequest>
                                                 <drugId>imatinib</drugId>
                                                 <configId>imatinib.gotta2012</configId>
                                                 <output>
                                                     <format>xml</format>
                                                     <language>en</language>
                                                 </output>
                                             </xpertRequest>
                                             <xpertRequest>
                                                 <drugId>rifampicin</drugId>
                                                 <configId>imatinib.gotta2012</configId>
                                                 <output>
                                                     <format>xml</format>
                                                     <language>en</language>
                                                 </output>
                                             </xpertRequest>
                                         </requests>
                                     </query>)";

private:
};

/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Test cases for the XpertQueryResult::getRequestIndexBeingProcessed method.
///        Check that the initial request index is -1.
TEST_F(XpertQueryResultAdditionalTest, InitialRequestIndex)
{
    unique_ptr<XpertQueryData> query = nullptr;
    XpertQueryImport importer;
    XpertQueryImport::Status status = importer.importFromString(query, queryStringSingleRequest);
    ASSERT_EQ(status, XpertQueryImport::Status::Ok);

    XpertQueryResult result(std::move(query), "");

    EXPECT_EQ(result.getRequestIndexBeingProcessed(), -1);
}

/// \brief Test cases for the XpertQueryResult::incrementRequestIndexBeingProcessed method.
///        Check that the request index is correctly incremented.
TEST_F(XpertQueryResultAdditionalTest, RequestIndexIncrement)
{
    unique_ptr<XpertQueryData> query = nullptr;
    XpertQueryImport importer;
    XpertQueryImport::Status status = importer.importFromString(query, queryStringSingleRequest);
    ASSERT_EQ(status, XpertQueryImport::Status::Ok);

    XpertQueryResult result(std::move(query), "");

    EXPECT_EQ(result.incrementRequestIndexBeingProcessed(), 0);
    EXPECT_EQ(result.incrementRequestIndexBeingProcessed(), 1);
    EXPECT_EQ(result.incrementRequestIndexBeingProcessed(), 2);
    EXPECT_EQ(result.getRequestIndexBeingProcessed(), 2);
}

/// \brief Test cases for the XpertQueryResult::getOutputPath method.
///        Check that the output path is correctly stored and retrieved.
TEST_F(XpertQueryResultAdditionalTest, OutputPathGetter)
{
    unique_ptr<XpertQueryData> query = nullptr;
    XpertQueryImport importer;
    XpertQueryImport::Status status = importer.importFromString(query, queryStringSingleRequest);
    ASSERT_EQ(status, XpertQueryImport::Status::Ok);

    XpertQueryResult result(std::move(query), "/tmp/output");

    EXPECT_EQ(result.getOutputPath(), "/tmp/output");
}

/// \brief Test cases for the XpertQueryResult::getXpertRequestResults method.
///        Check that the correct number of XpertRequestResult objects are created.
TEST_F(XpertQueryResultAdditionalTest, MultipleRequestResults)
{
    unique_ptr<XpertQueryData> query = nullptr;
    XpertQueryImport importer;
    XpertQueryImport::Status status = importer.importFromString(query, queryStringMultipleRequests);
    ASSERT_EQ(status, XpertQueryImport::Status::Ok);

    XpertQueryResult result(std::move(query), "");

    EXPECT_EQ(result.getXpertRequestResults().size(), 2);
}

/// \brief Test cases for the XpertQueryResult constructor with template parameters.
///        Check that template-related getters return the correct values.
TEST_F(XpertQueryResultAdditionalTest, TemplateParameters)
{
    unique_ptr<XpertQueryData> query = nullptr;
    XpertQueryImport importer;
    XpertQueryImport::Status status = importer.importFromString(query, queryStringSingleRequest);
    ASSERT_EQ(status, XpertQueryImport::Status::Ok);

    XpertQueryResult result(std::move(query), "/tmp/output", "default_template", "/path/to/templates", true);

    EXPECT_EQ(result.getOutputPath(), "/tmp/output");
    EXPECT_EQ(result.getTemplateName(), "default_template");
    EXPECT_EQ(result.getTemplatePath(), "/path/to/templates");
    EXPECT_EQ(result.getJsonDumpEnabled(), true);
}

/// \brief Test cases for the XpertQueryResult constructor with default template parameters.
///        Check that default template parameters are correctly initialized.
TEST_F(XpertQueryResultAdditionalTest, DefaultTemplateParameters)
{
    unique_ptr<XpertQueryData> query = nullptr;
    XpertQueryImport importer;
    XpertQueryImport::Status status = importer.importFromString(query, queryStringSingleRequest);
    ASSERT_EQ(status, XpertQueryImport::Status::Ok);

    XpertQueryResult result(std::move(query), "/tmp/output");

    EXPECT_EQ(result.getOutputPath(), "/tmp/output");
    EXPECT_EQ(result.getJsonDumpEnabled(), false);
}

} // namespace Xpert
} // namespace Tucuxi
