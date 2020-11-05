#include "line-control-helper.h"

#include "ns3/line-control.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

LineControlHelper::LineControlHelper ()
{
  m_factory.SetTypeId (LineControl::GetTypeId ());
}

void 
LineControlHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
LineControlHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
LineControlHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
LineControlHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
LineControlHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<LineControl> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
