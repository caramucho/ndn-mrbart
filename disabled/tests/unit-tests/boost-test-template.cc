// #include "ns3/core-module.h"
// #include "ns3/network-module.h"
// #include "ns3/ndnSIM-module.h"
// #include <iostream>

// // #include "dash-client.h"
// #include "model/ndn-parser.h"
//
// using namespace ns3;
// using namespace ns3::ndn;
// using namespace std;
//
//
// int
// main(int argc, char *argv[])
// {
//
// }

// #include "encoding/buffer-stream.hpp"
#include "model/dash-name.h"
#include "boost-test.hpp"

namespace ndn {
namespace tests {
    BOOST_AUTO_TEST_SUITE(Dash)
    BOOST_AUTO_TEST_SUITE(TestDashName)

    BOOST_AUTO_TEST_CASE(FirstTest)
    {
      BOOST_CHECK_EQUAL(0, 0);
    }

    BOOST_AUTO_TEST_SUITE_END() // TestBufferStream
    BOOST_AUTO_TEST_SUITE_END() // Dash

}
}
