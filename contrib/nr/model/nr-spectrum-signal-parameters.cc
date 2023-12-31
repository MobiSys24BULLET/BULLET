/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "nr-spectrum-signal-parameters.h"

#include "nr-control-messages.h"

#include <ns3/log.h>
#include <ns3/packet-burst.h>
#include <ns3/ptr.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSpectrumSignalParameters");

NrSpectrumSignalParametersDataFrame::NrSpectrumSignalParametersDataFrame()
{
    NS_LOG_FUNCTION(this);
}

NrSpectrumSignalParametersDataFrame::NrSpectrumSignalParametersDataFrame(
    const NrSpectrumSignalParametersDataFrame& p)
    : SpectrumSignalParameters(p)
{
    NS_LOG_FUNCTION(this << &p);
    cellId = p.cellId;
    if (p.packetBurst)
    {
        packetBurst = p.packetBurst->Copy();
    }
    ctrlMsgList = p.ctrlMsgList;
}

Ptr<SpectrumSignalParameters>
NrSpectrumSignalParametersDataFrame::Copy() const
{
    NS_LOG_FUNCTION(this);
    // Ideally we would use:
    //   return Copy<NrSpectrumSignalParametersDataFrame> (*this);
    // but for some reason it doesn't work. Another anrrnative is
    //   return Copy<NrSpectrumSignalParametersDataFrame> (this);
    // but it causes a double creation of the object, hence it is less efficient.
    // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
    Ptr<NrSpectrumSignalParametersDataFrame> lssp(new NrSpectrumSignalParametersDataFrame(*this),
                                                  false);
    return lssp;
}

NrSpectrumSignalParametersDlCtrlFrame::NrSpectrumSignalParametersDlCtrlFrame()
{
    NS_LOG_FUNCTION(this);
}

NrSpectrumSignalParametersDlCtrlFrame::NrSpectrumSignalParametersDlCtrlFrame(
    const NrSpectrumSignalParametersDlCtrlFrame& p)
    : SpectrumSignalParameters(p)
{
    NS_LOG_FUNCTION(this << &p);
    cellId = p.cellId;
    pss = p.pss;
    ctrlMsgList = p.ctrlMsgList;
}

Ptr<SpectrumSignalParameters>
NrSpectrumSignalParametersDlCtrlFrame::Copy() const
{
    NS_LOG_FUNCTION(this);
    // Ideally we would use:
    //   return Copy<NrSpectrumSignalParametersDlCtrlFrame> (*this);
    // but for some reason it doesn't work. Another alternative is
    //   return Copy<NrSpectrumSignalParametersDlCtrlFrame> (this);
    // but it causes a double creation of the object, hence it is less efficient.
    // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
    Ptr<NrSpectrumSignalParametersDlCtrlFrame> lssp(
        new NrSpectrumSignalParametersDlCtrlFrame(*this),
        false);
    return lssp;
}

NrSpectrumSignalParametersUlCtrlFrame::NrSpectrumSignalParametersUlCtrlFrame()
{
    NS_LOG_FUNCTION(this);
}

NrSpectrumSignalParametersUlCtrlFrame::NrSpectrumSignalParametersUlCtrlFrame(
    const NrSpectrumSignalParametersUlCtrlFrame& p)
    : SpectrumSignalParameters(p)
{
    NS_LOG_FUNCTION(this << &p);
    cellId = p.cellId;
    ctrlMsgList = p.ctrlMsgList;
}

Ptr<SpectrumSignalParameters>
NrSpectrumSignalParametersUlCtrlFrame::Copy() const
{
    NS_LOG_FUNCTION(this);
    // Ideally we would use:
    //   return Copy<NrSpectrumSignalParametersUlCtrlFrame> (*this);
    // but for some reason it doesn't work. Another alternative is
    //   return Copy<NrSpectrumSignalParametersUlCtrlFrame> (this);
    // but it causes a double creation of the object, hence it is less efficient.
    // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
    Ptr<NrSpectrumSignalParametersUlCtrlFrame> lssp(
        new NrSpectrumSignalParametersUlCtrlFrame(*this),
        false);
    return lssp;
}

} // namespace ns3
