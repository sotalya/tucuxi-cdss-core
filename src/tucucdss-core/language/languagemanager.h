// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace Tucuxi {
namespace Xpert {

/// \brief The language manager class is a singleton that loads an xml translations
///        file and allows to get some translation strings with unique identifiers.
class LanguageManager
{
public:
    /// \brief Get the unique instance of LanguageManager.
    /// \return The language manager unique instance.
    static LanguageManager& getInstance();

    /// \brief Get the translation of a given key.
    /// \param _key Key to look for translation.
    /// \return Translated string.
    [[nodiscard]] std::string translate(const std::string& _key) const;

    /// \brief Load an xml translations file from an xml string.
    ///        Each call clears and reloads the map that stores the translations.
    /// \param _xmlString Translations xml string.
    /// \throw LanguageException If the xml string could not be imported.
    void loadTranslations(const std::string& _xmlString);

private:
    /// \brief Constructor. Used internally to create the singleton instance.
    LanguageManager();

    /// \brief Singleton should not be clonable.
    LanguageManager(LanguageManager& _other) = delete;

    /// \brief Singleton should not be assignable.
    void operator=(const LanguageManager& _other) = delete;

public:
    /// \brief String returned when a key is not found when using "translate" method.
    static const std::string DEFAULT_TRANSLATION;

private:
    /// \brief Map containing the key to translation mapping.
    std::map<std::string, std::string> m_keyToTranslation;

    // Otherwise singleton does not work.
    friend std::unique_ptr<LanguageManager> std::make_unique<LanguageManager>();
};

} // namespace Xpert
} // namespace Tucuxi

#endif // LANGUAGEMANAGER_H
