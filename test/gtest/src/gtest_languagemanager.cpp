// SPDX-License-Identifier: AGPL-3.0-or-later
#include <gtest/gtest.h>

#include "testutils.h"
#include "tucucdss-core/language/languageexception.h"
#include "tucucdss-core/language/languagemanager.h" // Source file to be tested


/// \brief Gtest of languagemanager.
using namespace std;

namespace Tucuxi {
namespace Xpert {

/***********************************************************
 *                  Defined functions for the tests          *
 ***********************************************************/


// translate fixture
class TranslaterTest : public testing::Test
{
public:
    LanguageManager& CreateValidLanguageManager()
    {


        std::string goodString = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <translations
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="translations_file.xsd">

                                        <translation key="hello">Hello</translation>
                                        <translation key="world">World</translation>
                                        <translation key="xpert">Xpert</translation>

                                    </translations>)";

        LanguageManager& languageManager = LanguageManager::getInstance();
        languageManager.loadTranslations(goodString);

        return languageManager;
    }



private:
};

// loadTranslations fixture
class TranslationsLoaderTest : public testing::Test
{
public:
    LanguageManager& languageManager = LanguageManager::getInstance();

    std::string goodString = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <translations
                                        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                        xsi:noNamespaceSchemaLocation="translations_file.xsd">

                                        <translation key="hello">Hello</translation>
                                        <translation key="world">World</translation>

                                    </translations>)";

    std::string missSpelledTranslationsString = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                                       <translations
                                                           xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                                           xsi:noNamespaceSchemaLocation="translations_file.xsd">

                                                           <translation key="hello">Hello</translation>
                                                           <abc key="world">World</abc>

                                                       </translations>)";

    std::string noKeyAttributeString = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                              <translations
                                                  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                                  xsi:noNamespaceSchemaLocation="translations_file.xsd">

                                                  <translation>Hello</translation>

                                              </translations>)";

    std::string badKeyAttributeString = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                               <translations
                                                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                                   xsi:noNamespaceSchemaLocation="translations_file.xsd">

                                                   <translation yek="hello">Hello</translation>

                                               </translations>)";

    std::string nestedElement = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                       <translations
                                           xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                                           xsi:noNamespaceSchemaLocation="translations_file.xsd">

                                           <translation yek="hello">Hello</translation>
                                           <translation key="nested">
                                               <nested> a value </nested>
                                           </translation>

                                       </translations>)";



private:
};



/***********************************************************
 *                  Tests                                  *
 ***********************************************************/

/// \brief Checks that translate method behaves correctly.
///        If the key is known, the translation is returned.
///        If the key is unknown, the default translation is returned.
/// \param none but uses the related fixture
TEST_F(TranslaterTest, ValidTranslation)
{
    LanguageManager& languageManager = CreateValidLanguageManager();

    std::string key = "xpert";
    std::string expected = "Xpert";

    // Test translate
    // xpert key exists and returns Xpert
    EXPECT_EQ(expected, languageManager.translate(key));
}

TEST_F(TranslaterTest, EmptyTranslation)
{
    LanguageManager& languageManager = CreateValidLanguageManager();

    std::string key = "unknown key";
    std::string expected = LanguageManager::DEFAULT_TRANSLATION;

    // Test translate
    // "unknown key" is not part of test.xml
    EXPECT_EQ(expected, languageManager.translate(key));
}

/// \brief Check that the loadTranslations method behaves as expected.
///        If the xml is not well formatted, the loading must throw a languageException.
/// \param none but uses the related fixture
TEST_F(TranslationsLoaderTest, ValidTranslationsLoader)
{
    // Check that translate method does not throw an exception
    ASSERT_NO_THROW(languageManager.loadTranslations(goodString));
}
TEST_F(TranslationsLoaderTest, NestedElementsTranslationsLoader)
{
    // LanguageException expected;
    // Check that translate method does throw an exception
    ASSERT_THROW(languageManager.loadTranslations(nestedElement), Xpert::LanguageException);
}
TEST_F(TranslationsLoaderTest, BadKeyTranslationsLoader)
{
    // LanguageException expected;
    // Check that translate method does throw an exception
    ASSERT_THROW(languageManager.loadTranslations(badKeyAttributeString), Xpert::LanguageException);
}
TEST_F(TranslationsLoaderTest, NoKeyTranslationsLoader)
{
    // LanguageException expected;
    // Check that translate method does throw an exception
    ASSERT_THROW(languageManager.loadTranslations(noKeyAttributeString), Xpert::LanguageException);
}
TEST_F(TranslationsLoaderTest, MissSpelledTranslationsLoader)
{
    // LanguageException expected;
    // Check that translate method does throw an exception
    ASSERT_THROW(languageManager.loadTranslations(missSpelledTranslationsString), Xpert::LanguageException);
}

TEST_F(TranslationsLoaderTest, EmptyTranslationsLoader)
{
    // LanguageException expected;
    // Check that translate method does throw an exception
    ASSERT_THROW(languageManager.loadTranslations(""), Xpert::LanguageException);
}

TEST(LanguageExceptionTest, StringConstructor)
{
    std::string message = "This is a language exception.";
    LanguageException exception(message);
    EXPECT_EQ(message, exception.what());
}

TEST(LanguageExceptionTest, CharArrayConstructor)
{
    const char* message = "This is a language exception.";
    LanguageException exception(message);
    EXPECT_EQ(std::strcmp(message, exception.what()), 0);
}

} // namespace Xpert
} // namespace Tucuxi
