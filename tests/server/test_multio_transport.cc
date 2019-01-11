
#include "eckit/testing/Test.h"

#include "atlas/field/Field.h"

#include "multio/server/PartialMapping.h"
#include "multio/server/Message.h"
#include "multio/server/MpiTransport.h"
#include "multio/server/print_buffer.h"
#include "multio/server/SerialisationHelpers.h"

#include "TestServerHelpers.h"

using namespace eckit::testing;

namespace multio {
namespace server {
namespace test {

namespace {
const auto nServers = 1u;
const Transport& transport = MpiTransport{"transport test", nServers};
}  // namespace

CASE("Test that MPI transport layer") {
    SECTION(" is created correctly") {
        std::ostringstream oss;
        oss << transport;

        EXPECT(oss.str() == "MpiTransport[transport test]");
    }

    SECTION(" reads and writes correctly") {
        ASSERT(transport.size() == 2);

        auto test_plan = PartialMapping{"test_field", {7, 23, 43, 91}};
        auto test_field = set_up_atlas_test_field("temperature");

        if (transport.client()) {
            auto dest = 1;

            // Send plan
            Message msg(0, dest, msg_tag::plan_data);
            local_plan_to_message(test_plan, msg);
            transport.send(msg);

            EXPECT(msg.tag() == msg_tag::plan_data);

            // Send field
            msg.reset(0, dest, msg_tag::field_data);
            atlas_field_to_message(test_field, msg);
            transport.send(msg);

            EXPECT(msg.tag() == msg_tag::field_data);
            EXPECT(msg.size() == 273u);
        } else {
            EXPECT(transport.server());

            // Receive plan
            Message msg;
            transport.receive(msg);
            EXPECT(msg.tag() == msg_tag::plan_data);

            auto received_plan = unpack_local_plan(msg);
            EXPECT(test_plan == received_plan);

            // Receive field
            msg.reset(0, -1, msg_tag::plan_data);
            transport.receive(msg);
            EXPECT(msg.tag() == msg_tag::field_data);

            auto received_field = unpack_atlas_field(msg);

            std::ostringstream expected;
            print_atlas_info(test_field, expected);

            std::ostringstream actual;
            print_atlas_info(received_field, actual);

            EXPECT(expected.str() == actual.str());
        }
    }
}

}  // namespace server
}  // namespace test
}  // namespace multio

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
