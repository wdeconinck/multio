/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Domokos Sarmany
/// @author Simon Smart
/// @author Tiago Quintino

/// @date Jan 2019

#ifndef multio_server_actions_Null_H
#define multio_server_actions_Null_H

#include "multio/server/Action.h"

namespace eckit {
class Configuration;
}

namespace multio {
namespace server {
namespace actions {

class Null : public Action {
public:
    Null(const eckit::Configuration& config);

private:
    bool doExecute(Message& msg) const override;

    void print(std::ostream& os) const override;
};

}  // namespace actions
}  // namespace server
}  // namespace multio

#endif
