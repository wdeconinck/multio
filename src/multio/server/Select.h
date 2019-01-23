#ifndef multio_server_Select_H
#define multio_server_Select_H

#include "Action.h"

namespace multio {
namespace server {

class Message;

class Select : public Action {
public:
    explicit Select(const std::string& plan_name, const std::string& nm = "Select");

private:  // methods
    bool doExecute(std::shared_ptr<Message> msg) const override;

    bool matchPlan(const Message& msg) const;

private:  // members
    std::string plan_name_;
};

}  // namespace server
}  // namespace multio

#endif
