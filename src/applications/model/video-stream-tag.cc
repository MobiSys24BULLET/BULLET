/*
 * Copyright (c) 2011 CTTC
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
 * Author: Jaume Nin <jaume.nin@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 */

#include "video-stream-tag.h"

#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(VideoStreamTag);

VideoStreamTag::VideoStreamTag()
    : m_sourceId (0),
    m_frameNum(0),
    m_byteSequenceNum(0), 
    m_currentSequence(0),
    m_senderTimestamp(Seconds(0))
{
    // Nothing to do here
}

/*
VideoStreamTag::VideoStreamTag(uint32_t frameNum, uint32_t byteSequenceNum, Time senderTimestamp)
    : m_frameNum (frameNum),
    m_byteSequenceNum (byteSequenceNum), 
    m_senderTimestamp(senderTimestamp)

{
    // Nothing to do here
}
*/
TypeId
VideoStreamTag::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::VideoStreamTag").SetParent<Tag>().SetGroupName("Application").AddConstructor<VideoStreamTag>();
    return tid;
}

TypeId
VideoStreamTag::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
VideoStreamTag::GetSerializedSize() const
{
    return sizeof(Time) + sizeof(uint32_t) + sizeof (uint32_t) + sizeof (uint32_t) + sizeof (uint32_t);
}

void
VideoStreamTag::Serialize(TagBuffer i) const
{
    i.WriteU32(m_sourceId);
    i.WriteU32(m_frameNum);
    i.WriteU32(m_byteSequenceNum);
    i.WriteU32(m_currentSequence);
    int64_t senderTimestamp = m_senderTimestamp.GetNanoSeconds();
    i.Write((const uint8_t*)&senderTimestamp, sizeof(int64_t));
}

void
VideoStreamTag::Deserialize(TagBuffer i)
{
    m_sourceId = i.ReadU32();
    m_frameNum = i.ReadU32();
    m_byteSequenceNum = i.ReadU32();
    m_currentSequence = i.ReadU32();
    int64_t senderTimestamp;
    i.Read((uint8_t*)&senderTimestamp, 8);
    m_senderTimestamp = NanoSeconds(senderTimestamp);
}

void
VideoStreamTag::Print(std::ostream& os) const
{
    os << m_sourceId <<" "<< m_frameNum << " " << m_byteSequenceNum << " "<<m_currentSequence<<" "<< m_senderTimestamp;
}

Time
VideoStreamTag::GetSenderTimestamp() const
{
    return m_senderTimestamp;
}

void
VideoStreamTag::SetSenderTimestamp(Time senderTimestamp)
{
    this->m_senderTimestamp = senderTimestamp;
}

} // namespace ns3
