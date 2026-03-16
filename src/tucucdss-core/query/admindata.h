// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef ADMINDATA_H
#define ADMINDATA_H

/**************************************************
 |      Basically this class is a copypasta       |
 |    of Tucuquery::Query::AdministrativeData,    |
 |           but with const refs, comments.       |
 |            and more consistant naming          |
 **************************************************/

#include <map>
#include <memory>
#include <optional>
#include <string>

namespace Tucuxi {
namespace Xpert {

/// \brief Class that represents an address of an institute or a person.
///        It contains the information of the "address" element from a query.
class AddressData
{
public:
    /// \brief Address constructor.
    /// \param _street Name of the street of the address.
    /// \param _postCode Post code of the address.
    /// \param _city Name of the city of the address.
    /// \param _state Name of the state of the address.
    /// \param _country Name of the country of the address.
    AddressData(
            std::string _street, std::string _postCode, std::string _city, std::string _state, std::string _country);

    // Getters

    /// \brief Get the street.
    /// \return The name of the street as a string.
    [[nodiscard]] std::string getStreet() const;

    /// \brief Get the postal code.
    /// \return The postal code as a string.
    [[nodiscard]] std::string getPostalCode() const;

    /// \brief Get the city.
    /// \return The name of the city as a string.
    [[nodiscard]] std::string getCity() const;

    /// \brief Get the state.
    /// \return The name of the state as a string.
    [[nodiscard]] std::string getState() const;

    /// \brief Gets the country.
    /// \return The name of the country as a string.
    [[nodiscard]] std::string getCountry() const;

protected:
    /// \brief Street of the address.
    std::string m_street;

    /// \brief Postal code of the address.
    std::string m_postalCode;

    /// \brief City of the address.
    std::string m_city;

    /// \brief State of the address.
    std::string m_state;

    /// \brief Country of the address.
    std::string m_country;
};

/// \brief Class that represents a phone number of an institute or a person.
///        It contains the information of the "phone" element from a query.
class PhoneData
{
public:
    /// \brief Phone constructor.
    /// \param _number Phone number.
    /// \param _type Phone type.
    PhoneData(std::string _number, std::string _type);

    // Getters

    /// \brief Get the phone number.
    /// \return The phone number as a string.
    [[nodiscard]] std::string getNumber() const;

    /// \brief Get the phone number type.
    /// \return The phone number type string.
    [[nodiscard]] std::string getType() const;

protected:
    /// \brief Phone number.
    std::string m_number;

    /// \brief Phone number type.
    std::string m_type;
};

/// \brief Class that represents an email address of an institute or a person.
///        It contains the information of the "email" element from a query.
class EmailData
{
public:
    /// \brief Email constructor.
    /// \param _address Email address.
    /// \param _type Email address type.
    EmailData(std::string _address, std::string _type);

    // Getters

    /// \brief Get the email address.
    /// \return The email address as a string.
    [[nodiscard]] std::string getAddress() const;

    /// \brief Get the email address type.
    /// \return The email address type as a string.
    [[nodiscard]] std::string getType() const;

protected:
    /// \brief Email address.
    std::string m_address;

    /// \brief Email address type.
    std::string m_type;
};

/// \brief Class that represents a person and their contact information.
///        It contains the information of the "person" element from a query.
class PersonData
{
public:
    /// \brief PersonData constructor.
    /// \param _id Person's identifier.
    /// \param _title Person's title.
    /// \param _firstName Person's first name.
    /// \param _lastName Person's last name.
    /// \param _address Person's address.
    /// \param _phone Person's phone number.
    /// \param _email Person's email address.
    /// \param _birthdate Person's birthdate.
    PersonData(
            std::string _id,
            std::string _title,
            std::string _firstName,
            std::string _lastName,
            std::unique_ptr<AddressData> _address,
            std::unique_ptr<PhoneData> _phone,
            std::unique_ptr<EmailData> _email,
            std::optional<std::string> _birthdate);

    /// \brief Copy constructor is not supported because of unique pointers.
    PersonData(PersonData& _other) = delete;

    // Getters

    /// \brief Get the person's identifier.
    /// \return The person's identifier string.
    [[nodiscard]] std::string getId() const;

    /// \brief Get the person's title.
    /// \return The person's title string.
    [[nodiscard]] std::string getTitle() const;

    /// \brief Get the person's first name.
    /// \return The person's first name string.
    [[nodiscard]] std::string getFirstName() const;

    /// \brief Get the person's last name.
    /// \return The person's last name string.
    [[nodiscard]] std::string getLastName() const;

    [[nodiscard]] std::optional<std::string> getBirthdate() const;

    /// \brief Get the person's address.
    /// \return The person's address. May be nullptr.
    [[nodiscard]] const AddressData* getAddress() const;

    /// \brief Get the person's phone number.
    /// \return The person's phone number. May be nullptr.
    [[nodiscard]] const PhoneData* getPhone() const;

    /// \brief Get the person's email.
    /// \return The person's email. May be nullptr.
    [[nodiscard]] const EmailData* getEmail() const;

protected:
    /// \brief Person's identifier.
    std::string m_id;

    /// \brief Person's title.
    std::string m_title;

    /// \brief Person's first name.
    std::string m_firstName;

    /// \brief Person's last name.
    std::string m_lastName;

    /// \brief Person's birthday.
    std::optional<std::string> m_birthdate;

    /// \brief Person's address. May be nullptr.
    std::unique_ptr<AddressData> m_address;

    /// \brief Person's phone number. May be nullptr.
    std::unique_ptr<PhoneData> m_phone;

