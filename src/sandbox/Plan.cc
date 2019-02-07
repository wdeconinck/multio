/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef multio_sandbox_Listener_H
#define multio_sandbox_Listener_H

#include "Plan.h"

#include "eckit/log/Log.h"
#include "eckit/config/Configuration.h"
#include "eckit/config/LocalConfiguration.h"

#include "sandbox/Action.h"

using eckit::LocalConfiguration;

namespace multio {
namespace sandbox {

Plan::Plan(const eckit::Configuration& config) {

    const LocalConfiguration cfg = config.getSubConfiguration("root");
    eckit::Log::info() << cfg << std::endl;
    root_.reset(ActionFactory::instance().build(cfg.getString("type"), cfg));
    eckit::Log::info() << "    Returning from plan constructor" << std::endl;

//    const std::vector<LocalConfiguration> configs = config.getSubConfigurations("actions");
//    for (const auto& cfg : configs) {
//    }
}

Plan::~Plan() = default;

void Plan::process(Message msg)
{
    root_->execute(msg);
}

}  // namespace sandbox
}  // namespace multio

#endif
