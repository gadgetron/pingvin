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
    IParameter(const std::string& prefix, const std::string& name, const std::string& description)
        : prefix_(prefix), shortname_(name), fullname_(prefix + "." + name), description_(description) {}
    virtual boost::shared_ptr<po::option_description> as_boost_option(void) const = 0;
    virtual void print(std::ostream&) const = 0;
    virtual bool modified(void) = 0;
    virtual ~IParameter() = default;

protected:
    std::string prefix_;
    std::string shortname_;
    std::string fullname_;
    std::string description_;
};

template <typename T>
class Parameter : public IParameter {
public:
    Parameter(const std::string& prefix, const std::string& name, const std::string& description, T* storage)
        : IParameter(prefix, name, description), storage_(storage), default_value_(*storage) {}

    boost::shared_ptr<po::option_description> as_boost_option(void) const override
    {
        return boost::make_shared<po::option_description>(
            fullname_.c_str(),
            po::value<T>(storage_)->default_value(default_value_),
            description_.c_str()
        );
    }

    void print(std::ostream& os) const override {
        os << shortname_ << " = " << *storage_;
    }

    bool modified(void) override {
        return *storage_ != default_value_;
    }

protected:
    T* storage_;
    T default_value_;
    std::string name_;
    std::string description_;
};

template <typename T>
class MultitokenParameter : public IParameter {
public:
    MultitokenParameter(const std::string& prefix, const std::string& name
            , const std::string& description, std::vector<T>* storage)
        : IParameter(prefix, name, description), storage_(storage), default_value_(*storage) {}

    boost::shared_ptr<po::option_description> as_boost_option(void) const override
    {
        return boost::make_shared<po::option_description>(
            fullname_.c_str(),
            po::value<std::vector<T>>(storage_)->default_value(*storage_)->multitoken(),
            description_.c_str()
        );
    }

    void print(std::ostream& os) const override {
        os << shortname_ << " = " << *storage_;
    }

    bool modified(void) override {
        return *storage_ != default_value_;
    }

protected:
    std::vector<T>* storage_;
    std::vector<T> default_value_;
};

class Flag : public Parameter<bool> {
public:
    using Parameter::Parameter;

    boost::shared_ptr<po::option_description> as_boost_option(void) const override
    {
        return boost::make_shared<po::option_description>(
            fullname_.c_str(),
            po::bool_switch(this->storage_)->default_value(this->default_value_),
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
        parameters_.push_back(std::make_shared<Parameter<T>>(prefix_, name, description, value));
    }

    template <typename T>
    void register_multitoken(const std::string& name, std::vector<T>* value, const std::string& description) {
        parameters_.push_back(std::make_shared<MultitokenParameter<T>>(prefix_, name, description, value));
    }

    void register_flag(const std::string& name, bool* value, const std::string& description) {
        parameters_.push_back(std::make_shared<Flag>(prefix_, name, description, value));
    }

    const std::vector<std::shared_ptr<IParameter>>& parameters() const {
        return parameters_;
    }

    std::string prefix(void) const { return prefix_; }
    std::string description(void) const { return description_; }

private:
    std::string prefix_;
    std::string description_;
    std::vector<std::shared_ptr<IParameter>> parameters_;
};


} // namespace Gadgetron::Core