    /// \brief Person's email. May be nullptr.
    std::unique_ptr<EmailData> m_email;
};

/// \brief Class that represents an institute and its contact information.
///        It contains the information of the "institute" element from a query.
class InstituteData
{
public:
    /// \brief InstituteData constructor.
    /// \param _id Identifier of the institute.
    /// \param _name Name of the institute.
    /// \param _address Address of the institute.
    /// \param _phone Phone number of the institute.
    /// \param _email Email address of the institute.
    InstituteData(
            std::string _id,
            std::string _name,
            std::unique_ptr<AddressData> _address,
            std::unique_ptr<PhoneData> _phone,
            std::unique_ptr<EmailData> _email);

    /// \brief Copy constructor is not supported because of unique pointers.
    InstituteData(InstituteData& _other) = delete;

    // Getters

    /// \brief Get the identifier of the institute.
    /// \return The identifier string of the institute.
    [[nodiscard]] std::string getId() const;

    /// \brief Get the name of the institute.
    /// \return The name string of the institute.
    [[nodiscard]] std::string getName() const;

    /// \brief Get the address of the institute.
    /// \return The address of the institute. May be nullptr.
    [[nodiscard]] const AddressData* getAddress() const;

    /// \brief Get the phone of the institute.
    /// \return The phone of the institute. May be nullptr.
    [[nodiscard]] const PhoneData* getPhone() const;

    /// \brief Get the email of the institute.
    /// \return The email of the institute. May be nullptr.
    [[nodiscard]] const EmailData* getEmail() const;

protected:
    /// \brief Identifier of the institute.
    std::string m_id;

    /// \brief Name of the institute.
    std::string m_name;

    /// \brief Address of the institute. May be nullptr.
    std::unique_ptr<AddressData> m_address;

    /// \brief Phone of the institute. May be nullptr.
    std::unique_ptr<PhoneData> m_phone;

    /// \brief Email of the institute. May be nullptr.
    std::unique_ptr<EmailData> m_email;
};

/// \brief This class represents a full person that has contact information and
///        can be affiliated with an institute.
///        It contains the information of the "mandator" or "patient" element from a query.
class FullPersonData
{
public:
    /// \brief FullPersonData constructor.
    /// \param _person Person's contact information.
    /// \param _institute contact information of the affiliated institute.
    FullPersonData(std::unique_ptr<PersonData> _person, std::unique_ptr<InstituteData> _institute);

    /// \brief Copy constructor is not supported because of unique pointers.
    FullPersonData(FullPersonData& _other) = delete;

    // Getters

    /// \brief Get the person's contact information.
    /// \return The person's contact information reference.
    [[nodiscard]] const PersonData& getPerson() const;

    /// \brief Get the contact information of the person's institute.
    /// \return The contact information of the person's institute. May be nullptr
    [[nodiscard]] const InstituteData* getInstitute() const;

protected:
    /// \brief P&erson's contact information.
    std::unique_ptr<PersonData> m_person;

    /// \brief Contact information of the person's institute. May be nullptr.
    std::unique_ptr<InstituteData> m_institute;
};

/// \brief Class allowing to store various information related to a patient.
///        It contains the information of the "clinicalDatas" element from a query.
class ClinicalDatas
{
public:
    /// \brief ClinicalDatas constructor.
    /// \param _data Map of data. Each entry corresponds to a clinicalData.
    ///              The keys correspond to the key attributes and the values to the
    ///              values of the clinicalData elements.
    ClinicalDatas(const std::map<std::string, std::string>& _data);

    // Getters

    /// \brief Get the clinical data. Each entry corresponds to a clinicalData.
    ///        The keys correspond to the key attributes and the values to the
    ///        values of the clinicalData elements.
    /// \return The map containing the clinical data.
    [[nodiscard]] const std::map<std::string, std::string>& getData() const;

protected:
    /// \brief Map of the clinical data.
    std::map<std::string, std::string> m_data;
};

/// \brief Wrapper for all admin data of a TuberXpert query.
class AdminData
{
public:
    /// \brief Constructor of admin data.
    /// \param _mandator Mandator information pointer.
    /// \param _patient Patient information pointer.
    /// \param _clinicalDatas Patient's clinical data information pointer.
    AdminData(
            std::unique_ptr<FullPersonData> _mandator,
            std::unique_ptr<FullPersonData> _patient,
            std::unique_ptr<ClinicalDatas> _clinicalDatas);

    /// \brief Copy constructor is not supported because of unique pointers.
    AdminData(AdminData& _other) = delete;

    // Getters

    /// \brief Get the mandator full person.
    /// \return The unique pointer of the mandator full person. May be nullptr.
    [[nodiscard]] const FullPersonData* getMandator() const;

    /// \brief Get the patient full person.
    /// \return The unique pointer of the patient full person. May be nullptr.
    [[nodiscard]] const FullPersonData* getPatient() const;

    /// \brief Get the clinical data.
    /// \return The clinical data unique pointer. May be nullptr.
    [[nodiscard]] const ClinicalDatas* getClinicalDatas() const;

protected:
    /// \brief The mandator. May be nullptr.
    std::unique_ptr<FullPersonData> m_mandator;

    /// \brief The patient. May be nullptr.
    std::unique_ptr<FullPersonData> m_patient;

    /// \brief The clinical data. May be nullptr.
    std::unique_ptr<ClinicalDatas> m_clinicalDatas;
};

} // namespace Xpert
} // namespace Tucuxi

#endif // ADMINDATA_H
