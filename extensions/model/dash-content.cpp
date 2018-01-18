#include "dash-content.hpp"


// #include <ns3/random-variable-stream.h>
// #include <ns3/tcp-socket.h>
// #include <ns3/double.h>

namespace ns3
{
  namespace ndn
  {
  NS_LOG_COMPONENT_DEFINE("DashContent");
  NS_OBJECT_ENSURE_REGISTERED(DashContent);

  TypeId
  DashContent::GetTypeId(void)
  {
    static TypeId tid =
        TypeId("ns3::ndn::DashContent")
        .SetGroupName("Ndn")
        .AddConstructor<DashContent>();
        // .AddAttribute("DashContentPayloadSize", "payload size for Content packets", UintegerValue(8000),MakeUintegerAccessor(&DashContent::m_payloadSize),MakeUintegerChecker<uint32_t>());
    return tid;
  }

  DashContent::DashContent():
  m_payloadSize(NDN_PAYLOAD_SIZE)
  {
    NS_LOG_FUNCTION(this);
    // m_totalRx = 0;
  }

  DashContent::~DashContent()
  {
    NS_LOG_FUNCTION(this);
  }

  void
  DashContent::MakeSegment(uint32_t video_id, uint32_t resolution,
      uint32_t segment_id)
  {
    int avg_packetsize = resolution / (50 * 8);

    // HTTPHeader http_header_tmp;
    MPEGHeader mpeg_header_tmp;

    Ptr<UniformRandomVariable> frame_size_gen = CreateObject<UniformRandomVariable> ();

    frame_size_gen->SetAttribute ("Min", DoubleValue (0));
    frame_size_gen->SetAttribute ("Max", DoubleValue (
        std::max(
            std::min(2 * avg_packetsize, MPEG_MAX_MESSAGE)
                - (int) (mpeg_header_tmp.GetSerializedSize()
                    // + http_header_tmp.GetSerializedSize()
                  ), 1)));

    for (uint32_t f_id = 0; f_id < MPEG_FRAMES_PER_SEGMENT; f_id++)
      {
        uint32_t frame_size = (unsigned) frame_size_gen->GetValue();

        // HTTPHeader http_header;
        // http_header.SetMessageType(HTTP_RESPONSE);
        // http_header.SetVideoId(video_id);
        // http_header.SetResolution(resolution);
        // http_header.SetSegmentId(segment_id);

        MPEGHeader mpeg_header;
        mpeg_header.SetFrameId(f_id);
        mpeg_header.SetPlaybackTime(
            MilliSeconds(
                (f_id + (segment_id * MPEG_FRAMES_PER_SEGMENT))
                    * MPEG_TIME_BETWEEN_FRAMES)); //50 fps
        mpeg_header.SetType('B');
        mpeg_header.SetSize(frame_size);

        Ptr<Packet> frame = Create<Packet>(frame_size);
        // frame->AddHeader(http_header);
        frame->AddHeader(mpeg_header);
        NS_LOG_INFO(
            "SENDING PACKET " << f_id << " " << frame->GetSize() << " res=" << http_header.GetResolution() << " size=" << mpeg_header.GetSize() << " avg=" << avg_packetsize);

        m_queues.push(*frame);
      }
    // DataSend(socket, 0);
  }

} // Namespace ndn
} // Namespace ns3
