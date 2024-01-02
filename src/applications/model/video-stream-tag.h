/*
 * Copyright (c) 2023 nxc
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
 */

#ifndef VIDEO_STREAM_TAG_H
#define VIDEO_STREAM_TAG_H

#include "ns3/nstime.h"
#include "ns3/packet.h"

namespace ns3
{

class Tag;

/**
 * Tag to calculate the per-PDU delay from eNb PDCP to UE PDCP
 */

class VideoStreamTag : public Tag
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    /**
     * Create an empty PDCP tag
     */
    VideoStreamTag();
    /**
     * Create an PDCP tag with the given senderTimestamp
     * \param senderTimestamp the time stamp
     */
    //VideoStreamTag::VideoStreamTag(uint32_t frameNum, uint32_t byteSequenceNum, Time senderTimestamp);

    void Serialize(TagBuffer i) const override;
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;

    /**
     * Get the instant when the PDCP delivers the PDU to the MAC SAP provider
     * @return the sender timestamp
     */
    Time GetSenderTimestamp() const;

    /**
     * Set the sender timestamp
     * @param senderTimestamp time stamp of the instant when the PDCP delivers the PDU to the MAC
     * SAP provider
     */
    void SetSenderTimestamp(Time senderTimestamp);

    uint32_t m_sourceId;
    uint32_t m_frameNum;
    uint32_t m_byteSequenceNum;
    uint32_t m_currentSequence;
    Time m_senderTimestamp; ///< sender timestamp
};

} // namespace ns3

#endif /* VIDEO_STREAM_TAG_H */
