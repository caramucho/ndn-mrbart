#ifndef DASH_PARAMETERS_H
#define DASH_PARAMETERS_H

#define NDN_PAYLOAD_SIZE 8000
#define INIT_BITRATE 45000
#define MIN_SEGMENT_ID 10
#define MAX_SEGMENT_ID 100
#define SCENARIOTIME 10000
#define CONTENT_NUMBER 26
#define CONTENT_NUMBER_STR "26"
#define CACHE_SIZE 10000
#define MEAN_PARAMETER 1.0
#define DASH_CLIENT_TYPE "ns3::ndn::RaahsClient"

// uint32_t rates[] =
// /*  { 13281, 18593, 26030, 36443, 51020, 71428, 100000, 140000, 195999,
//  274399, 384159, 537823 };*/
//   { 45000, 89000, 131000, 178000, 221000, 263000, 334000, 396000, 522000,
//       595000, 791000, 1033000, 1245000, 1547000, 2134000, 2484000, 3079000,
//       3527000, 3840000, 4220000 };

// "ns3::ndn::RaahsClient"
// "ns3::ndn::SvaaClient"
// "ns3::ndn::SftmClient"
// "ns3::ndn::OsmpClient"
// "ns3::ndn::FdashClient"
// "ns3::ndn::AaashClient"

#endif
