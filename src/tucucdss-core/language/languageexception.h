// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef LANGUAGEEXCEPTION_H
#define LANGUAGEEXCEPTION_H

#include <stdexcept>

namespace Tucuxi {
namespace Xpert {

/// \brief Custom exception used for language problems.
class LanguageException : public std::runtime_error
{
public:
    /// \brief String constructor.
    /// \param _msg Message to embeed.
    explicit LanguageException(const std::string& _msg);

    /// \brief Character array constructor.
    /// \param _msg Message to embeed.
    explicit LanguageException(const char* _msg);
};

} // namespace Xpert
} // namespace Tucuxi

#endif // LANGUAGEEXCEPTION_H
