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

#ifndef NR_SPECTRUM_SIGNAL_PARAMETERS_H
#define NR_SPECTRUM_SIGNAL_PARAMETERS_H

#include <ns3/spectrum-signal-parameters.h>

#include <list>

namespace ns3
{

class PacketBurst;
class NrControlMessage;

/**
 * \ingroup spectrum
 *
 * \brief Data signal representation for the module
 *
 * This struct provides the generic signal representation to be used by the module
 * for what regards the data part.
 */
struct NrSpectrumSignalParametersDataFrame : public SpectrumSignalParameters
{
    // inherited from SpectrumSignalParameters
    Ptr<SpectrumSignalParameters> Copy() const override;

    /**
     * \brief NrSpectrumSignalParametersDataFrame
     */
    NrSpectrumSignalParametersDataFrame();

    /**
     * \brief NrSpectrumSignalParametersDataFrame copy constructor
     * \param p the object from which we have to copy things
     */
    NrSpectrumSignalParametersDataFrame(const NrSpectrumSignalParametersDataFrame& p);

    Ptr<PacketBurst> packetBurst;                 //!< Packet burst
    std::list<Ptr<NrControlMessage>> ctrlMsgList; //!< List of contrl messages
    uint16_t cellId;                              //!< CellId
};

/**
 * \ingroup gnb-phy
 * \ingroup ue-phy
 *
 * \brief DL CTRL signal representation for the module
 *
 * This struct provides the generic signal representation to be used by the module
 * for what regards the downlink control part.
 */
struct NrSpectrumSignalParametersDlCtrlFrame : public SpectrumSignalParameters
{
    // inherited from SpectrumSignalParameters
    Ptr<SpectrumSignalParameters> Copy() const override;

    /**
     * \brief NrSpectrumSignalParametersDlCtrlFrame
     */
    NrSpectrumSignalParametersDlCtrlFrame();

    /**
     * \brief NrSpectrumSignalParametersDlCtrlFrame copy constructor
     * \param p the object from which we have to copy from
     */
    NrSpectrumSignalParametersDlCtrlFrame(const NrSpectrumSignalParametersDlCtrlFrame& p);

    std::list<Ptr<NrControlMessage>> ctrlMsgList; //!< CTRL message list
    bool pss;                                     //!< PSS (?)
    uint16_t cellId;                              //!< cell id
};

/**
 * \ingroup gnb-phy
 * \ingroup ue-phy
 *
 * \brief UL CTRL signal representation for the module
 *
 * This struct provides the generic signal representation to be used by the module
 * for what regards the UL CTRL part.
 */
struct NrSpectrumSignalParametersUlCtrlFrame : public SpectrumSignalParameters
{
    // inherited from SpectrumSignalParameters
    Ptr<SpectrumSignalParameters> Copy() const override;

    /**
     * \brief NrSpectrumSignalParametersUlCtrlFrame
     */
    NrSpectrumSignalParametersUlCtrlFrame();

    /**
     * \brief NrSpectrumSignalParametersUlCtrlFrame copy constructor
     * \param p the object from which we have to copy from
     */
    NrSpectrumSignalParametersUlCtrlFrame(const NrSpectrumSignalParametersUlCtrlFrame& p);

    std::list<Ptr<NrControlMessage>> ctrlMsgList; //!< CTRL message list
    uint16_t cellId;                              //!< cell id
};

} // namespace ns3

#endif /* NR_SPECTRUM_SIGNAL_PARAMETERS_H */
