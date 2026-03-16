// SPDX-License-Identifier: AGPL-3.0-or-later
#include "languagemanager.h"

#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/xmlnode.h"

#include "tucucdss-core/language/languageexception.h"

using namespace std;

namespace Tucuxi {
namespace Xpert {

const std::string LanguageManager::DEFAULT_TRANSLATION = "unknown translation";

LanguageManager::LanguageManager(){};

LanguageManager& LanguageManager::getInstance()
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    static unique_ptr<LanguageManager> upInstance = std::make_unique<LanguageManager>();
    return *upInstance;
}

std::string LanguageManager::translate(const std::string& _key) const
{
    // Try to find the key.
    auto it = m_keyToTranslation.find(_key);

    // Key found!
    if (it != m_keyToTranslation.end()) {
        return it->second;
    }
    // Key not found!
    return DEFAULT_TRANSLATION;
}

void LanguageManager::loadTranslations(const std::string& _xmlString)
{
    m_keyToTranslation.clear();
    Common::XmlDocument document;

    if (_xmlString.empty() || !document.fromString(_xmlString)) {
        throw LanguageException("Error importing language file. It may be badly formatted.");
    }

    Common::XmlNode root = document.getRoot();
    Common::XmlNodeIterator childrenIt = root.getChildren();


    // Iterating over child elements of root.
    while (childrenIt != Common::XmlNodeIterator::none()) {

        // Check if the name of the element is correct.
        if (childrenIt->getName() != "translation") {
            throw LanguageException("Element name must be \"translation\".");
        }

        // Check if only attribute is named "key".
        Common::XmlAttributeIterator attributeIt = childrenIt->getAttributes();

        if (attributeIt == Common::XmlAttributeIterator::none()) {
            throw LanguageException("Translation element needs one attribute.");
        }

        while (attributeIt != Common::XmlAttributeIterator::none()) {
            if (attributeIt->getName() != "key") {
                throw LanguageException("Only one attribute named \"key\" expected.");
            }
            attributeIt++;
        }

        m_keyToTranslation[childrenIt->getAttribute("key").getValue()] = childrenIt->getValue();
        childrenIt++;
    }
}


} // namespace Xpert
} // namespace Tucuxi
