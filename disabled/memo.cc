// cout<<StringValue(Frequency)<<endl;

// printFib
auto printFib = [](Ptr<Node> node) {
  auto ndn = node->GetObject<ndn::L3Protocol>();
  for (const auto& entry : ndn->getForwarder()->getFib()) {
    cout << entry.getPrefix() << " (";

    bool isFirst = true;
    for (auto& nextHop : entry.getNextHops()) {
      // cout << *nextHop.getFace();
      auto face = dynamic_pointer_cast<ndn::NetDeviceFace>(nextHop.getFace());
      if (face == nullptr)
      continue;

      cout << " towards ";
      cout << Names::FindName(face->GetNetDevice()->GetChannel()->GetDevice(1)->GetNode());

      if (!isFirst)
      cout << ", ";
      isFirst = false;
    }
    cout << ")" << endl;
  }
};

for (int i = 0; i < 10; ++i)
{
  cout << "FIB content on node "+ nameNode[i] << endl;
  printFib(producers[i]);
}
