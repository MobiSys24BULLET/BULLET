/*
 * Copyright (c) 2008 INRIA
 * Copyright (c) 2013 Magister Solutions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Original work author (from packet-sink-helper.cc):
 *   - Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *
 * Converted to HTTP web browsing traffic models by:
 *   - Budiarto Herman <budiarto.herman@magister.fi>
 *
 */

#ifndef RTS_H
#define RTS_H

#include <ns3/application-container.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>

namespace ns3
{

/**
 * \ingroup applications
 * Helper to make it easier to instantiate an RtsClient on a set of nodes.
 */
class RtsClientHelper
{
  public:
    /**
     * Create a RtsClientHelper to make it easier to work with RtsClient
     * applications.
     * \param address The address of the remote server node to send traffic to.
     */
    RtsClientHelper(const Address& address);

    /**
     * Helper function used to set the underlying application attributes, but
     * *not* the socket attributes.
     * \param name The name of the application attribute to set.
     * \param value The value of the application attribute to set.
     */
    void SetAttribute(const std::string& name, const AttributeValue& value);

    /**
     * Install a RtsClient on each node of the input container configured with
     * all the attributes set with SetAttribute().
     * \param c NodeContainer of the set of nodes on which an RtsClient
     *          will be installed.
     * \return Container of Ptr to the applications installed.
     */
    ApplicationContainer Install(NodeContainer c) const;

    /**
     * Install a RtsClient on each node of the input container
     * configured with all the attributes set with SetAttribute().
     * \param node The node on which an RtsClient will be installed.
     * \return Container of Ptr to the applications installed.
     */
    ApplicationContainer Install(Ptr<Node> node) const;

    /**
     * Install a RtsClient on each node of the input container
     * configured with all the attributes set with SetAttribute().
     * \param nodeName The name of the node on which an RtsClient
     *                 will be installed.
     * \return Container of Ptr to the applications installed.
     */
    ApplicationContainer Install(const std::string& nodeName) const;

  private:
    /**
     * \internal
     * Install a RtsClient on the node configured with all the
     * attributes set with SetAttribute().
     * \param node The node on which an RtsClient will be installed.
     * \return Ptr to the application installed.
     */
    Ptr<Application> InstallPriv(Ptr<Node> node) const;

    /// Used to instantiate an RtsClient instance.
    ObjectFactory m_factory;

}; // end of `class RtsClientHelper`

/**
 * \ingroup http
 * Helper to make it easier to instantiate an RtsServer on a set of nodes.
 */
class RtsServerHelper
{
  public:
    /**
     * Create a RtsServerHelper to make it easier to work with
     * RtsServer applications.
     * \param address The address of the server.
     */
    RtsServerHelper(const Address& address);

    /**
     * Helper function used to set the underlying application attributes, but
     * *not* the socket attributes.
     * \param name The name of the application attribute to set.
     * \param value The value of the application attribute to set.
     */
    void SetAttribute(const std::string& name, const AttributeValue& value);

    /**
     * Install an RtsServer on each node of the input container
     * configured with all the attributes set with SetAttribute().
     * \param c NodeContainer of the set of nodes on which an RtsServer
     *              will be installed.
     * \return Container of Ptr to the applications installed.
     */
    ApplicationContainer Install(NodeContainer c) const;

    /**
     * Install an RtsServer on each node of the input container
     * configured with all the attributes set with SetAttribute().
     * \param node The node on which an RtsServer will be installed.
     * \return Container of Ptr to the applications installed.
     */
    ApplicationContainer Install(Ptr<Node> node) const;

    /**
     * Install an RtsServer on each node of the input container
     * configured with all the attributes set with SetAttribute().
     * \param nodeName The name of the node on which an RtsServer
     *                             will be installed.
     * \return Container of Ptr to the applications installed.
     */
    ApplicationContainer Install(const std::string& nodeName) const;

  private:
    /**
     * \internal
     * Install an RtsServer on the node configured with all the
     * attributes set with SetAttribute().
     * \param node The node on which an RtsServer will be installed.
     * \return Ptr to the application installed.
     */
    Ptr<Application> InstallPriv(Ptr<Node> node) const;

    /// Used to instantiate a RtsServer instance.
    ObjectFactory m_factory;

}; // end of `class RtsServerHelper`

} // namespace ns3

#endif /* THREE_GPP_HTTP_HELPER_H */
