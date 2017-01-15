
#include "dash-content.h"
#include "../dash-parameters.h"

NS_LOG_COMPONENT_DEFINE("DashContent");

namespace ns3{
  namespace ndn{
    DashContent::DashContent() :
    m_payloadSize(NDN_PAYLOAD_SIZE)
    {
    }
    DashContent::~DashContent()
    {

    }
    Data
    DashContent::getDataPacket(uint32_t representation, uint32_t seq)
    {
      // uint32_t representation = dashname.GetRepresentation();

      auto itr = m_map.find(representation);
      if( itr != m_map.end() ){
        return itr->second[seq];
      }else{
        MakeDataPacket(representation);
        return getDataPacket(representation, seq);
      }
    }

    void
    DashContent::MakeDataPacket(uint32_t representation){


      int avg_packetsize = representation / (50 * 8);
      std::vector<Data> data_v;

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
      int bytes = 0;
      for (uint32_t f_id = 0; f_id < MPEG_FRAMES_PER_SEGMENT; f_id++)
        {
          // uint32_t frame_size = (unsigned) frame_size_gen->GetValue();



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
          // uint32_t headersize = mpeg_header.GetSerializedSize();
          // MPEGHeader mpeg_header2;
          // Packet headerPacket(&m_buffer[0], frame->GetSize());
          // headerPacket.RemoveHeader(mpeg_header2);
          // cout << "content size:" << mpeg_header2.GetSize() << endl;



        }
        // cout << bytes << endl;
        uint32_t seq = 0;

        while (bytes > 0) {

          // std::cout << "creating packet"<< seq << '\n';
          // cout << bytes << endl;
          Data dataPacket = Data();
          dataPacket.setContent(&m_buffer[seq * m_payloadSize],m_payloadSize);

          // std::cout << "set content" << '\n';
          data_v.push_back(dataPacket);
          // std::cout << "add content" << '\n';


          seq++;
          bytes -= m_payloadSize;
        }

        // for (size_t i = 0; i < seq; i++) {
        //   cout << v[i].getContent().value_size() << endl;
        // }

        m_map[representation] = data_v;
        //
        // auto itr = m_map.find(representation);
        // if( itr != m_map.end() ){
        //   cout <<  itr->second.size()<< endl;
        // }

      }




      // Name name = Name(dashname.GetInterestName());
      // DashName dashname;
      // dashname.SetVideoId(m_videoId);
      // dashname.SetProducerDomain(m_producerDomain);
      // dashname.SetPeriodId(m_periodId);
      // dashname.SetAdaptationSetId(m_adaptationSetId);
      // dashname.SetRepresentation(representation);
      // dashname.SetSegmentId(segment_id);
      void
      DashContent::setPayloadSize(uint32_t PayloadSize){
        m_payloadSize = PayloadSize;
      }

      uint32_t
      DashContent::GetFrameSize(uint32_t representation) {
        int avg_packetsize = representation / (50 * 8);

        MPEGHeader mpeg_header_tmp;

        uint32_t frame_size = std::max(
                          std::min(avg_packetsize, MPEG_MAX_MESSAGE) - (int) (mpeg_header_tmp.GetSerializedSize()) ,
                          1);
        return frame_size;

      }

      const string&
      DashContent::GetProducerDomain(uint32_t VideoId){
        const std::vector<string> producerList = {"remap","byu","ua","csu","wu","um","uiuc","mich","verisign","neu","lip6","urjc","orange","systemx","ntnu","basel","padua","bupt","pku","tongji","anyang","kisti","waseda","caida","ucla","uci"};
        if (VideoId <= 26) {
          return producerList[VideoId];
        }else{
          return producerList[VideoId / producerList.size()];
        }
      }


  }
}
