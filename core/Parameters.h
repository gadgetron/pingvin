#pragma once

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/program_options.hpp>


namespace std {

/** TODO: This pollutes the `std` namespace...
 *
 * It is used to enable setting a `default_value` on multitoken parameters (i.e. a vector of numbers)
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    for (auto item : vec) {
        os << item << " ";
    }
    return os;
}

}


namespace Gadgetron::Core {

namespace po = boost::program_options;

struct IParameter {
    virtual boost::shared_ptr<po::option_description> as_boost_option(void) const = 0;
    virtual ~IParameter() = default;
};

template <typename T>
class Parameter : public IParameter {
public:
    Parameter(const std::string& name, T* storage, const std::string& description)
        : storage_(storage), name_(name), description_(description) {}

    boost::shared_ptr<po::option_description> as_boost_option(void) const override
    {
        return boost::make_shared<po::option_description>(
            name_.c_str(),
            po::value<T>(storage_)->default_value(*storage_),
            description_.c_str()
        );
    }

protected:
    T* storage_;
    std::string name_;
    std::string description_;
};

template <typename T>
class MultitokenParameter : public IParameter {
public:
    MultitokenParameter(const std::string& name, std::vector<T>* storage, const std::string& description)
        : storage_(storage), name_(name), description_(description) {}

    boost::shared_ptr<po::option_description> as_boost_option(void) const override
    {
        return boost::make_shared<po::option_description>(
            name_.c_str(),
            po::value<std::vector<T>>(storage_)->default_value(*storage_)->multitoken(),
            description_.c_str()
        );
    }

protected:
    std::vector<T>* storage_;
    std::string name_;
    std::string description_;
};

class Flag : public Parameter<bool> {
public:
    using Parameter::Parameter;

    boost::shared_ptr<po::option_description> as_boost_option(void) const override
    {
        return boost::make_shared<po::option_description>(
            name_.c_str(),
            po::bool_switch(this->storage_)->default_value(*this->storage_),
            description_.c_str()
        );
    }
};

class NodeParameters {
public:
    NodeParameters(const std::string& prefix, const std::string& description): prefix_(prefix), description_(description) {}
    NodeParameters(const std::string& prefix): prefix_(prefix) {}
    NodeParameters(): prefix_("unknown") {};

    template <typename T>
    void register_parameter(const std::string& name, T* value, const std::string& description) {
        auto argname = prefix_ + "." + name;
        parameters_.push_back(std::make_shared<Parameter<T>>(argname, value, description));
    }

    template <typename T>
    void register_multitoken(const std::string& name, std::vector<T>* value, const std::string& description) {
        auto argname = prefix_ + "." + name;
        parameters_.push_back(std::make_shared<MultitokenParameter<T>>(argname, value, description));
    }

    void register_flag(const std::string& name, bool* value, const std::string& description) {
        auto argname = prefix_ + "." + name;
        parameters_.push_back(std::make_shared<Flag>(argname, value, description));
    }

    const std::vector<std::shared_ptr<IParameter>>& parameters() const {
        return parameters_;
    }

    std::string description(void) const {
        return description_;
    }

private:
    std::string prefix_;
    std::string description_;
    std::vector<std::shared_ptr<IParameter>> parameters_;
};


} // namespace Gadgetron::Core