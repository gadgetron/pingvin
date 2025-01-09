#pragma once

#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

namespace pingvin {

class Gadget {
  public:
    Gadget(std::string name, std::string description) : name_(name), description_(description) {
     std::cerr << "Constructed Gadget " << name << std::endl;
    }

    virtual ~Gadget() = default;

    virtual void process() = 0;

    virtual void install_cli(po::options_description&) = 0;

    std::string name() const { return name_; }
    std::string description() const { return description_; }

protected:
    std::string name_;
    std::string description_;
};

} // namespace pingvin