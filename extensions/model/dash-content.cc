
#include "dash-content.h"


NS_LOG_COMPONENT_DEFINE("DashContent");

namespace ns3{
  namespace ndn{
    DashContent::DashContent() :
    m_payloadSize(8000)
    {
    }
    DashContent::~DashContent()
    {

    }
    Data
    DashContent::getDataPacket(DashName dashname, uint32_t seq)
    {
      uint32_t representation = dashname.GetRepresentation();

      auto itr = m_map.find(representation);
      if( itr != m_map.end() ){
        return itr->second[seq];
      }else{
        MakeDataPacket(representation);
        return getDataPacket(dashname, seq);
      }

    }

    void
    DashContent::MakeDataPacket(uint32_t representation){


      int avg_packetsize = representation / (50 * 8);

      MPEGHeader mpeg_header_tmp;

      // Ptr<UniformRandomVariable> frame_size_gen = CreateObject<UniformRandomVariable> ();
      //
      // frame_size_gen->SetAttribute ("Min", DoubleValue (0));
      // frame_size_gen->SetAttribute ("Max", DoubleValue (
      //     std::max(
      //         std::min(2 * avg_packetsize, MPEG_MAX_MESSAGE)
      //             - (int) (mpeg_header_tmp.GetSerializedSize()
      //                 + http_header_tmp.GetSerializedSize()), 1)));
      uint32_t frame_size = std::max(
                        std::min(avg_packetsize, MPEG_MAX_MESSAGE) - (int) (mpeg_header_tmp.GetSerializedSize()) ,
                        1);
      uint32_t bytes = 0;
      for (uint32_t f_id = 0; f_id < MPEG_FRAMES_PER_SEGMENT; f_id++)
        {
          // uint32_t frame_size = (unsigned) frame_size_gen->GetValue();

          // DashName dashname;
          //
          // dashname.SetVideoId(m_videoId);
          // dashname.SetRepresentation(representation);
          // dashname.SetSegmentId(segment_id);

          MPEGHeader mpeg_header;
          mpeg_header.SetFrameId(f_id);
          mpeg_header.SetPlaybackTime(
              MilliSeconds(
                  (f_id + (m_segmentId * MPEG_FRAMES_PER_SEGMENT))
                      * MPEG_TIME_BETWEEN_FRAMES)); //50 fps
          mpeg_header.SetType('B');
          mpeg_header.SetSize(frame_size);

          Ptr<Packet> frame = Create<Packet>(frame_size);
          // frame->AddHeader(http_header);
          frame->AddHeader(mpeg_header);
          // NS_LOG_INFO(
          //     "SENDING PACKET " << f_id << " " << frame->GetSize() << " res=" << http_header.GetResolution() << " size=" << mpeg_header.GetSize() << " avg=" << avg_packetsize);
          if (frame->Serialize(&m_buffer[bytes], (MPEG_MAX_MESSAGE * MPEG_FRAMES_PER_SEGMENT - bytes))) {
            bytes += frame->GetSize();
          }

        }
        cout << bytes << endl;
        Data
      }


  }
}
